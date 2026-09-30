"""Independent bounded structural checks. Never calls an evaluator or trusts its flag."""
from dataclasses import dataclass
from model import (CONTRACT, StructuralProposal, canonical, certificate_reference,
                   digest, freeze, mutation_radius, observe, projection, reconstruct)


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
            return [{k: v for k, v in n.items() if k != 'body_id'} for n in obj['occurrences']]
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
        for n in new['occurrences']:
            owners.setdefault(n['body_id'], set()).add((n['authority_id'], n['kind']))
        if any(len(group) != 1 for group in owners.values()):
            reasons.append('shared_authority')
        restored = reconstruct(p.candidate)
        recovered = digest(restored)
        if restored != source:
            reasons.append('exact_source_reconstruction')
        after = observe(p.candidate).vector()
        if any(b > a for a, b in zip(before, after)):
            reasons.append('protected_component_worsened')
        if not any(b < a for a, b in zip(before, after)):
            reasons.append('no_strict_reduction')
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
