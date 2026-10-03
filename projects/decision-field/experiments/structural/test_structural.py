"""Recovered contract suite; each assertion exercises the real proposal protocol."""
from dataclasses import replace, FrozenInstanceError
import itertools
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

from model import DuplicateEvaluator, Session, canonical, digest, freeze, initial_projection, observe, reconstruct
from checker import check

SOURCE = Path(__file__).with_name('source_graph.json').read_bytes()
LEFT = DuplicateEvaluator(('occ:dup-1', 'occ:dup-2'), 'DuplicateEvaluator:left')
RIGHT = DuplicateEvaluator(('occ:dup-2', 'occ:dup-3'), 'DuplicateEvaluator:right')


def altered(raw, mutate):
    obj = json.loads(raw)
    mutate(obj)
    return canonical(obj)


class StructuralTests(unittest.TestCase):
    def test_share_payload_without_aliasing_occurrence_or_evidence(self):
        s = Session(SOURCE)
        before = s.active
        p = s.generate(LEFT)
        self.assertEqual(s.active, before)
        c = s.verify(p.proposal_id)
        self.assertTrue(c.accepted, c.reasons)
        self.assertEqual(s.active, before)
        self.assertEqual(reconstruct(p.candidate), SOURCE)
        self.assertTrue(s.admit(p.proposal_id, c.check_id))
        nodes = {n['occurrence_id']: n for n in json.loads(s.active)['occurrences']}
        self.assertEqual(len(nodes), 11)
        self.assertNotEqual(nodes['occ:dup-1']['evidence_ids'], nodes['occ:dup-2']['evidence_ids'])
        self.assertEqual(nodes['occ:dup-1']['body_id'], nodes['occ:dup-2']['body_id'])
        self.assertEqual(p.recovery.predecessor_bytes, before)
        self.assertEqual(p.recovery.source_bytes, SOURCE)
        self.assertEqual(observe(before).active_nodes - observe(s.active).active_nodes, 1)

    def test_smaller_alias_is_rejected_and_retained(self):
        s = Session(SOURCE)
        p = s.propose('forged:alias', altered(LEFT.generate(s.active), lambda a: a['occurrences'].pop(4)))
        self.assertLess(observe(p.candidate).active_nodes, observe(s.active).active_nodes)
        self.assertLess(len(p.candidate), len(s.active))
        before, prefix = s.active, s.history
        c = s.verify(p.proposal_id)
        self.assertIn('occurrence_distinctions', c.reasons)
        self.assertFalse(s.admit(p.proposal_id, c.check_id))
        self.assertEqual(s.active, before)
        self.assertEqual(s.history[:len(prefix)], prefix)
        self.assertEqual(s.proposal(p.proposal_id), p)
        self.assertEqual(s.history[-1].kind, 'ADMIT_REJECTED')

    def test_smaller_authority_removal_is_rejected_even_with_full_source_receipt(self):
        s = Session(SOURCE)
        p = s.propose('forged:authority', altered(LEFT.generate(s.active), lambda a: a['authorities'].pop()))
        self.assertLess(observe(p.candidate).active_nodes, observe(s.active).active_nodes)
        c = s.verify(p.proposal_id)
        self.assertIn('authority_boundaries', c.reasons)
        self.assertFalse(s.admit(p.proposal_id, c.check_id))
        self.assertEqual(reconstruct(s.active), SOURCE)

    def test_generate_and_forged_success_cannot_admit(self):
        s = Session(SOURCE)
        p = s.generate(LEFT)
        before = s.active
        self.assertFalse(s.admit(p.proposal_id, 'unrecorded-success'))
        self.assertEqual(s.active, before)
        c = s.verify(p.proposal_id)
        self.assertFalse(s.admit(p.proposal_id, s.history[0].event_id))
        self.assertTrue(s.admit(p.proposal_id, c.check_id))
        self.assertFalse(s.admit(p.proposal_id, c.check_id))

    def test_stale_verified_predecessor_cannot_replace_new_active(self):
        s = Session(SOURCE)
        left, right = s.generate(LEFT), s.generate(RIGHT)
        lc, rc = s.verify(left.proposal_id), s.verify(right.proposal_id)
        self.assertTrue(s.admit(left.proposal_id, lc.check_id))
        before = s.active
        self.assertFalse(s.admit(right.proposal_id, rc.check_id))
        self.assertEqual(s.active, before)
        self.assertEqual(s.history[-1].payload['reasons'], ['stale_predecessor'])

    def test_exact_receipt_and_certificate_tampering_rejected_independently(self):
        s = Session(SOURCE)
        p = s.generate(LEFT)
        for receipt in (replace(p.recovery, source_bytes=SOURCE + b' '),
                        replace(p.recovery, predecessor_bytes=p.candidate),
                        replace(p.recovery, source_sha256='0' * 64)):
            c = check(SOURCE, s.active, replace(p, recovery=receipt))
            self.assertFalse(c.accepted)
            self.assertIn('recovery_receipt', c.reasons)
        self.assertIn('certificate_binding', check(SOURCE, s.active, replace(p, certificate_ref='self-certified')).reasons)

    def test_checker_does_not_trust_prediction_or_evaluator_label(self):
        s = Session(SOURCE)
        p = s.generate(LEFT)
        self.assertIn('prediction_mismatch', check(SOURCE, s.active, replace(p, predicted_delta=(-99,) * 9)).reasons)
        self.assertIn('mutation_radius', check(SOURCE, s.active, replace(p, mutation_radius=0)).reasons)
        p = s.propose('DuplicateEvaluator', altered(p.candidate, lambda a: a['occurrences'][0].update(obligations=['changed:obligation'])))
        self.assertIn('occurrence_distinctions', s.verify(p.proposal_id).reasons)

    def test_equal_values_with_different_representation_or_authority_are_not_shared(self):
        s = Session(SOURCE)
        p = s.generate(DuplicateEvaluator(('occ:dup-1', 'occ:other-authority', 'occ:equal-value')))
        self.assertEqual(p.candidate, s.active)
        self.assertIn('no_strict_reduction', s.verify(p.proposal_id).reasons)
        def cross(a):
            nodes = {n['occurrence_id']: n for n in a['occurrences']}
            unused = nodes['occ:other-authority']['body_id']
            nodes['occ:other-authority']['body_id'] = nodes['occ:dup-1']['body_id']
            a['bodies'] = [b for b in a['bodies'] if b['body_id'] != unused]
        raw = altered(s.active, cross)
        self.assertEqual(reconstruct(raw), SOURCE)
        self.assertIn('shared_authority', s.verify(s.propose('forged:cross-authority', raw).proposal_id).reasons)

    def test_all_24_orders_reobserve_preserve_history_and_recover_each_step(self):
        from probe import attempt
        for order in itertools.permutations(('left', 'right', 'alias', 'authority')):
            s = Session(SOURCE)
            for action in order:
                before, prefix = s.active, s.history
                p = attempt(s, action)
                c = s.verify(p.proposal_id)
                self.assertEqual(s.admit(p.proposal_id, c.check_id), action in ('left', 'right'))
                self.assertEqual(p.recovery.predecessor_bytes, before)
                self.assertEqual(s.history[:len(prefix)], prefix)
                self.assertEqual(reconstruct(s.active), SOURCE)
            self.assertEqual(len(json.loads(s.active)['bodies']), 9)
            self.assertEqual(len(s.history), 13)

    def test_freeze_rejects_noncanonical_ambiguous_or_invalid_sources(self):
        bad = [SOURCE + b' ', SOURCE.replace(b'"graph_id":', b'"graph_id":"shadow","graph_id":', 1),
               altered(SOURCE, lambda g: g['nodes'].append(g['nodes'][0])),
               altered(SOURCE, lambda g: g['nodes'][0].update(authority_id='missing'))]
        for raw in bad:
            with self.assertRaises(ValueError):
                freeze(raw)

    def test_malformed_bounded_candidate_is_retained_but_rejected(self):
        s = Session(SOURCE)
        for raw in (b'{', b'[]', s.active + b' ', b'x' * 65537,
                    altered(s.active, lambda a: a['bodies'].append(a['bodies'][0]))):
            p = s.propose('forged:malformed', raw)
            self.assertFalse(s.verify(p.proposal_id).accepted)
            self.assertEqual(s.proposal(p.proposal_id).candidate, raw)

    def test_history_and_proposals_are_immutable_snapshots(self):
        s = Session(SOURCE)
        prefix = s.history
        p = s.generate(LEFT)
        self.assertEqual(len(prefix), 1)
        with self.assertRaises(FrozenInstanceError):
            p.candidate = b'forged'
        exported = s.history[0].payload
        exported['source_sha256'] = 'forged'
        self.assertEqual(s.history[0].payload['source_sha256'], digest(SOURCE))

    def test_invalid_coordinate_width_is_rejected_at_freeze(self):
        with self.assertRaises(ValueError):
            Session(altered(SOURCE, lambda g: g['nodes'][-1]['payload'].update(coordinates=8)))

    def test_retained_body_id_cannot_be_reassigned_after_exact_recovery(self):
        s = Session(SOURCE)
        def swap(a):
            first, second = a['bodies'][0], a['bodies'][1]
            first['body_id'], second['body_id'] = second['body_id'], first['body_id']
            for n in a['occurrences']:
                if n['body_id'] == first['body_id']:
                    n['body_id'] = second['body_id']
                elif n['body_id'] == second['body_id']:
                    n['body_id'] = first['body_id']
        raw = altered(LEFT.generate(s.active), swap)
        self.assertEqual(reconstruct(raw), SOURCE)
        self.assertFalse(s.verify(s.propose('forged:body-identity', raw).proposal_id).accepted)

    def test_mutated_edge_chronology_evidence_and_payload_cannot_admit(self):
        for mutate in (lambda a: a['edges'][0].update(relevant=False),
                       lambda a: a['occurrences'][3].update(chronology=50),
                       lambda a: a['occurrences'][3].update(evidence_ids=['changed']),
                       lambda a: a['bodies'][3]['payload'].update(value=8)):
            s = Session(SOURCE)
            p = s.propose('forged:distinction', altered(LEFT.generate(s.active), mutate))
            self.assertFalse(s.admit(p.proposal_id, s.verify(p.proposal_id).check_id))
            self.assertEqual(reconstruct(s.active), SOURCE)

    def test_larger_protected_vector_cannot_admit_even_if_source_recovery_is_exact(self):
        s = Session(SOURCE)
        p = s.generate(LEFT)
        self.assertTrue(s.admit(p.proposal_id, s.verify(p.proposal_id).check_id))
        p = s.propose('forged:expand', initial_projection(SOURCE))
        self.assertIn('protected_component_worsened', s.verify(p.proposal_id).reasons)

    def test_receipt_replays_in_separate_process_and_rejects_altered_failure(self):
        from probe import run_probe, verify_report
        report = run_probe(SOURCE)
        self.assertEqual(report['summary'], {'orders': 24, 'admissions': 48, 'rejections': 48, 'reconstructions': 96})
        self.assertTrue(verify_report(SOURCE, report))
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'receipt.json'
            path.write_bytes(canonical(report))
            r = subprocess.run([sys.executable, str(Path(__file__).with_name('probe.py')), '--verify', str(path)], capture_output=True, text=True)
            self.assertEqual(r.returncode, 0, r.stderr)
        report['cases'][0]['history'][-1]['payload']['reasons'] = []
        with self.assertRaises(ValueError):
            verify_report(SOURCE, report)

    def test_receipt_rejects_changed_blobs_and_missing_order_coverage(self):
        from probe import run_probe, verify_report
        report = run_probe(SOURCE)
        report['blobs'][next(iter(report['blobs']))] += '20'
        with self.assertRaises(ValueError):
            verify_report(SOURCE, report)
        report = run_probe(SOURCE)
        report['cases'].pop()
        with self.assertRaises(ValueError):
            verify_report(SOURCE, report)

    def test_numeric_equality_does_not_allow_retained_body_identity_swap(self):
        source = altered(SOURCE, lambda g: g['nodes'][7]['payload'].update(value=7.0))
        s = Session(source)
        def swap(a):
            nodes = {n['occurrence_id']: n for n in a['occurrences']}
            one, two = nodes['occ:dup-1']['body_id'], nodes['occ:equal-value']['body_id']
            bodies = {b['body_id']: b for b in a['bodies']}
            bodies[one]['payload'], bodies[two]['payload'] = bodies[two]['payload'], bodies[one]['payload']
            for n in a['occurrences']:
                if n['body_id'] == one:
                    n['body_id'] = two
                elif n['body_id'] == two:
                    n['body_id'] = one
        raw = altered(LEFT.generate(s.active), swap)
        self.assertEqual(reconstruct(raw), source)
        self.assertIn('body_identity', s.verify(s.propose('forged:numeric', raw).proposal_id).reasons)

    def test_long_stable_occurrence_ids_produce_recoverable_active_ids(self):
        obj = json.loads(SOURCE)
        old = obj['nodes'][-1]['occurrence_id']
        obj['nodes'][-1]['occurrence_id'] = 'x' * 128
        for e in obj['edges']:
            if e['to'] == old:
                e['to'] = 'x' * 128
        raw = canonical(obj)
        self.assertEqual(reconstruct(Session(raw).active), raw)

    def test_source_envelope_cannot_initialize_unusable_active_projection(self):
        obj = json.loads(SOURCE)
        obj['nodes'][0]['payload']['padding'] = ''
        obj['nodes'][0]['payload']['padding'] = 'x' * (65536 - len(canonical(obj)))
        raw = canonical(obj)
        self.assertEqual(len(raw), 65536)
        self.assertEqual(freeze(raw), raw)
        with self.assertRaises(ValueError):
            Session(raw)

    def _replaced_authority_report(self, mode):
        from probe import run_probe, attempt, _history
        report = run_probe(SOURCE)
        case, s = report['cases'][0], Session(SOURCE)
        for i, action in enumerate(case['order']):
            if action == 'authority':
                obj = json.loads(LEFT.generate(s.active))
                if mode == 'alias':
                    obj['occurrences'] = [n for n in obj['occurrences'] if n['occurrence_id'] != 'occ:dup-2']
                    for e in obj['edges']:
                        if e['to'] == 'occ:dup-2':
                            e['to'] = 'occ:dup-1'
                else:
                    obj['authorities'].pop()
                    for n in obj['occurrences']:
                        if n['authority_id'] == 'auth:checker':
                            n['authority_id'] = 'auth:owner'
                    obj['bodies'] = []
                p = s.propose('forged:authority', canonical(obj))
                case['steps'][i]['proposal_bytes'] = len(p.candidate)
            else:
                p = attempt(s, action)
            s.admit(p.proposal_id, s.verify(p.proposal_id).check_id)
        case['history'] = _history(s, report['blobs'])
        return report

    def test_receipt_rejects_consistent_mislabeled_authority_negative(self):
        from probe import verify_report
        with self.assertRaises(ValueError):
            verify_report(SOURCE, self._replaced_authority_report('alias'))

    def test_receipt_rejects_promoted_claim_ceiling(self):
        from probe import run_probe, verify_report
        report = run_probe(SOURCE)
        report['claim_ceiling'] = 'universal structural equivalence and P = NP'
        with self.assertRaises(ValueError):
            verify_report(SOURCE, report)

    def test_receipt_rejects_malformed_negative_substituted_for_discriminator(self):
        from probe import verify_report
        with self.assertRaises(ValueError):
            verify_report(SOURCE, self._replaced_authority_report('malformed'))


if __name__ == '__main__':
    unittest.main()
