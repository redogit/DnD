"""Finite synthetic carriers only. No VM, Mirror or callback imports/bindings."""
from __future__ import annotations

from collections import Counter
from dataclasses import asdict, dataclass
import hashlib
import json

MAX_BYTES = 65536
MAX_NODES, MAX_EDGES, MAX_AUTHORITIES = 64, 128, 16
SOURCE_SCHEMA = 'decision-field/synthetic-source/v1'
ACTIVE_SCHEMA = 'decision-field/synthetic-projection/v1'
CONTRACT = 'exact-ordered-source+occurrence+authority+evidence/v1'
NODE_FIELDS = {'occurrence_id', 'semantic_object_id', 'source_id', 'chronology',
               'authority_id', 'evidence_ids', 'obligations', 'kind',
               'call_depth', 'adapter_depth', 'wrapper_layers'}


def canonical(obj) -> bytes:
    return (json.dumps(obj, sort_keys=True, separators=(',', ':'),
                       ensure_ascii=True, allow_nan=False) + '\n').encode('ascii')


def digest(raw: bytes) -> str:
    return hashlib.sha256(raw).hexdigest()


def _pairs(items):
    obj = {}
    for key, value in items:
        if key in obj:
            raise ValueError('duplicate JSON key')
        obj[key] = value
    return obj


def decode(raw: bytes) -> dict:
    if type(raw) is not bytes or len(raw) > MAX_BYTES:
        raise ValueError('graph byte envelope exceeded')
    try:
        obj = json.loads(raw, object_pairs_hook=_pairs,
                         parse_constant=lambda _: (_ for _ in ()).throw(ValueError('nonfinite JSON')))
        if type(obj) is not dict or canonical(obj) != raw:
            raise ValueError('requires exact canonical JSON object bytes')
        pending = [(obj, 0)]
        while pending:
            value, depth = pending.pop()
            if depth > 16:
                raise ValueError('graph nesting envelope exceeded')
            if isinstance(value, dict):
                pending.extend((v, depth + 1) for v in value.values())
            elif isinstance(value, list):
                pending.extend((v, depth + 1) for v in value)
        return obj
    except (UnicodeError, RecursionError, TypeError) as exc:
        raise ValueError('invalid graph JSON') from exc


def _id(value):
    if type(value) is not str or not 1 <= len(value) <= 128:
        raise ValueError('invalid stable ID')


def _records(value, key, maximum):
    if type(value) is not list or len(value) > maximum:
        raise ValueError('record envelope exceeded')
    ids = []
    for record in value:
        if type(record) is not dict or key not in record:
            raise ValueError('invalid record')
        _id(record[key])
        ids.append(record[key])
    if len(set(ids)) != len(ids):
        raise ValueError('duplicate record ID')
    return value


def _node(node, payload_field):
    if set(node) != NODE_FIELDS | {payload_field}:
        raise ValueError('unexpected occurrence fields')
    for key in ('occurrence_id', 'semantic_object_id', 'source_id', 'authority_id', 'kind'):
        _id(node[key])
    for key in ('chronology', 'call_depth', 'adapter_depth', 'wrapper_layers'):
        if type(node[key]) is not int or not 0 <= node[key] <= MAX_NODES:
            raise ValueError('invalid declared structural tag')
    for key in ('evidence_ids', 'obligations'):
        if type(node[key]) is not list or not 1 <= len(node[key]) <= 16:
            raise ValueError('missing bounded evidence/obligation list')
        for value in node[key]:
            _id(value)
    if payload_field == 'body_id':
        _id(node[payload_field])
    else:
        _payload(node[payload_field])


def _payload(payload):
    if type(payload) is not dict:
        raise ValueError('payload must be an opaque JSON object')
    if 'coordinates' in payload and (type(payload['coordinates']) is not list or
                                     len(payload['coordinates']) > MAX_NODES):
        raise ValueError('invalid bounded coordinate-width tag')


def _graph(obj, active=False):
    node_key = 'occurrences' if active else 'nodes'
    fields = {'schema', 'graph_id', 'authorities', node_key, 'edges'}
    if active:
        fields.add('bodies')
    if set(obj) != fields or obj['schema'] != (ACTIVE_SCHEMA if active else SOURCE_SCHEMA):
        raise ValueError('unexpected graph schema')
    _id(obj['graph_id'])
    authorities = _records(obj['authorities'], 'authority_id', MAX_AUTHORITIES)
    for authority in authorities:
        if set(authority) != {'authority_id', 'scope'}:
            raise ValueError('invalid authority boundary')
        _id(authority['scope'])
    nodes = _records(obj[node_key], 'occurrence_id', MAX_NODES)
    if not nodes or not authorities:
        raise ValueError('empty source/projection')
    for node in nodes:
        _node(node, 'body_id' if active else 'payload')
    edges = _records(obj['edges'], 'edge_id', MAX_EDGES)
    for edge in edges:
        if set(edge) != {'edge_id', 'from', 'to', 'relation', 'relevant', 'evidence_id'}:
            raise ValueError('invalid edge distinction')
        for key in ('from', 'to', 'relation', 'evidence_id'):
            _id(edge[key])
        if type(edge['relevant']) is not bool:
            raise ValueError('invalid dependency relevance tag')
    if active:
        bodies = _records(obj['bodies'], 'body_id', MAX_NODES)
        for body in bodies:
            if set(body) != {'body_id', 'payload'}:
                raise ValueError('invalid body')
            _payload(body['payload'])
    return obj


def freeze(raw: bytes) -> bytes:
    obj = _graph(decode(raw))
    nodes = {n['occurrence_id'] for n in obj['nodes']}
    authorities = {a['authority_id'] for a in obj['authorities']}
    if any(n['authority_id'] not in authorities for n in obj['nodes']):
        raise ValueError('missing authority boundary')
    if any(e['from'] not in nodes or e['to'] not in nodes for e in obj['edges']):
        raise ValueError('missing edge occurrence')
    return raw


def projection(raw: bytes) -> dict:
    return _graph(decode(raw), active=True)


def initial_projection(source: bytes) -> bytes:
    obj = json.loads(freeze(source))
    obj['schema'] = ACTIVE_SCHEMA
    obj['bodies'] = []
    obj['occurrences'] = []
    for node in obj.pop('nodes'):
        body_id = 'body:' + digest(node['occurrence_id'].encode('utf-8'))
        obj['bodies'].append({'body_id': body_id, 'payload': node.pop('payload')})
        obj['occurrences'].append(dict(node, body_id=body_id))
    raw = canonical(obj)
    projection(raw)  # Reject incompatible source/projection envelopes before FREEZE.
    return raw


def reconstruct(active: bytes) -> bytes:
    """Expand bindings, not receipt bytes; preserve source order/chronology exactly."""
    obj = projection(active)
    bodies = {b['body_id']: b['payload'] for b in obj.pop('bodies')}
    if set(bodies) != {n['body_id'] for n in obj['occurrences']}:
        raise ValueError('missing or unreachable body')
    obj['nodes'] = []
    for node in obj.pop('occurrences'):
        body_id = node.pop('body_id')
        obj['nodes'].append(dict(node, payload=bodies[body_id]))
    obj['schema'] = SOURCE_SCHEMA
    return freeze(canonical(obj))


@dataclass(frozen=True)
class K:
    active_nodes: int
    active_edges: int
    call_depth: int
    adapter_depth: int
    duplicate_payloads: int
    recurring_cycles: int
    dependency_volume: int
    projection_width: int
    wrapper_layers: int

    def vector(self) -> tuple[int, ...]:
        return tuple(asdict(self).values())


def observe(active: bytes) -> K:
    """Explicit finite accounting; depth/cycle tags are not execution semantics."""
    obj = projection(active)
    nodes, bodies, edges = obj['occurrences'], obj['bodies'], obj['edges']
    owners = {}
    for n in nodes:
        owners.setdefault(n['body_id'], set()).add((n['authority_id'], n['kind']))
    groups = Counter((canonical(b['payload']), tuple(sorted(owners.get(b['body_id'], ())))) for b in bodies)
    return K(len(nodes) + len(bodies) + len(obj['authorities']), len(edges) + len(nodes),
             max(n['call_depth'] for n in nodes), max(n['adapter_depth'] for n in nodes),
             sum(v - 1 for v in groups.values()),
             sum(e['relation'] == 'cycle' for e in edges),
             sum(e['relation'] == 'dependency' for e in edges),
             sum(len(b['payload'].get('coordinates', [])) for b in bodies),
             sum(n['wrapper_layers'] for n in nodes))


@dataclass(frozen=True)
class RecoveryReceipt:
    source_bytes: bytes
    source_sha256: str
    predecessor_bytes: bytes
    predecessor_sha256: str


@dataclass(frozen=True)
class StructuralProposal:
    proposal_id: str
    evaluator_id: str
    predecessor_id: str
    candidate: bytes
    certificate_ref: str
    predicted_delta: tuple[int, ...] | None
    verification_cost_hint: int
    recovery_cost_hint: int
    mutation_radius: int
    recovery: RecoveryReceipt


def certificate_reference(source: bytes, predecessor: bytes, candidate: bytes) -> str:
    # This is a binding reference, never a proof or a success flag.
    return 'certificate:' + digest(canonical([CONTRACT, digest(source), digest(predecessor), digest(candidate)]))


def mutation_radius(before: bytes, after: bytes) -> int:
    a, b = projection(before), projection(after)
    old = {n['occurrence_id']: n for n in a['occurrences']}
    new = {n['occurrence_id']: n for n in b['occurrences']}
    return sum(old.get(k) != new.get(k) for k in old.keys() | new.keys())


@dataclass(frozen=True)
class DuplicateEvaluator:
    scope: tuple[str, ...]
    evaluator_id: str = 'DuplicateEvaluator'

    def generate(self, active: bytes) -> bytes:
        obj = projection(active)
        nodes = {n['occurrence_id']: n for n in obj['occurrences']}
        if len(set(self.scope)) != len(self.scope) or not set(self.scope) <= nodes.keys():
            raise ValueError('invalid occurrence scope')
        bodies = {b['body_id']: b for b in obj['bodies']}
        groups = {}
        for occurrence_id in self.scope:
            n = nodes[occurrence_id]
            key = (n['authority_id'], n['kind'], canonical(bodies[n['body_id']]['payload']))
            groups.setdefault(key, []).append(n['body_id'])
        replacements = {body: min(group) for group in groups.values() for body in group}
        # Rewrite all bindings of a selected body: overlapping scopes retain all users.
        for n in obj['occurrences']:
            n['body_id'] = replacements.get(n['body_id'], n['body_id'])
        used = {n['body_id'] for n in obj['occurrences']}
        obj['bodies'] = [b for b in obj['bodies'] if b['body_id'] in used]
        return canonical(obj)


@dataclass(frozen=True)
class HistoryEvent:
    event_id: str
    kind: str
    authority_id: str
    predecessor_event_id: str | None
    payload_bytes: bytes

    @property
    def payload(self):
        return json.loads(self.payload_bytes)  # detached copy


class Session:
    """Single-process, API-append-only H and immutable A snapshots; no durability claim."""
    def __init__(self, source: bytes):
        self._source = freeze(source)
        self._active = initial_projection(source)
        self._history = ()
        self._proposals, self._checks = {}, {}
        event = self._append('FREEZE', 'authority:fixture', {
            'source_hex': source.hex(), 'source_sha256': digest(source),
            'active_hex': self._active.hex(), 'active_sha256': digest(self._active)})
        self._active_id = event.event_id

    @property
    def active(self) -> bytes:
        return self._active

    @property
    def history(self) -> tuple[HistoryEvent, ...]:
        return self._history

    A = property(lambda self: self.active)
    H = property(lambda self: self.history)

    def _append(self, kind, authority, payload) -> HistoryEvent:
        event = HistoryEvent(f'history:{len(self._history):06}', kind, authority,
                             self._history[-1].event_id if self._history else None, canonical(payload))
        self._history += (event,)
        return event

    def proposal(self, proposal_id: str) -> StructuralProposal:
        return self._proposals[proposal_id]

    def propose(self, evaluator_id: str, candidate: bytes) -> StructuralProposal:
        _id(evaluator_id)
        if type(candidate) is not bytes:
            raise ValueError('candidate must be immutable bytes')
        # Malformed candidates are retained as counterevidence within a separate envelope.
        if len(candidate) > 2 * MAX_BYTES:
            raise ValueError('proposal retention envelope exceeded')
        proposal_id = f'proposal:{len(self._history):06}'
        try:
            delta = tuple(b - a for a, b in zip(observe(self._active).vector(), observe(candidate).vector()))
            radius = mutation_radius(self._active, candidate)
        except (ValueError, TypeError, KeyError):
            delta, radius = None, 0
        receipt = RecoveryReceipt(self._source, digest(self._source), self._active, digest(self._active))
        p = StructuralProposal(proposal_id, evaluator_id, self._active_id, candidate,
                               certificate_reference(self._source, self._active, candidate), delta,
                               len(self._source) + len(self._active) + len(candidate),
                               len(self._source) + len(self._active), radius, receipt)
        self._proposals[proposal_id] = p
        payload = asdict(p)
        payload['candidate'] = candidate.hex()
        payload['recovery']['source_bytes'] = self._source.hex()
        payload['recovery']['predecessor_bytes'] = self._active.hex()
        self._append('GENERATE', 'authority:synthetic-generator', payload)
        return p

    def generate(self, evaluator: DuplicateEvaluator) -> StructuralProposal:
        return self.propose(evaluator.evaluator_id, evaluator.generate(self._active))

    def verify(self, proposal_id: str):
        from checker import check
        from dataclasses import replace
        p = self.proposal(proposal_id)
        result = check(self._source, self._active, p)
        if p.predecessor_id != self._active_id:
            result = replace(result, accepted=False, reasons=result.reasons + ('stale_predecessor',))
        result = replace(result, check_id=f'history:{len(self._history):06}')
        event = self._append('VERIFY', 'authority:synthetic-checker', asdict(result))
        self._checks[event.event_id] = result
        return result

    def admit(self, proposal_id: str, check_id: str) -> bool:
        p, c = self._proposals.get(proposal_id), self._checks.get(check_id)
        reasons = []
        if p is None or c is None or c.proposal_id != proposal_id:
            reasons.append('unrecorded_or_mismatched_check')
        elif p.predecessor_id != self._active_id or c.predecessor_sha256 != digest(self._active):
            reasons.append('stale_predecessor')
        elif not c.accepted:
            reasons.extend(c.reasons)
        elif c.candidate_sha256 != digest(p.candidate):
            reasons.append('changed_candidate')
        event = self._append('ADMIT_REJECTED' if reasons else 'ADMIT', 'authority:synthetic-admitter', {
            'proposal_id': proposal_id, 'check_id': check_id,
            'predecessor_id': self._active_id, 'reasons': reasons})
        if reasons:
            return False
        self._active, self._active_id = p.candidate, event.event_id
        return True
