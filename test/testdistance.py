import unittest

import sys
sys.path.append('..')
import rlfl

class TestDistance(unittest.TestCase):   
    def test_distance(self):
        x, y, x1, y1 = (1, 1, 1, 1)
        self.assertEqual(rlfl.distance(x, y, x1, y1), 0)
        x, y, x1, y1 = (1, 1, 2, 2)
        self.assertEqual(rlfl.distance(x, y, x1, y1), 1)
        x, y, x1, y1 = (1, 1, 11, 11)
        self.assertEqual(rlfl.distance(x, y, x1, y1), 15)
        x, y, x1, y1 = (1, 1, 0, 0)
        self.assertEqual(rlfl.distance(x, y, x1, y1), 1)

if __name__ == '__main__':
    unittest.main()