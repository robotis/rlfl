"""
"""
try:
    import rlfl
except ImportError:
    import sys
    sys.path.append('..')
    import rlfl
import RLFLExample

class Full_example(RLFLExample.RLFLExample):
    def print_map(self, p):
        fov = []
        for row in range(len(self.map)):
            for col in range(len(self.map[row])):
                if rlfl.has_flag(self.map_number, (row, col), rlfl.CELL_SEEN):
                    fov.append((row, col))
        super(Full_example, self).print_map(fov, p, p)
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
        have_los = rlfl.los(self.map_number, self.origos[1], self.origos[4])
        assert(have_los == False)
        # LOS between 2 and 3
        have_los = rlfl.los(self.map_number, self.origos[2], self.origos[3])
        assert(have_los == True)
        
        # Lets calculate FOV from 3 using recursive shadowcasting
        # with a light source radius of 6
        rlfl.fov(self.map_number, self.origos[3], rlfl.FOV_SHADOW, 6)
        self.print_map(self.origos[3]) 

if __name__ == '__main__':
    example = Full_example()
    example.show()