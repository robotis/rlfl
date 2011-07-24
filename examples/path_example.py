"""
    Example path generation
    
    rlfl.path(
        map,
        origin,
        destination,
        algorithm,
        range,
        flags,
        diagonal cost
    )
    
    Algorithms:
    rlfl.PATH_BASIC
    rlfl.PATH_ASTAR
"""
try:
    import rlfl
except ImportError:
    import sys
    sys.path.append('..')
    import rlfl
import RLFLExample

class Path_example(RLFLExample.RLFLExample):     
    def show_path(self, p1, p2, type, m, r=30, flags=0, diagonal=1.0):
        # Import the map
        imap = __import__(m)
        self.map, origos = imap.MAP
        
        # Create the RLFL internal map
        mapnum = rlfl.create_map(len(self.map), len(self.map[0]))
        for row in range(len(self.map)):
            for col in range(len(self.map[row])):
                if self.map[row][col] != '#':
                    p = (row, col)
                    # Set non-wall grids as open and seen
                    rlfl.set_flag(mapnum, p, rlfl.CELL_SEEN) 
                    rlfl.set_flag(mapnum, p, rlfl.CELL_OPEN)
                    
        # Fetch points to path
        p1, p2 = (origos[p1], origos[p2])    
        
        # Create the path
        path = rlfl.path(mapnum, p1, p2, type, r, flags, diagonal)
        
        # Print the path
        self.print_map(path, p1, p2) 
        
if __name__ == '__main__':
    example = Path_example()
    
    # Basic path
    example.show_path(1, 4, rlfl.PATH_BASIC, 'tmap')
    
    # Using A*
    example.show_path(1, 4, rlfl.PATH_ASTAR, 'tmap')
    
    # Basic path
    example.show_path(1, 5, rlfl.PATH_BASIC, 'emap')
    
    # Using A*
    example.show_path(1, 5, rlfl.PATH_ASTAR, 'emap')
    
    # Using A*, Make diagonals the cheapest move
    example.show_path(1, 5, rlfl.PATH_ASTAR, 'emap', 30, 0, 0.0)
    
    # Basic path
    example.show_path(1, 4, rlfl.PATH_BASIC, 'emap')
    
    # Using A*
    example.show_path(1, 4, rlfl.PATH_ASTAR, 'emap', -1, 0, 0.0)
    
    
    