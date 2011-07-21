import unittest

import sys
sys.path.append('..')

import rlfl
from emap import MAP as m
MAP, ORIGOS = m

class TestProject(unittest.TestCase):
    def setUp(self):
        self.map = rlfl.create_map(len(MAP), len(MAP[0]))
        for row in range(len(MAP)):
            for col in range(len(MAP[row])):
                if MAP[row][col] != '#':
                    rlfl.set_flag(self.map, row, col, rlfl.CELL_SEEN) 
                    rlfl.set_flag(self.map, row, col, rlfl.CELL_OPEN) 
                    
#    def test_beam(self): 
#        beam = rlfl.project_beam(self.map, ORIGOS[0], ORIGOS[3])
#        self.pr_map(beam)
#        
#    def test_ball(self): 
#        beam = rlfl.project_ball(self.map, ORIGOS[0], ORIGOS[3], 3)
#        self.pr_map(beam)
#        
#    def test_breath(self): 
#        beam = rlfl.project_breath(self.map, ORIGOS[0], ORIGOS[3], 3)
#        self.pr_map(beam)
#        beam = rlfl.project_breath(self.map, ORIGOS[0], ORIGOS[2], 3)
#        self.pr_map(beam)
        
    def pr_map(self, projection):
        print('\n->')
        for row in range(len(MAP)):
            for col in range(len(MAP[row])):
                if (row, col) in projection:
                    print('*', end="")
                else:
                    print(MAP[row][col], end="")
            print()
        print('<-')