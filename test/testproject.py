import unittest

import sys
sys.path.append('..')

import rlfl
from emap import MAP as m
MAP, ORIGOS = m

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
                    
    def test_beam(self): 
        beam = rlfl.project_beam(self.map, ORIGOS[1], ORIGOS[4])
#        self.pr_map(beam, ORIGOS[1], ORIGOS[4])
        
    def test_ball(self): 
        beam = rlfl.project_ball(self.map, ORIGOS[1], ORIGOS[4], 2)
#        self.pr_map(beam, ORIGOS[1], ORIGOS[4])
        
    def test_cone(self): 
        beam = rlfl.project_cone(self.map, ORIGOS[1], ORIGOS[4], 2)
#        self.pr_map(beam, ORIGOS[1], ORIGOS[4])
        beam = rlfl.project_cone(self.map, ORIGOS[1], ORIGOS[3], 2, 10)
#        self.pr_map(beam, ORIGOS[1], ORIGOS[3])
        beam = rlfl.project_cone(self.map, ORIGOS[1], ORIGOS[5], 4)
#        self.pr_map(beam, ORIGOS[1], ORIGOS[5])
        beam = rlfl.project_cone(self.map, ORIGOS[1], ORIGOS[5], 2)
#        self.pr_map(beam, ORIGOS[1], ORIGOS[5])
        beam = rlfl.project_cone(self.map, ORIGOS[3], ORIGOS[6], 2)
#        self.pr_map(beam, ORIGOS[3], ORIGOS[6])
        beam = rlfl.project_cone(self.map, ORIGOS[1], ORIGOS[8], 1)
#        self.pr_map(beam, ORIGOS[1], ORIGOS[8])
        beam = rlfl.project_cone(self.map, ORIGOS[1], ORIGOS[5], 2, 4)
#        self.pr_map(beam, ORIGOS[1], ORIGOS[5])
        
    def pr_map(self, projection, S, T):
        print('\n->')
        for row in range(len(MAP)):
            for col in range(len(MAP[row])):
                if (row, col) == S:
                    print('S', end="")
                elif (row, col) == T:
                    print('T', end="")
                elif (row, col) in projection:
                    print('*', end="")
                else:
                    print(MAP[row][col], end="")
            print()
        print('<-')
        
    