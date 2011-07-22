import unittest

import sys
sys.path.append('..')
import rlfl

class TestDistance(unittest.TestCase):   
    def test_distance(self):
        p, p1 = ((1, 1), (1, 1))
        self.assertEqual(rlfl.distance(p, p1), 0)
        p, p1 = ((1, 1), (2, 2))
        self.assertEqual(rlfl.distance(p, p1), 1)
        p, p1 = ((1, 1), (11, 11))
        self.assertEqual(rlfl.distance(p, p1), 15)
        p, p1 = ((1, 1), (0, 0))
        self.assertEqual(rlfl.distance(p, p1), 1)

if __name__ == '__main__':
    unittest.main()