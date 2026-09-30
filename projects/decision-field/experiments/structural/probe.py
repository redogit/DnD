"""24 finite schedules; compact exact histories; independent-process receipt replay."""
from __future__ import annotations

import argparse
import itertools
import json
from pathlib import Path

from model import MAX_BYTES, DuplicateEvaluator, Session, canonical, digest, observe, projection, reconstruct

ACTIONS = ('left', 'right', 'alias', 'authority')
HEX_FIELDS = {'source_hex', 'active_hex', 'candidate', 'source_bytes', 'predecessor_bytes'}
REPORT_SCHEMA = 'decision-field/synthetic-structural-receipt/v1'
CLAIM_CEILING = 'finite synthetic structural recovery only; no behavioral equivalence, universality, runtime integration or P-versus-NP result'
MAX_REPORT_BYTES = 2 * 1024 * 1024


def attempt(s: Session, action: str):
    if action in ('left', 'right'):
        scope = ('occ:dup-1', 'occ:dup-2') if action == 'left' else ('occ:dup-2', 'occ:dup-3')
        return s.generate(DuplicateEvaluator(scope, 'DuplicateEvaluator:' + action))
    # Deliberately smaller and structurally well-formed forgeries, not parser errors.
    obj = json.loads(DuplicateEvaluator(('occ:dup-1', 'occ:dup-2')).generate(s.active))
    if action == 'alias':
        obj['occurrences'] = [n for n in obj['occurrences'] if n['occurrence_id'] != 'occ:dup-2']
        for edge in obj['edges']:
            for endpoint in ('from', 'to'):
                if edge[endpoint] == 'occ:dup-2':
                    edge[endpoint] = 'occ:dup-1'
    elif action == 'authority':
        obj['authorities'] = [a for a in obj['authorities'] if a['authority_id'] != 'auth:checker']
        for n in obj['occurrences']:
            if n['authority_id'] == 'auth:checker':
                n['authority_id'] = 'auth:owner'
    else:
        raise ValueError('unknown finite action')
    return s.propose('forged:' + action, canonical(obj))


def _packed(value, blobs):
    if isinstance(value, dict):
        out = {}
        for key, item in value.items():
            if key in HEX_FIELDS:
                raw = bytes.fromhex(item)
                sha = digest(raw)
                blobs[sha] = item
                out[key] = {'blob': sha}
            else:
                out[key] = _packed(item, blobs)
        return out
    if isinstance(value, list):
        return [_packed(item, blobs) for item in value]
    return value


def _unpacked(value, blobs):
    if isinstance(value, dict):
        if set(value) == {'blob'}:
            return blobs[value['blob']]
        return {key: _unpacked(item, blobs) for key, item in value.items()}
    if isinstance(value, list):
        return [_unpacked(item, blobs) for item in value]
    return value


def _history(s, blobs):
    return [dict(event_id=e.event_id, kind=e.kind, authority_id=e.authority_id,
                 predecessor_event_id=e.predecessor_event_id, payload=_packed(e.payload, blobs))
            for e in s.history]


def _same(actual, expected, label):
    if canonical(actual) != canonical(expected):
        raise ValueError('receipt mismatch: ' + label)


def _validate_action(before: bytes, candidate: bytes, action: str):
    """Check declared panel coverage from graph distinctions, not evaluator labels."""
    old, new = projection(before), projection(candidate)
    if action in ('left', 'right'):
        ids = ('occ:dup-1', 'occ:dup-2') if action == 'left' else ('occ:dup-2', 'occ:dup-3')
        by_id = {n['occurrence_id']: n for n in old['occurrences']}
        selected = {by_id[i]['body_id'] for i in ids}
        representative = min(selected)
        expected = [(n['occurrence_id'], representative if n['body_id'] in selected else n['body_id'])
                    for n in old['occurrences']]
        _same([(n['occurrence_id'], n['body_id']) for n in new['occurrences']], expected, 'declared duplicate scope')
        return
    expected_source = json.loads(reconstruct(before))
    if action == 'alias':
        expected_source['nodes'] = [n for n in expected_source['nodes'] if n['occurrence_id'] != 'occ:dup-2']
        for edge in expected_source['edges']:
            for endpoint in ('from', 'to'):
                if edge[endpoint] == 'occ:dup-2':
                    edge[endpoint] = 'occ:dup-1'
    elif action == 'authority':
        expected_source['authorities'] = [a for a in expected_source['authorities'] if a['authority_id'] != 'auth:checker']
        for node in expected_source['nodes']:
            if node['authority_id'] == 'auth:checker':
                node['authority_id'] = 'auth:owner'
    else:
        raise ValueError('unknown panel action')
    retained = {b['body_id'] for b in new['bodies']}
    _same(new['bodies'], [b for b in old['bodies'] if b['body_id'] in retained], 'negative retained body identity')
    if reconstruct(candidate) != canonical(expected_source):
        raise ValueError('negative witness is not the exact declared source alteration')


def run_probe(source: bytes) -> dict:
    blobs, cases = {}, []
    admissions = rejections = 0
    for order in itertools.permutations(ACTIONS):
        s = Session(source)
        initial_k, steps = observe(s.active).vector(), []
        for action in order:
            before, prefix = s.active, s.history
            p = attempt(s, action)
            _same(s.active.hex(), before.hex(), 'GENERATE changed A')
            c = s.verify(p.proposal_id)
            _same(s.active.hex(), before.hex(), 'VERIFY changed A')
            admitted = s.admit(p.proposal_id, c.check_id)
            if admitted != (action in ('left', 'right')):
                raise ValueError('unexpected discrimination')
            if s.history[:len(prefix)] != prefix or reconstruct(s.active) != source:
                raise ValueError('history or exact reconstruction failed')
            admissions += admitted
            rejections += not admitted
            steps.append({'action': action, 'proposal_id': p.proposal_id, 'admitted': admitted,
                          'k_before': observe(before).vector(), 'k_after': observe(s.active).vector(),
                          'active_sha256': digest(s.active), 'reconstruction_sha256': digest(reconstruct(s.active)),
                          'proposal_bytes': len(p.candidate), 'predecessor_bytes': len(before),
                          'failed_proposal_retained': not admitted and s.proposal(p.proposal_id) == p})
        cases.append({'order': order, 'initial_k': initial_k, 'final_k': observe(s.active).vector(),
                      'final_active_sha256': digest(s.active), 'steps': steps, 'history': _history(s, blobs)})
    report = {'schema': REPORT_SCHEMA, 'source_sha256': digest(source),
              'summary': {'orders': len(cases), 'admissions': admissions, 'rejections': rejections,
                          'reconstructions': sum(len(c['steps']) for c in cases)},
              'cases': cases, 'blobs': blobs, 'claim_ceiling': CLAIM_CEILING}
    verify_report(source, report)
    return report


def verify_report(source: bytes, report: dict) -> bool:
    """Replay stored proposal bytes against the caller's trusted source, without generators."""
    try:
        if (type(report) is not dict or set(report) != {'schema', 'source_sha256', 'summary', 'cases', 'blobs', 'claim_ceiling'} or
                len(canonical(report)) > MAX_REPORT_BYTES or report['schema'] != REPORT_SCHEMA or
                report['claim_ceiling'] != CLAIM_CEILING):
            raise ValueError('receipt envelope/schema')
        if report['source_sha256'] != digest(source):
            raise ValueError('receipt trusted source mismatch')
        blobs = report['blobs']
        if type(blobs) is not dict:
            raise ValueError('invalid receipt blob container')
        for sha, value in blobs.items():
            if digest(bytes.fromhex(value)) != sha:
                raise ValueError('receipt blob digest mismatch')
        _same([case['order'] for case in report['cases']], list(itertools.permutations(ACTIONS)), 'order coverage')
        admissions = rejections = reconstructions = 0
        for case in report['cases']:
            if set(case) != {'order', 'initial_k', 'final_k', 'final_active_sha256', 'steps', 'history'}:
                raise ValueError('unexpected case schema')
            s = Session(source)
            _same(case['initial_k'], observe(s.active).vector(), 'initial K')
            expected_events = [_unpacked(e, blobs) for e in case['history']]
            _same(expected_events[0]['payload'], s.history[0].payload, 'frozen receipt')
            if len(expected_events) != 13 or len(case['steps']) != 4:
                raise ValueError('incomplete history')
            for i, action in enumerate(case['order']):
                before = s.active
                generated = expected_events[1 + 3 * i]['payload']
                p = s.propose(generated['evaluator_id'], bytes.fromhex(generated['candidate']))
                _validate_action(before, p.candidate, action)
                # Recompute, do not trust the recorded certificate/status/cost prediction.
                _same(s.history[-1].payload, generated, 'generated proposal/recovery receipt')
                c = s.verify(p.proposal_id)
                admitted = s.admit(p.proposal_id, c.check_id)
                if admitted != (action in ('left', 'right')) or reconstruct(s.active) != source:
                    raise ValueError('replay discrimination/reconstruction mismatch')
                if generated['evaluator_id'] != ('DuplicateEvaluator:' if admitted else 'forged:') + action:
                    raise ValueError('evaluator/action mismatch')
                if not admitted and (len(p.candidate) >= len(before) or
                                     observe(p.candidate).active_nodes >= observe(before).active_nodes):
                    raise ValueError('negative probe was not smaller')
                step = {'action': action, 'proposal_id': p.proposal_id, 'admitted': admitted,
                        'k_before': observe(before).vector(), 'k_after': observe(s.active).vector(),
                        'active_sha256': digest(s.active), 'reconstruction_sha256': digest(reconstruct(s.active)),
                        'proposal_bytes': len(p.candidate), 'predecessor_bytes': len(before),
                        'failed_proposal_retained': not admitted and s.proposal(p.proposal_id) == p}
                _same(case['steps'][i], step, 'step observation')
                admissions += admitted
                rejections += not admitted
                reconstructions += 1
            actual = [dict(event_id=e.event_id, kind=e.kind, authority_id=e.authority_id,
                           predecessor_event_id=e.predecessor_event_id, payload=e.payload) for e in s.history]
            _same(expected_events, actual, 'append-only event replay')
            _same(case['final_k'], observe(s.active).vector(), 'final K')
            _same(case['final_active_sha256'], digest(s.active), 'final A')
        _same(report['summary'], {'orders': 24, 'admissions': admissions, 'rejections': rejections,
                                  'reconstructions': reconstructions}, 'summary')
    except (KeyError, TypeError, IndexError, RecursionError, OverflowError) as exc:
        raise ValueError('invalid receipt') from exc
    return True


def _read_bounded(path: Path, maximum: int, label: str) -> bytes:
    # One overflow byte detects excess without consuming an unbounded file.
    with path.open('rb') as stream:
        raw = stream.read(maximum + 1)
    if len(raw) > maximum:
        raise ValueError(label + ' byte envelope exceeded')
    return raw


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(__file__).with_name('source_graph.json'))
    parser.add_argument('--verify', type=Path, help='replay a recorded receipt against trusted --source')
    parser.add_argument('--output', type=Path, help='create a receipt file; never overwrite an existing file')
    args = parser.parse_args()
    try:
        source = _read_bounded(args.source, MAX_BYTES, 'source')
        if args.verify:
            raw = _read_bounded(args.verify, MAX_REPORT_BYTES, 'receipt')
            try:
                report = json.loads(raw)
            except RecursionError as exc:
                raise ValueError('receipt JSON nesting exceeds parser capacity') from exc
            if canonical(report) != raw:
                raise ValueError('receipt requires exact canonical JSON bytes')
            verify_report(source, report)
            print('PASS exact source recovery and 24 recorded schedules (generator not rerun)')
        else:
            report = run_probe(source)
            if args.output:
                with args.output.open('xb') as out:
                    out.write(canonical(report))
            print(json.dumps(report['summary'], sort_keys=True))
    except (ValueError, OSError) as exc:
        parser.exit(1, 'REJECTED: ' + str(exc) + '\n')


if __name__ == '__main__':
    main()
