import unittest

import sys
sys.path.append('..')

import rlfl
from maps.tmap import MAP as m
MAP, ORIGOS = m

class TestLos(unittest.TestCase):
    def setUp(self):
        rlfl.delete_all_maps()
        self.map = rlfl.create_map(len(MAP), len(MAP[0]))
        for row in range(len(MAP)):
            for col in range(len(MAP[row])):
                if MAP[row][col] != '#':
                    p = (row, col)
                    rlfl.set_flag(self.map, p, rlfl.CELL_SEEN) 
                    rlfl.set_flag(self.map, p, rlfl.CELL_OPEN) 
    
    def test_los(self):
        p, p1, p2, p3, p4, p5, p6, p7, p8, p9 = ORIGOS
        self.assertFalse(rlfl.los(self.map, p, p1))
        self.assertFalse(rlfl.los(self.map, p, p2))
        self.assertFalse(rlfl.los(self.map, p3, p))
        self.assertTrue(rlfl.los(self.map, p2, p3))
        self.assertTrue(rlfl.los(self.map, p3, p2))
        
    def test_input(self):
        test = (
            (-1, ORIGOS[1], ORIGOS[2], 'Map not initialized'),
            (self.map, (-1, -1), ORIGOS[2], 'Location out of bounds'),
            (self.map, (-1, -1), (-1, -1), 'Location out of bounds'),
            (self.map, ORIGOS[1], (-1, -1), 'Location out of bounds'),
        )
        for i in test:
            try:
                rlfl.los(i[0], i[1], i[2])
            except Exception as e:
                self.assertEqual(str(e), i[3])
                self.assertEqual(str(e.__class__), "<class 'rlfl.Error'>")
            else:
                self.fail('Expected Exception: %s' % (i[3]))

if __name__ == '__main__':
    unittest.main()