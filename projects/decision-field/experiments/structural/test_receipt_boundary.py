"""Receipt ingress counterprobes; exercise real replay, CLI and file reads."""
from contextlib import contextmanager, redirect_stderr
import io
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

import probe
from model import MAX_BYTES, canonical

SOURCE_PATH = Path(__file__).with_name('source_graph.json')
RECEIPT_PATH = Path(__file__).resolve().parents[2] / 'evidence/structural_probe_2026_09_30/PROCESS_RECEIPT.json'


class ReceiptBoundaryTests(unittest.TestCase):
    def test_non_mapping_blobs_return_verification_rejection(self):
        # Removing container validation must fail this public API contract.
        source = SOURCE_PATH.read_bytes()
        report = json.loads(RECEIPT_PATH.read_bytes())
        for blobs in (None, [], 0, 'blobs'):
            with self.subTest(blobs=blobs):
                with self.assertRaises(Exception) as caught:
                    probe.verify_report(source, dict(report, blobs=blobs))
                self.assertIsInstance(caught.exception, ValueError)

    def test_cli_rejects_non_mapping_blobs_without_traceback(self):
        # A malformed external receipt must produce REJECTED, not an API crash.
        report = json.loads(RECEIPT_PATH.read_bytes())
        report['blobs'] = []
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / 'malformed.json'
            path.write_bytes(canonical(report))
            result = subprocess.run([sys.executable, str(Path(probe.__file__)),
                                     '--verify', str(path)], capture_output=True, text=True)
        self.assertEqual(result.returncode, 1)
        self.assertTrue(result.stderr.startswith('REJECTED:'), result.stderr)
        self.assertNotIn('Traceback', result.stderr)

    def test_cli_limits_consumed_bytes_before_rejecting_oversized_files(self):
        # An unbounded read before the size check consumes more than the budget.
        source, receipt = SOURCE_PATH.read_bytes(), RECEIPT_PATH.read_bytes()
        real_open = Path.open
        with tempfile.TemporaryDirectory() as temp:
            source_path, receipt_path = Path(temp) / 'source.json', Path(temp) / 'receipt.json'
            for oversized in ('source', 'receipt'):
                with self.subTest(oversized=oversized):
                    source_path.write_bytes(b'x' * (MAX_BYTES + 1024) if oversized == 'source' else source)
                    receipt_path.write_bytes(b'x' * (probe.MAX_REPORT_BYTES + 1024) if oversized == 'receipt' else receipt)
                    consumed = {source_path: 0, receipt_path: 0}

                    @contextmanager
                    def observed_open(path, *args, **kwargs):
                        # Instrument actual reads; neither contents nor I/O are replaced.
                        with real_open(path, *args, **kwargs) as stream:
                            class ObservedReader:
                                def read(self, size=-1):
                                    data = stream.read(size)
                                    consumed[path] += len(data)
                                    return data
                            yield ObservedReader()

                    error = io.StringIO()
                    argv = ['probe.py', '--source', str(source_path), '--verify', str(receipt_path)]
                    with patch.object(sys, 'argv', argv), patch.object(Path, 'open', observed_open), redirect_stderr(error):
                        with self.assertRaises(SystemExit) as caught:
                            probe.main()
                    self.assertEqual(caught.exception.code, 1)
                    self.assertTrue(error.getvalue().startswith('REJECTED:'), error.getvalue())
                    self.assertLessEqual(consumed[source_path], MAX_BYTES + 1)
                    self.assertLessEqual(consumed[receipt_path], probe.MAX_REPORT_BYTES + 1)

    def test_cli_rejects_excessive_json_nesting_without_traceback(self):
        # Byte-bounded input can still exceed the JSON parser's stack limit.
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / 'nested.json'
            path.write_bytes(b'[' * 10000 + b'0' + b']' * 10000)
            result = subprocess.run([sys.executable, str(Path(probe.__file__)),
                                     '--verify', str(path)], capture_output=True, text=True)
        self.assertEqual(result.returncode, 1)
        self.assertTrue(result.stderr.startswith('REJECTED:'), result.stderr)
        self.assertNotIn('Traceback', result.stderr)


if __name__ == '__main__':
    unittest.main()
