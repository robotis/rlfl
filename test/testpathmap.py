import unittest

import sys
sys.path.append('..')

import rlfl
from maps.tmap import MAP as m
TMAP, TORIGOS = m

class TestPathMap(unittest.TestCase):  
    def setUp(self):
        rlfl.delete_all_maps()
        self.map = rlfl.create_map(len(TMAP), len(TMAP[0]))
        for row in range(len(TMAP)):
            for col in range(len(TMAP[row])):
                if TMAP[row][col] != '#':
                    p = (row, col)
                    rlfl.set_flag(self.map, p, rlfl.CELL_SEEN) 
                    rlfl.set_flag(self.map, p, rlfl.CELL_OPEN) 
                    
    def test_create(self):
        return
        for i in range(0, rlfl.MAX_PATHS):
            self.assertEqual(i, rlfl.path_fill_map(self.map, TORIGOS[1]))
        try:
            rlfl.path_fill_map(self.map, TORIGOS[1])
        except Exception as e:
            self.assertEqual(str(e), 'Unable to create pathmap: Too many maps')
            self.assertEqual(str(e.__class__), "<class 'rlfl.Error'>")
        else:
            self.fail('Expected Exception: Unable to create pathmap: Too many maps')
        rlfl.path_clear_all_maps(self.map)
        self.assertEqual(0, rlfl.path_fill_map(self.map, TORIGOS[1]))
        
    def test_step(self):
        pass
        
    def test_step_input(self):
        return
        pm = rlfl.path_fill_map(self.map, TORIGOS[1])
        test = (
            {
                'm': -1,
                'x': pm,
                'p': TORIGOS[1],
                's': 'Map not initialized'
            },
            {
                'm': self.map,
                'x': pm,
                'p': (-1, -1),
                's': 'Location out of bounds'
            },
            {
                'm': self.map,
                'x': -1,
                'p': TORIGOS[1],
                's': 'Uninitialized pathmap used'
            },
        )
        for i in test:
            try:
                pmap = rlfl.path_step_map(i['m'], i['x'], i['p'])
            except Exception as e:
                self.assertEqual(str(e), i['s'])
                self.assertEqual(str(e.__class__), "<class 'rlfl.Error'>")
            else:
                self.fail('Expected Exception: %s' % i['s'])
        
    def test_input(self):
        test = (
            {
                'm': -1,
                'p': TORIGOS[1],
                's': 'Map not initialized'
            },
            {
                'm': self.map,
                'p': (-1, -1),
                's': 'Location out of bounds'
            },
        )
        for i in test:
            try:
                pmap = rlfl.path_fill_map(i['m'], i['p'])
            except Exception as e:
                self.assertEqual(str(e), i['s'])
                self.assertEqual(str(e.__class__), "<class 'rlfl.Error'>")
            else:
                self.fail('Expected Exception: %s' % i['s'])
        try:
            pmap = rlfl.path_fill_autoexplore_map(-1)
        except Exception as e:
            self.assertEqual(str(e), 'Map not initialized')
            self.assertEqual(str(e.__class__), "<class 'rlfl.Error'>")
        else:
            self.fail('Expected Exception: %s' % 'Map not initialized')
        try:
            pmap = rlfl.path_fill_custom_map(-1)
        except Exception as e:
            self.assertEqual(str(e), 'Map not initialized')
            self.assertEqual(str(e.__class__), "<class 'rlfl.Error'>")
        else:
            self.fail('Expected Exception: %s' % 'Map not initialized')
        try:
            rlfl.path_clear_map(-1, 0)
        except Exception as e:
            self.assertEqual(str(e), 'Map not initialized')
            self.assertEqual(str(e.__class__), "<class 'rlfl.Error'>")
        else:
            self.fail('Expected Exception: Map not initialized')
        
if __name__ == '__main__':
    unittest.main()