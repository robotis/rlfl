import unittest

import sys
sys.path.append('..')

import rlfl
from maps.rmap import MAP as m
MAP, ORIGOS = m
from tdebug import print_map

class TestProject(unittest.TestCase):
    def setUp(self):
        rlfl.delete_all_maps()
        self.map = rlfl.create_map(len(MAP), len(MAP[0]))
        for row in range(len(MAP)):
            for col in range(len(MAP[row])):
                if MAP[row][col] == 'R':
                    p = (row, col)
                    rlfl.set_flag(self.map, p, rlfl.CELL_REFL) 
                elif MAP[row][col] == '.':
                    p = (row, col)
                    rlfl.set_flag(self.map, p, rlfl.CELL_SEEN) 
                    rlfl.set_flag(self.map, p, rlfl.CELL_OPEN)  
                    
    def test_beam(self): 
        test = [0, 0, 29, 7, 10, 29, 6, 10, 0]
        for i in range(2, 8):
            beam = rlfl.project_beam(self.map, ORIGOS[1], ORIGOS[i], rlfl.MAX_RANGE, rlfl.PROJECT_REFL)
            self.assertEqual(test[i], len(beam), i)
#            print_map(MAP, beam, ORIGOS[1], ORIGOS[i])
        # Diagonals create random fun    
        beam = rlfl.project_beam(self.map, ORIGOS[1], ORIGOS[8], rlfl.MAX_RANGE, rlfl.PROJECT_REFL)
        self.assertIn(len(beam), [8, 15])
#        print_map(MAP, beam, ORIGOS[1], ORIGOS[i])   
        
if __name__ == '__main__':
    unittest.main()                 
