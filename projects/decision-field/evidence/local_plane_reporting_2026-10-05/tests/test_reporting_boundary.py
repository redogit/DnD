"""Reporting tests, not tests of the unrecovered three-state model."""
import json
import os
from pathlib import Path
import subprocess
import unittest

ROOT = Path(__file__).resolve().parents[1]

class ReportingBoundaryTests(unittest.TestCase):
    def test_stdout_does_not_claim_unobserved_control_execution(self):
        output = subprocess.check_output([os.environ['LOCAL_PLANE_TEST_BINARY']], text=True)
        self.assertNotIn('four-stage stair', output)
        self.assertIn('stage descriptors', output)

    def test_stdout_does_not_claim_a_complexity_measurement(self):
        output = subprocess.check_output([os.environ['LOCAL_PLANE_TEST_BINARY']], text=True)
        self.assertNotIn('average-O(1)', output)
        self.assertIn('route reuse', output)

    def test_same_descriptors_do_not_identify_same_outcome(self):
        rows = json.loads((ROOT/'evidence/native_probe.json').read_text())
        by = {r['case']: r for r in rows}
        self.assertEqual(by['integrate']['stages'], by['rejected_execution']['stages'])
        self.assertTrue(by['integrate']['integrated'])
        self.assertFalse(by['rejected_execution']['integrated'])

    def test_revision_is_not_complete_state_identity(self):
        rows = json.loads((ROOT/'evidence/native_probe.json').read_text())
        by = {r['case']: r for r in rows}
        self.assertEqual(by['reuse']['revision_before'], by['reuse']['revision_after'])
        self.assertNotEqual(by['integrate']['occurrence'], by['reuse']['occurrence'])
        self.assertEqual(by['conflict']['revision_before'], by['conflict']['revision_after'])
        self.assertEqual(by['conflict']['residuals_after'], by['conflict']['residuals_before'] + 1)

    def test_rejected_routes_cannot_overwrite_admitted_route(self):
        rows = json.loads((ROOT/'evidence/native_probe.json').read_text())
        for row in rows:
            if row['case'] in ('conflict','missing_obligation','rejected_execution'):
                self.assertTrue(row['route_A_preserved'])
                self.assertFalse(row['integrated'])
                self.assertEqual(row['occurrence'], row['residual_occurrence'])

    def test_validation_failure_limitation_is_exposed_not_erased(self):
        rows = json.loads((ROOT/'evidence/native_probe.json').read_text())
        by = {r['case']: r for r in rows}
        bad = by['invalid_obligation_id']
        self.assertIn('must not be empty', bad['exception'])
        self.assertEqual(bad['residuals_before'], bad['residuals_after'])
        self.assertEqual(by['after_invalid']['occurrence'], by['new_context']['occurrence'] + 2)
        self.assertEqual(bad['failure_preserved_by'], 'outer-diagnostic-only')

if __name__ == '__main__':
    unittest.main(verbosity=2)
