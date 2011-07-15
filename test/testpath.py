import unittest

import sys
sys.path.append('..')

import rlfl
from tmap import MAP as m
MAP, ORIGOS = m

class TestPath(unittest.TestCase):  
    def setUp(self):
        self.map = rlfl.create_map(len(MAP), len(MAP[0]))
        for row in range(len(MAP)):
            for col in range(len(MAP[row])):
                if MAP[row][col] != '#':
                    rlfl.set_flag(self.map, row, col, rlfl.CELL_SEEN) 
                    rlfl.set_flag(self.map, row, col, rlfl.CELL_OPEN) 
    
    def test_create(self):
        p = ORIGOS[0]
        p1 = ORIGOS[1]
        p2 = ORIGOS[2]
        p3 = ORIGOS[3]
        self.assertEqual(False, rlfl.path(self.map, (0,0), (1000,1000)), ORIGOS[2])
        self.assertEqual(False, rlfl.path(self.map, p, p1), ORIGOS[1])
        self.assertEqual(False, rlfl.path(self.map, p2, p3), ORIGOS[3])
        self.assertEqual(False, rlfl.path(self.map, p2, p), ORIGOS[0])
        r = ((18, 33), (17, 33), (16, 33), (15, 33), (14, 33), (13, 33), (12, 32), (11, 32), (10, 32), (9, 32))
        self.assertEqual(r, rlfl.path(self.map, p, p3), ORIGOS[3])
        r = ((15, 11), (15, 10), (15, 9), (15, 8), (15, 7), (15, 6))
        self.assertEqual(r, rlfl.path(self.map, p1, p2), ORIGOS[2])
    
    def print_path(self, path, t):
        if not path:
            print("NO PATH")
            return
        for row in range(len(MAP)):
            for col in range(len(MAP[row])):
                if (row, col) == t:
                    print('T', end="")
                elif (row, col) in path:
                    print('*', end="")
#                elif (row, col) == (9, 32):
#                    print('X', end="")
                else:
                    print(MAP[row][col], end="")
            print() 
        print()

if __name__ == '__main__':
    unittest.main()