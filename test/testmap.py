import unittest

import sys
sys.path.append('..')

import rlfl
from tmap import MAP as m
MAP, ORIGOS = m

class TestMap(unittest.TestCase):   
    def test_create(self):
        rlfl.delete_all_maps()
        for m in range(rlfl.MAX_MAPS):
            self.assertEqual(m, rlfl.create_map(20, 20))
        self.assertFalse(rlfl.create_map(20, 20))
        
    def test_delete(self):
        rlfl.delete_all_maps()
        for m in range(rlfl.MAX_MAPS):
            rlfl.create_map(20, 20)
        self.assertFalse(rlfl.create_map(20, 20))
        rlfl.delete_all_maps()
        self.assertEqual(0, rlfl.create_map(20, 20))
        self.assertTrue(rlfl.delete_map(0))
        self.assertEqual(0, rlfl.create_map(20, 20))
        
    def test_flags(self):
        rlfl.delete_all_maps()
        rlfl.create_map(20, 20)
        self.assertFalse(rlfl.has_flag(0, 10, 10, rlfl.CELL_OPEN))
        rlfl.set_flag(0, 10, 10, rlfl.CELL_OPEN)
        self.assertTrue(rlfl.has_flag(0, 10, 10, rlfl.CELL_OPEN))
        self.assertEqual(rlfl.CELL_OPEN, rlfl.get_flags(0, 10, 10))
        rlfl.set_flag(0, 10, 10, rlfl.CELL_SEEN)
        self.assertEqual(rlfl.CELL_OPEN | rlfl.CELL_SEEN, rlfl.get_flags(0, 10, 10))
        rlfl.clear_flag(0, 10, 10, rlfl.CELL_OPEN)
        self.assertFalse(rlfl.has_flag(0, 10, 10, rlfl.CELL_OPEN))
        self.assertTrue(rlfl.has_flag(0, 10, 10, rlfl.CELL_SEEN))
        rlfl.clear_flag(0, 10, 10, rlfl.CELL_SEEN)
        rlfl.set_flag(0, 10, 10, rlfl.CELL_MASK)
        self.assertTrue(rlfl.has_flag(0, 10, 10, rlfl.CELL_OPEN))
        
    def test_map(self):
        rlfl.delete_all_maps()
        rlfl.create_map(20, 20)
        rlfl.fill_map(0, rlfl.CELL_SEEN)
        for i in range(20):
            for j in range(20):
                self.assertTrue(rlfl.has_flag(0, i, j, rlfl.CELL_SEEN))
#        rlfl.clear_map(6)
        rlfl.clear_map(0)
        for i in range(20):
            for j in range(20):
                self.assertFalse(rlfl.has_flag(0, i, j, rlfl.CELL_SEEN))

if __name__ == '__main__':
    unittest.main()