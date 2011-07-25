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
import sys
if sys.version_info[0] < 3:
    sys.exit("This example works with python3 only...")
try:
    import rlfl
except ImportError:
    sys.path.append('../..')
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
        
    def show_path_away(self, p1, p2, type, m, r=30, flags=0, diagonal=1.0):
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
        path = rlfl.path_away(mapnum, p1, p2, type, r, flags, diagonal)
        
        # Print the path
        self.print_map(path, p1, p2) 
        
if __name__ == '__main__':
    example = Path_example()
    
    # Basic path
    example.show_path(1, 4, rlfl.PATH_BASIC, 'tmap')
    
    # Using A*
    example.show_path(1, 4, rlfl.PATH_ASTAR, 'tmap')
    
    # Basic path
    example.show_path(1, 5, rlfl.PATH_BASIC, 'pmap')
    
    # Using A*
    example.show_path(1, 5, rlfl.PATH_ASTAR, 'pmap')
    
    # Using A*, Make diagonals the cheapest move
    example.show_path(1, 5, rlfl.PATH_ASTAR, 'pmap', 30, 0, 0.0)
    
    # Basic path
    example.show_path(4, 6, rlfl.PATH_BASIC, 'pmap')
    
    # Using A*
    example.show_path(4, 6, rlfl.PATH_ASTAR, 'pmap', -1, 0, 10.0)
    
    # Using A*
    example.show_path_away(4, 6, rlfl.PATH_ASTAR, 'pmap', -1, 0, 10.0)
    
    
    