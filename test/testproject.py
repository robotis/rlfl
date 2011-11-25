import unittest

import sys
sys.path.append('..')

import rlfl
from maps.emap import MAP as m
MAP, ORIGOS = m
from tdebug import print_map

class TestProject(unittest.TestCase):
    def setUp(self):
        rlfl.delete_all_maps()
        self.map = rlfl.create_map(len(MAP), len(MAP[0]))
        for row in range(len(MAP)):
            for col in range(len(MAP[row])):
                if MAP[row][col] != '#':
                    p = (row, col)
                    rlfl.set_flag(self.map, p, rlfl.CELL_SEEN) 
                    rlfl.set_flag(self.map, p, rlfl.CELL_OPEN) 
                    
    def test_input(self):
        test = (
            {
                'p': (10, 10),
                'p1': (10, 10),
                'm': -1,
                's': 'Map not initialized',
                'r': 2,
                'l': 10,
            },
            {
                'p': (-1, -1),
                'p1': (10, 10),
                'm': self.map,
                's': 'Projection failed: origin invalid',
                'r': 2,
                'l': 10,
            },
            {
                'p1': (-1, -1),
                'p': (10, 10),
                'm': self.map,
                's': 'Projection failed: destination invalid',
                'r': 2,
                'l': 10,
            },
            {
                'p1': (-1, -1),
                'p': (-1, -1),
                'm': self.map,
                's': 'Projection failed: origin invalid',
                'r': 2,
                'l': 10,
            },
        )
        for i in test:
            try:
                rlfl.project_beam(i['m'], i['p'], i['p1'], i['l'])
            except Exception as e:
                self.assertEqual(str(e), i['s'])
                self.assertEqual(str(e.__class__), "<class 'rlfl.Error'>")
            else:
                self.fail('Expected Exception: %s in project_beam' % (i['s']))
        for f in ['project_ball', 'project_cone']:
            for i in test:
                try:
                    getattr(rlfl, f)(i['m'], i['p'], i['p1'], i['r'], i['l'])
                except Exception as e:
                    self.assertEqual(str(e), i['s'])
                    self.assertEqual(str(e.__class__), "<class 'rlfl.Error'>")
                else:
                    self.fail('Expected Exception: %s in %s' % (i['s'], f))
                    
    def test_beam(self): 
        beam = rlfl.project_beam(self.map, ORIGOS[1], ORIGOS[4])
        self.assertEqual(17, len(beam))
        beam = rlfl.project_beam(self.map, ORIGOS[1], ORIGOS[1])
        self.assertEqual(1, len(beam))
        beam = rlfl.project_beam(self.map, ORIGOS[1], ORIGOS[4], -5)
        self.assertEqual(17, len(beam))
        
    def test_ball(self): 
        ball = rlfl.project_ball(self.map, ORIGOS[1], ORIGOS[4], 2)
        self.assertEqual(32, len(ball))
        ball = rlfl.project_ball(self.map, ORIGOS[1], ORIGOS[4], -2)
        self.assertEqual(17, len(ball))
        
    def test_cone(self): 
        beam = rlfl.project_cone(self.map, ORIGOS[1], ORIGOS[4], 2)
        self.assertEqual(39, len(beam))
        beam = rlfl.project_cone(self.map, ORIGOS[1], ORIGOS[3], 2, 10)
        self.assertEqual(29, len(beam))
        beam = rlfl.project_cone(self.map, ORIGOS[1], ORIGOS[5], 4)
        self.assertEqual(75, len(beam))
        beam = rlfl.project_cone(self.map, ORIGOS[1], ORIGOS[5], 2)
        self.assertEqual(31, len(beam))
        beam = rlfl.project_cone(self.map, ORIGOS[3], ORIGOS[6], 2)
        self.assertEqual(50, len(beam))
        beam = rlfl.project_cone(self.map, ORIGOS[1], ORIGOS[8], 1)
        self.assertEqual(9, len(beam))
        beam = rlfl.project_cone(self.map, ORIGOS[1], ORIGOS[5], 2, 4)
        self.assertEqual(19, len(beam))
        
        
    