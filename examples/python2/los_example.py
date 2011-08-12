"""
    Example RLFL Line Of Sight
    
    LOS method:
    rlfl.los(
        map,
        point 1,
        point 2,
    )
    return True or False
    
    Cells need to be marked rlfl.CELL_OPEN and rlfl.CELL_SEEN 
    to not block LOS
"""
import sys
if sys.version_info[0] > 2:
    sys.exit("This example works with python2 only...")
try:
    import rlfl
except ImportError:
    sys.path.append('../..')
    import rlfl
import RLFLExample

class LOS_example(RLFLExample.RLFLExample):
    def create_map(self, m):
        # Import the map
        imap = __import__(m)
        self.map, self.origos = imap.MAP
        
        # Create the RLFL internal map
        self.mapnum = rlfl.create_map(len(self.map), len(self.map[0]))
        for row in range(len(self.map)):
            for col in range(len(self.map[row])):
                if self.map[row][col] != '#':
                    p = (row, col)
                    # Set non-wall grids as open and seen
                    rlfl.set_flag(self.mapnum, p, rlfl.CELL_OPEN)
                    rlfl.set_flag(self.mapnum, p, rlfl.CELL_SEEN)
                    
    def show_los(self):
        # LOS between 1 and 4 on the map in tmap.py
        have_los = rlfl.los(self.map_number, self.origos[1], self.origos[4])
        assert(have_los == False)
        # LOS between 2 and 3
        have_los = rlfl.los(self.map_number, self.origos[2], self.origos[3])
        assert(have_los == True)

if __name__ == '__main__':
    example = LOS_example()
    example.create_map('tmap')