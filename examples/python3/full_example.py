"""
    RLFL roguelike functions
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

class Full_example(RLFLExample.RLFLExample):
    def print_map(self, p):
        fov = []
        for row in range(len(self.map)):
            for col in range(len(self.map[row])):
                if rlfl.has_flag(self.map_number, (row, col), rlfl.CELL_SEEN):
                    fov.append((row, col))
        super().print_map(fov, p, p)
    """
    '############################################################',
    '############################################################',
    '###########..........#######################################',
    '###########>...0.....+...........###########################',
    '###########..........#######################################',
    '################################.###########################',
    '################################.###########################',
    '################################.###########################',
    '##################<#############.###.#...###################',
    '################.#..............1......#.###################',
    '################.###############.#######.###################',
    '################.###############.#######>###################',
    '######.#.#.#.#.#.###############.###########################',
    '######...........##############...##########################',
    '#######.#.#.#.#.##############.....#########################',
    '######2....3.....############...............4.<#############',
    '#############################.......########################',
    '##############################.....#########################',
    '###############################...##########################',
    '############################################################',
    """
    def show(self):
        # Start by creating a map on which to work
        # We shall import a dummy map to use
        
        # Import the map
        import tmap
        self.map, self.origos = tmap.MAP
        p0, p1, p2, p3, p4, p5, p6, p7, p8, p9 = self.origos
        
        # Create the RLFL internal map
        width = len(self.map)
        height = len(self.map[0])
        self.map_number = rlfl.create_map(width, height)
        
        # We now have a map number representing the 
        # internal map in rlfl
        
        # initialize the map
        for row in range(len(self.map)):
            for col in range(len(self.map[row])):
                if self.map[row][col] != '#':
                    p = (row, col)
                    
                    # Set non-wall grids as open and seen
                    rlfl.set_flag(self.map_number, p, rlfl.CELL_SEEN) 
                    rlfl.set_flag(self.map_number, p, rlfl.CELL_OPEN)
                    
        # we now have a map to work on
        # LOS between 1 and 4 on the map above
        have_los = rlfl.los(self.map_number, p1, p2)
        assert(have_los == False)
        # LOS between 2 and 3
        have_los = rlfl.los(self.map_number, p2, p3)
        assert(have_los == True)
        
        # Measure distance
        dist = rlfl.distance(p1, p4)
        
        # Plot simple paths
        flags = 0
        # range (-1 for max range)
        r = -1 
        path = rlfl.path(self.map_number, p1, p2, rlfl.PATH_BASIC, r, flags, 0.0)
        
        # Or real path A*
        path = rlfl.path(self.map_number, p1, p2, rlfl.PATH_ASTAR, r, flags, 7.0)
        
        # Lets calculate FOV from 3 using recursive shadowcasting
        # with a light source radius of 6
        rlfl.fov(self.map_number, p3, rlfl.FOV_SHADOW, 6)
        self.print_map(p3) 
        
        # Use the scatter function to find a random spot (summon, teleport)
        # Here we want an open cell within range 16 from p
        require_los = False
        ps = rlfl.scatter(self.map_number, p0, 16, rlfl.CELL_OPEN, require_los)
        super().print_map([], p0, ps)
        
        # We can use the projections to create spell effects
        # For this see project_example.py

if __name__ == '__main__':
    example = Full_example()
    example.show()