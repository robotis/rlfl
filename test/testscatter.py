import unittest

import sys
sys.path.append('..')
import rlfl
from tmap import MAP as m
MAP, ORIGOS = m

class TestDistance(unittest.TestCase):   
    def setUp(self):
        rlfl.delete_all_maps()
        self.map = rlfl.create_map(len(MAP), len(MAP[0]))
        for row in range(len(MAP)):
            for col in range(len(MAP[row])):
                if MAP[row][col] != '#':
                    p = (row, col)
                    rlfl.set_flag(self.map, p, rlfl.CELL_SEEN) 
                    rlfl.set_flag(self.map, p, rlfl.CELL_OPEN) 
    
    def test_scatter(self):
        p = ORIGOS[0]
        x, y = p
        for i in range(10):
            px, py = rlfl.scatter(self.map, p, 10, rlfl.CELL_OPEN)
            self.assertTrue(px < (x + 10))
            self.assertTrue(px >= (x - 10))
            self.assertTrue(py < (y + 10))
            self.assertTrue(py >= (y - 10))
            self.assertTrue(rlfl.has_flag(self.map, (px, py), rlfl.CELL_OPEN))
        
if __name__ == '__main__':
    unittest.main()