"""Independent bounded structural checks. Never calls an evaluator or trusts its flag."""
from dataclasses import dataclass
from model import (CONTRACT, StructuralProposal, canonical, certificate_reference,
                   adapter_pairs, digest, freeze, mutation_radius, observe, occurrences,
                   projection, reconstruct)


def valid_adapter_pairs(obj):
    """Independent structural obligation check; no evaluator/certification flag trusted."""
    payloads = {b['body_id']: b['payload'] for b in obj['bodies']}
    nodes = {n['occurrence_id']: n for n in occurrences(obj)}
    for pair in adapter_pairs(obj):
        lift, lower = pair
        ids = (lift['occurrence_id'], lower['occurrence_id'])
        if (lift['kind'] != 'adapter' or lower['kind'] != 'adapter' or
                lift['adapter_depth'] != 1 or lower['adapter_depth'] != 2 or
                lift['authority_id'] != lower['authority_id']):
            return False
        contracts = [payloads[n['body_id']].get('adapter_contract') for n in pair]
        for contract in contracts:
            if type(contract) is not dict or set(contract) != {
                    'direction', 'input_type', 'output_type', 'owner_id', 'inverse_id', 'boundary_effects'}:
                return False
            if contract['boundary_effects'] != []:
                return False
            if any(type(value) is not str or not 1 <= len(value) <= 128
                   for key, value in contract.items() if key != 'boundary_effects'):
                return False
        a, b = contracts
        if (a['direction'] != 'lift' or b['direction'] != 'lower' or
                (a['input_type'], a['output_type'], a['owner_id'], a['inverse_id']) !=
                (b['output_type'], b['input_type'], b['owner_id'], b['inverse_id'])):
            return False
        counts = [0, 0, 0]
        for edge in obj['edges']:
            source, target = edge['from'], edge['to']
            if source not in ids and target not in ids:
                continue
            if not edge['relevant']:
                return False
            if (source, target) == ids and edge['relation'] == 'adapter':
                counts[1] += 1
            elif target == ids[0] and source not in ids and edge['relation'] == 'call':
                counts[0] += 1
                if nodes.get(source, {}).get('authority_id') != lift['authority_id']:
                    return False
            elif source == ids[1] and target not in ids and edge['relation'] == 'call':
                counts[2] += 1
                if nodes.get(target, {}).get('authority_id') != lift['authority_id']:
                    return False
            else:
                return False
        if counts != [1, 1, 1]:
            return False
    return True


@dataclass(frozen=True)
class CheckResult:
    proposal_id: str
    check_id: str
    accepted: bool
    reasons: tuple[str, ...]
    contract: str
    predecessor_sha256: str
    candidate_sha256: str
    reconstructed_sha256: str | None
    k_before: tuple[int, ...]
    k_after: tuple[int, ...] | None
    verification_bytes: int
    recovery_bytes: int


def check(source: bytes, active: bytes, p: StructuralProposal) -> CheckResult:
    """Source/active are caller-owned trusted anchors, not proposal-owned authority."""
    freeze(source)
    if reconstruct(active) != source:
        raise ValueError('trusted active/source mismatch')
    if not valid_adapter_pairs(projection(active)):
        raise ValueError('trusted active adapter contract mismatch')
    before = observe(active).vector()
    reasons, after, recovered = [], None, None
    r = p.recovery
    if (r.source_bytes != source or r.predecessor_bytes != active or
            r.source_sha256 != digest(source) or r.predecessor_sha256 != digest(active)):
        reasons.append('recovery_receipt')
    if p.certificate_ref != certificate_reference(source, active, p.candidate):
        reasons.append('certificate_binding')
    try:
        old, new = projection(active), projection(p.candidate)
        # Compare ordered metadata independently of payload sharing/recovery receipts.
        def distinctions(obj):
            return [{k: v for k, v in n.items() if k != 'body_id'} for n in occurrences(obj)]
        if distinctions(new) != distinctions(old):
            reasons.append('occurrence_distinctions')
        if new['authorities'] != old['authorities']:
            reasons.append('authority_boundaries')
        if new['graph_id'] != old['graph_id'] or new['edges'] != old['edges']:
            reasons.append('graph_distinctions')
        retained = {b['body_id'] for b in new['bodies']}
        if canonical(new['bodies']) != canonical([b for b in old['bodies'] if b['body_id'] in retained]):
            reasons.append('body_identity')
        owners = {}
        for n in occurrences(new):
            owners.setdefault(n['body_id'], set()).add((n['authority_id'], n['kind']))
        if any(len(group) != 1 for group in owners.values()):
            reasons.append('shared_authority')
        if not valid_adapter_pairs(new):
            reasons.append('adapter_contract')
        restored = reconstruct(p.candidate)
        recovered = digest(restored)
        if restored != source:
            reasons.append('exact_source_reconstruction')
        after = observe(p.candidate).vector()
        if any(b > a for a, b in zip(before, after)):
            reasons.append('protected_component_worsened')
        if not any(b < a for a, b in zip(before, after)):
            reasons.append('no_strict_reduction')
        old_pairs = {tuple(n['occurrence_id'] for n in pair) for pair in adapter_pairs(old)}
        new_pairs = {tuple(n['occurrence_id'] for n in pair) for pair in adapter_pairs(new)}
        if (new_pairs != old_pairs or p.evaluator_id.split(':')[0] == 'AdapterEvaluator') and after[3] >= before[3]:
            reasons.append('no_strict_adapter_reduction')
        if p.predicted_delta != tuple(b - a for a, b in zip(before, after)):
            reasons.append('prediction_mismatch')
        if p.mutation_radius != mutation_radius(active, p.candidate):
            reasons.append('mutation_radius')
        for hint in (p.verification_cost_hint, p.recovery_cost_hint):
            if type(hint) is not int or hint < 0:
                reasons.append('invalid_cost_hint')
    except (ValueError, TypeError, KeyError, OverflowError):
        reasons.append('malformed_or_unrecoverable_candidate')
    # These are deterministic byte-volume accounts, not timings/asymptotic bounds.
    return CheckResult(p.proposal_id, '', not reasons, tuple(reasons), CONTRACT,
                       digest(active), digest(p.candidate), recovered, before, after,
                       len(source) + len(active) + len(p.candidate), len(source) + len(active))
