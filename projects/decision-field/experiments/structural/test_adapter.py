"""Contract counterprobes for finite, declared lift/lower pair compaction."""
import itertools
import json
from pathlib import Path
import unittest

import model
from model import DuplicateEvaluator, Session, canonical, observe, reconstruct

SOURCE = Path(__file__).with_name('source_graph.json').read_bytes()
PAIR = ('occ:adapter-1', 'occ:adapter-2')
LEFT = DuplicateEvaluator(('occ:dup-1', 'occ:dup-2'), 'DuplicateEvaluator:left')
RIGHT = DuplicateEvaluator(('occ:dup-2', 'occ:dup-3'), 'DuplicateEvaluator:right')


def declared_source(mutate=None):
    obj = json.loads(SOURCE)
    obj['graph_id'] = 'graph:structural-adapter-2026-09-30'
    for node, direction, incoming, outgoing in (
            (obj['nodes'][1], 'lift', 'type:scalar', 'type:lifted'),
            (obj['nodes'][2], 'lower', 'type:lifted', 'type:scalar')):
        node['payload']['adapter_contract'] = dict(
            direction=direction, input_type=incoming, output_type=outgoing,
            owner_id='owner:synthetic', inverse_id='inverse:declared-pair',
            boundary_effects=[])
    if mutate:
        mutate(obj)
    return canonical(obj)


def pack_unchecked(active):
    """Adversary builds a pair directly; never calls the evaluator/checker."""
    obj = json.loads(active)
    obj['schema'] = 'decision-field/synthetic-projection/v2'
    obj['occurrences'][1:3] = [{'adapter_pair': obj['occurrences'][1:3]}]
    return canonical(obj)


class AdapterTests(unittest.TestCase):
    def evaluator(self):
        self.assertTrue(hasattr(model, 'AdapterEvaluator'), 'bounded AdapterEvaluator is missing')
        return model.AdapterEvaluator(PAIR)

    def test_checked_pair_reduces_depth_preserving_exact_identity_and_history(self):
        source, evaluator = declared_source(), self.evaluator()
        s = Session(source)
        before, prefix = s.A, s.H
        p = s.generate(evaluator)
        self.assertEqual(s.A, before)
        c = s.verify(p.proposal_id)
        self.assertTrue(c.accepted, c.reasons)
        self.assertEqual(s.A, before)
        self.assertEqual(reconstruct(p.candidate), source)
        self.assertEqual(observe(p.candidate).vector(), (23, 20, 3, 1, 2, 2, 1, 8, 1))
        self.assertTrue(s.admit(p.proposal_id, c.check_id))
        self.assertEqual(s.H[:len(prefix)], prefix)
        self.assertEqual(p.recovery.predecessor_bytes, before)
        self.assertEqual(p.recovery.source_bytes, source)
        self.assertEqual([e.kind for e in s.H], ['FREEZE', 'GENERATE', 'VERIFY', 'ADMIT'])
        recovered = json.loads(reconstruct(s.A))
        self.assertEqual(recovered['nodes'], json.loads(source)['nodes'])
        self.assertEqual(recovered['authorities'], json.loads(source)['authorities'])

    def test_opaque_historical_adapters_are_not_certified_by_name(self):
        s = Session(SOURCE)
        self.assertEqual(self.evaluator().generate(s.A), s.A)
        p = s.propose('forged:opaque', pack_unchecked(s.A))
        c = s.verify(p.proposal_id)
        self.assertIn('adapter_contract', c.reasons)
        self.assertFalse(s.admit(p.proposal_id, c.check_id))

    def test_exactly_recoverable_smaller_forgeries_cannot_cross_distinctions(self):
        mutations = {
            'authority': lambda g: g['nodes'][2].update(authority_id='auth:checker'),
            'ownership': lambda g: g['nodes'][2]['payload']['adapter_contract'].update(owner_id='owner:other'),
            'type': lambda g: g['nodes'][2]['payload']['adapter_contract'].update(output_type='type:other'),
            'inner_type': lambda g: g['nodes'][2]['payload']['adapter_contract'].update(input_type='type:other'),
            'effect': lambda g: g['nodes'][2]['payload']['adapter_contract'].update(boundary_effects=['effect:write']),
            'inverse': lambda g: g['nodes'][2]['payload']['adapter_contract'].update(inverse_id='inverse:other'),
            'direction': lambda g: g['nodes'][2]['payload']['adapter_contract'].update(direction='lift'),
            'unknown_contract': lambda g: g['nodes'][2]['payload']['adapter_contract'].update(extra=True),
            'boundary_edge': lambda g: g['edges'][1].update(relation='authority-boundary'),
            'fanout': lambda g: g['edges'][3].update(**{'from': 'occ:adapter-1'}),
            'fan_in': lambda g: g['edges'][3].update(to='occ:adapter-2'),
            'depth': lambda g: g['nodes'][2].update(adapter_depth=3),
        }
        evaluator = self.evaluator()
        for name, mutate in mutations.items():
            with self.subTest(name=name):
                source = declared_source(mutate)
                s = Session(source)
                self.assertEqual(evaluator.generate(s.A), s.A)
                candidate = pack_unchecked(LEFT.generate(s.A))
                self.assertLess(len(candidate), len(s.A))
                self.assertLess(observe(candidate).active_nodes, observe(s.A).active_nodes)
                self.assertEqual(reconstruct(candidate), source)
                before, prefix = s.A, s.H
                p = s.propose('DuplicateEvaluator:forged-label', candidate)
                c = s.verify(p.proposal_id)
                self.assertIn('adapter_contract', c.reasons)
                self.assertFalse(s.admit(p.proposal_id, c.check_id))
                self.assertEqual(s.A, before)
                self.assertEqual(s.H[:len(prefix)], prefix)
                self.assertEqual(s.proposal(p.proposal_id).candidate, candidate)
                self.assertEqual(s.H[-1].kind, 'ADMIT_REJECTED')

    def test_pair_cannot_be_reordered_or_lose_a_source_occurrence(self):
        source = declared_source()
        for mode in ('reverse', 'omit', 'repeat', 'nested'):
            with self.subTest(mode=mode):
                s = Session(source)
                obj = json.loads(pack_unchecked(s.A))
                pair = obj['occurrences'][1]['adapter_pair']
                if mode == 'reverse':
                    pair.reverse()
                elif mode == 'omit':
                    pair.pop()
                elif mode == 'repeat':
                    pair[1] = pair[0]
                else:
                    pair[1] = {'adapter_pair': list(pair)}
                p = s.propose('forged:pair', canonical(obj))
                self.assertFalse(s.admit(p.proposal_id, s.verify(p.proposal_id).check_id))
                self.assertEqual(reconstruct(s.A), source)

    def test_depth_must_strictly_drop_even_when_duplicate_count_drops(self):
        # A separate uncollapsed adapter-depth declaration keeps global depth at 2.
        source = declared_source(lambda g: g['nodes'][0].update(adapter_depth=2))
        s = Session(source)
        p = s.propose('forged:depth', pack_unchecked(LEFT.generate(s.A)))
        c = s.verify(p.proposal_id)
        self.assertIn('no_strict_adapter_reduction', c.reasons)
        self.assertFalse(s.admit(p.proposal_id, c.check_id))
        s = Session(declared_source())
        p = s.propose('AdapterEvaluator', LEFT.generate(s.A))
        self.assertIn('no_strict_adapter_reduction', s.verify(p.proposal_id).reasons)

    def test_adapter_reduction_cannot_pay_for_any_other_k_increase(self):
        s = Session(declared_source())
        p = s.generate(LEFT)
        self.assertTrue(s.admit(p.proposal_id, s.verify(p.proposal_id).check_id))
        p = s.propose('AdapterEvaluator', pack_unchecked(Session(declared_source()).A))
        c = s.verify(p.proposal_id)
        self.assertEqual(c.k_after[3], 1)
        self.assertIn('protected_component_worsened', c.reasons)
        self.assertFalse(s.admit(p.proposal_id, c.check_id))

    def test_three_evaluator_orders_recover_each_step(self):
        evaluator = self.evaluator()
        for order in itertools.permutations((LEFT, RIGHT, evaluator)):
            s = Session(declared_source())
            for item in order:
                before, prefix = s.A, s.H
                p = s.generate(item)
                c = s.verify(p.proposal_id)
                self.assertTrue(s.admit(p.proposal_id, c.check_id), c.reasons)
                self.assertEqual(p.recovery.predecessor_bytes, before)
                self.assertEqual(s.H[:len(prefix)], prefix)
                self.assertEqual(reconstruct(s.A), declared_source())
            self.assertEqual(observe(s.A).vector(), (21, 20, 3, 1, 0, 2, 1, 8, 1))

    def test_noop_stale_and_unverified_adapter_proposals_do_not_admit(self):
        s, evaluator = Session(declared_source()), self.evaluator()
        p = s.generate(evaluator)
        c = s.verify(p.proposal_id)
        self.assertFalse(s.admit(p.proposal_id, 'unrecorded-success'))
        duplicate = s.generate(LEFT)
        self.assertTrue(s.admit(duplicate.proposal_id, s.verify(duplicate.proposal_id).check_id))
        self.assertFalse(s.admit(p.proposal_id, c.check_id))
        p = s.generate(evaluator)
        self.assertTrue(s.admit(p.proposal_id, s.verify(p.proposal_id).check_id))
        p = s.generate(evaluator)
        self.assertEqual(p.candidate, s.A)
        self.assertIn('no_strict_adapter_reduction', s.verify(p.proposal_id).reasons)

    def test_adapter_checker_does_not_call_generator(self):
        from unittest.mock import patch
        s = Session(declared_source())
        p = s.generate(self.evaluator())
        with patch.object(model.AdapterEvaluator, 'generate', side_effect=AssertionError('generator rerun')):
            self.assertTrue(s.verify(p.proposal_id).accepted)

    def test_finite_adapter_receipt_replays_without_any_generator(self):
        from unittest.mock import patch
        from probe import run_probe, verify_report
        source = declared_source()
        report = run_probe(source, adapter=True)
        self.assertEqual(report['summary'], {'orders': 24, 'admissions': 72, 'rejections': 24, 'reconstructions': 96})
        for case in report['cases']:
            self.assertEqual(case['final_k'], (21, 20, 3, 1, 0, 2, 1, 8, 1))
        with patch.object(model.AdapterEvaluator, 'generate', side_effect=AssertionError('adapter rerun')), \
                patch.object(DuplicateEvaluator, 'generate', side_effect=AssertionError('duplicate rerun')):
            self.assertTrue(verify_report(source, report))

    def test_adapter_receipt_rejects_lost_order_and_relabelled_failure(self):
        from probe import run_probe, verify_report
        source = declared_source()
        report = run_probe(source, adapter=True)
        report['cases'].pop()
        with self.assertRaises(ValueError):
            verify_report(source, report)
        report = run_probe(source, adapter=True)
        report['cases'][0]['history'][10]['payload']['evaluator_id'] = 'AdapterEvaluator'
        with self.assertRaises(ValueError):
            verify_report(source, report)


if __name__ == '__main__':
    unittest.main()
