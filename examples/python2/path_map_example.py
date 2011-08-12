"""
    Path / Safety map 
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

class Project_example(RLFLExample.RLFLExample):
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
                    rlfl.set_flag(self.mapnum, p, rlfl.CELL_SEEN) 
                    rlfl.set_flag(self.mapnum, p, rlfl.CELL_OPEN)

if __name__ == '__main__':
    example = Project_example()
    example.create_map('pmap')
    
    # Define origin
    origin = example.origos[6]
    
    # Create a path map. example.origos[6] is a point on the map
    # that is the origin
    path_map_n = rlfl.path_fill_map(example.mapnum, origin, 0.7)
    
    # Bootstrap
    p = example.origos[4]
    
    # Create a path at most 90 steps long TOWARDS origin
    path = []
    for i in range(90):
        p = rlfl.path_step_map(example.mapnum, path_map_n, p)
        path.append(p)
        if p == origin:
            break
    example.print_map(path, example.origos[1], origin)
    
    # clear the path map 
    rlfl.path_clear_map(example.mapnum, path_map_n)
    
    # Create a safety amp map. example.origos[6] is a point on the map
    # that is the origin
    path_map_n = rlfl.path_fill_safety_map(example.mapnum, origin, 0.7)
    
    # Bootstrap
    p = example.origos[4]
    
    # Create a path at most 90 steps long TOWARDS origin
    path = []
    for i in range(90):
        p = rlfl.path_step_map(example.mapnum, path_map_n, p)
        path.append(p)
    example.print_map(path, example.origos[4], example.origos[6])
    
    # clear the path map 
    rlfl.path_clear_map(example.mapnum, path_map_n)
    
    
