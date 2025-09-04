import unittest
import sys
import os

# Ensure parent dir is in sys.path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))

from crc32 import Crc32

class TestCRC32(unittest.TestCase):
    def setUp(self):
        self.crc = Crc32()

    def test_version(self):
        version = self.crc.version()
        self.assertIsInstance(version, int)
        version_str = f"{version >> 16}.{(version >> 8) & 0xff}.{version & 0xff}"
        self.assertEqual(self.crc.version_str(), version_str)

    def test_initialize_finalize(self):
        val = self.crc.initialize()
        self.assertIsInstance(val, int)
        final = self.crc.finalize(val)
        self.assertEqual(final, 0)

    def test_calc_known(self):
        # CRC32 of b"123456789" should be 0xCBF43926
        self.assertEqual(self.crc.calc(b"123456789"), 0xCBF43926)

    def test_block_vs_calc(self):
        data1 = b"Lorem ipsum dolor sit amet consectetur adipiscing elit. " \
                b"Quisque faucibus ex sapien vitae pellentesque sem placerat. " \
                b"In id cursus mi pretium tellus duis convallis. " \
                b"Tempus leo eu aenean sed diam urna tempor. "
        data2 = b"Pulvinar vivamus fringilla lacus nec metus bibendum egestas. " \
                b"Iaculis massa nisl malesuada lacinia integer nunc posuere. " \
                b"Ut hendrerit semper vel class aptent taciti sociosqu. " \
                b"Ad litora torquent per conubia nostra inceptos himenaeos."
        step = self.crc.update(data1)
        step = self.crc.update(data2, step)
        step = self.crc.finalize(step)        
        direct = self.crc.calc(data1 + data2)
        self.assertEqual(direct, 0xBF4A2172)
        self.assertEqual(direct, step)

if __name__ == "__main__":
    unittest.main()
