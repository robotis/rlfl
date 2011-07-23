import unittest

import sys
sys.path.append('..')

import rlfl
from tmap import MAP as m
MAP, ORIGOS = m

class TestMap(unittest.TestCase):   
    def setUp(self):
        rlfl.delete_all_maps()
    
    def test_create(self):
        for m in range(rlfl.MAX_MAPS):
            self.assertEqual(m, rlfl.create_map(20, 20))
        try:
            m = rlfl.create_map(20, 20)
        except Exception as e:
            self.assertEqual(str(e), 'Too many maps')
            self.assertEqual(str(e.__class__), "<class 'rlfl.Error'>")
        else:
            self.fail('Expected Exception')
        rlfl.delete_all_maps()
        try:
            rlfl.create_map(-1, -1)
        except Exception as e:
            self.assertEqual(str(e), 'Invalid map size')
            self.assertEqual(str(e.__class__), "<class 'rlfl.Error'>")
        else:
            self.fail('Expected Exception')
        
    def test_delete(self):
        for m in range(rlfl.MAX_MAPS):
            rlfl.create_map(20, 20)
        rlfl.delete_all_maps()
        self.assertEqual(0, rlfl.create_map(20, 20))
        self.assertTrue(rlfl.delete_map(0))
        self.assertEqual(0, rlfl.create_map(20, 20))
        self.assertTrue(rlfl.delete_map(0))
        self.assertFalse(rlfl.delete_map(0))
        self.assertFalse(rlfl.delete_map(-1))
        
    def test_flags(self):
        m = rlfl.create_map(20, 20)
        test = (
            {
                'p': (10, 10),
                'm': -1,
                's': 'Map not initialized',
                'f': rlfl.CELL_OPEN
            },
            {
                'p': (-1, -1),
                'm': m,
                's': 'Location out of bounds',
                'f': rlfl.CELL_OPEN
            },
            {
                'p': (1, 1),
                'm': m,
                's': 'Invalid flag used',
                'f': -1
            },
        )
        for func in ['has_flag', 'set_flag', 'clear_flag']:
            for i in test:
                try:
                    getattr(rlfl, func)(i['m'], i['p'], i['f'])
                except Exception as e:
                    self.assertEqual(str(e), i['s'])
                    self.assertEqual(str(e.__class__), "<class 'rlfl.Error'>")
                else:
                    self.fail('Expected Exception: func: %s, %s' % (func, i['s']))
        for i in test[0:2]:
            try:
                f = rlfl.get_flags(i['m'], i['p'])
                self.assertEqual(f, 0)
            except Exception as e:
                self.assertEqual(str(e), i['s'])
                self.assertEqual(str(e.__class__), "<class 'rlfl.Error'>")
            else:
                self.fail('Expected Exception: func: %s, %s' % ('get_flags', i['s']))
        
    def test_map(self):
        m = rlfl.create_map(20, 20)
        rlfl.fill_map(m, rlfl.CELL_SEEN)
        for i in range(20):
            for j in range(20):
                self.assertTrue(rlfl.has_flag(m, (i, j), rlfl.CELL_SEEN))
        rlfl.clear_map(m)
        for i in range(20):
            for j in range(20):
                self.assertFalse(rlfl.has_flag(m, (i, j), rlfl.CELL_SEEN))
        test = (
            {
                'm': -1,
                's': 'Map not initialized',
                'f': rlfl.CELL_OPEN
            },
            {
                'm': 3,
                's': 'Map not initialized',
                'f': rlfl.CELL_OPEN
            },
            {
                'm': m,
                's': 'Invalid flag used',
                'f': -1
            },
        )
        for func in ['fill_map', 'clear_map']:
            for i in test:
                try:
                    getattr(rlfl, func)(i['m'], i['f'])
                except Exception as e:
                    self.assertEqual(str(e), i['s'])
                    self.assertEqual(str(e.__class__), "<class 'rlfl.Error'>")
                else:
                    self.fail('Expected Exception: func: %s, %s' % (func, i['s']))

if __name__ == '__main__':
    unittest.main()