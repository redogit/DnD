"""Reproduce retained, exactly recoverable adapter-contract failures (no runtime)."""
import argparse
import json
from pathlib import Path
import sys

EXPERIMENT = Path(__file__).resolve().parents[2] / 'experiments' / 'structural'
sys.path.insert(0, str(EXPERIMENT))
from model import AdapterEvaluator, DuplicateEvaluator, Session, canonical, digest, observe, reconstruct


def run():
    base = (EXPERIMENT / 'adapter_source_graph.json').read_bytes()
    changes = {
        'authority': ('node', 'authority_id', 'auth:checker'),
        'ownership': ('contract', 'owner_id', 'owner:other'),
        'outer_type': ('contract', 'output_type', 'type:other'),
        'inner_type': ('contract', 'input_type', 'type:other'),
        'boundary_effect': ('contract', 'boundary_effects', ['effect:write']),
        'inverse': ('contract', 'inverse_id', 'inverse:other'),
        'direction': ('contract', 'direction', 'lift'),
        'unknown_contract': ('contract', 'extra', True),
        'boundary_edge': ('edge', 'relation', 'authority-boundary'),
        'fanout': ('side_edge', 'from', 'occ:adapter-1'),
        'fanin': ('side_edge', 'to', 'occ:adapter-2'),
        'depth': ('node', 'adapter_depth', 3),
    }
    cases = []
    for name, (target, key, value) in changes.items():
        graph = json.loads(base)
        targets = {'node': graph['nodes'][2], 'contract': graph['nodes'][2]['payload']['adapter_contract'],
                   'edge': graph['edges'][1], 'side_edge': graph['edges'][3]}
        targets[target][key] = value
        source = canonical(graph)
        s = Session(source)
        if AdapterEvaluator(('occ:adapter-1', 'occ:adapter-2')).generate(s.A) != s.A:
            raise ValueError(name + ': generator should decline')
        # Force compaction independently, despite incompatible declarations.
        obj = json.loads(DuplicateEvaluator(('occ:dup-1', 'occ:dup-2')).generate(s.A))
        obj['schema'] = 'decision-field/synthetic-projection/v2'
        obj['occurrences'][1:3] = [{'adapter_pair': obj['occurrences'][1:3]}]
        candidate = canonical(obj)
        before = s.A
        p = s.propose('DuplicateEvaluator:forged-adapter', candidate)
        c = s.verify(p.proposal_id)
        admitted = s.admit(p.proposal_id, c.check_id)
        if (admitted or 'adapter_contract' not in c.reasons or s.A != before or
                len(candidate) >= len(before) or observe(candidate).active_nodes >= observe(before).active_nodes or
                reconstruct(candidate) != source or s.proposal(p.proposal_id) != p):
            raise ValueError(name + ': failed discrimination/retention')
        cases.append(dict(name=name, source_sha256=digest(source), candidate_sha256=digest(candidate),
                          candidate_reconstructs_exactly=True, admitted=admitted,
                          predecessor_bytes=len(before), candidate_bytes=len(candidate),
                          k_before=observe(before).vector(), k_candidate=observe(candidate).vector(),
                          history=[dict(event_id=e.event_id, kind=e.kind, authority_id=e.authority_id,
                                        predecessor_event_id=e.predecessor_event_id, payload=e.payload) for e in s.H]))
    return dict(schema='decision-field/adapter-counterprobes/v1', base_source_sha256=digest(base),
                claim_ceiling='12 finite synthetic failures; exact recovery is insufficient for adapter admission',
                cases=cases)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True, help='new file only')
    args = parser.parse_args()
    report = run()
    with args.output.open('xb') as stream:
        stream.write(canonical(report))
    print('PASS 12 smaller, exactly recoverable forgeries rejected and retained')
