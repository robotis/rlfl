"""
    Example RLFL FOV generation
    
    FOV methods are called:
    rlfl.fov(
        map,
        origin,
        radius,
        algorithm,
        light walls (default True)
    )
    
    All the algorithms start be clearing rlfl.CELL_SEEN from the map
    then mark the new `seen` cells with rlfl.CELL_SEEN and rlfl.CELL_MEMO
    
    Cells need to be marked rlfl.CELL_OPEN to not block FOV
    
    Algorithms available:
    
    rlfl.FOV_CIRCULAR
    rlfl.FOV_SHADOW
    rlfl.FOV_RESTRICTIVE
    rlfl.FOV_PERMISSIVE
    rlfl.FOV_DIAMOND
    rlfl.FOV_DIGITAL
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

class FOV_example(RLFLExample.RLFLExample):
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
                    # Set non-wall grids as open
                    rlfl.set_flag(self.mapnum, p, rlfl.CELL_OPEN)
                    
    def print_map(self, p):
        fov = []
        for row in range(len(self.map)):
            for col in range(len(self.map[row])):
                if rlfl.has_flag(self.mapnum, (row, col), rlfl.CELL_SEEN):
                    fov.append((row, col))
        super(FOV_example, self).print_map(fov, p, p)
                    
    def show_circular(self, p, rad, light_walls=True):
        rlfl.fov(self.mapnum, self.origos[p], rad, rlfl.FOV_CIRCULAR, light_walls);  
        self.print_map(self.origos[p])  
        
    def show_shadowcasting(self, p, rad, light_walls=True):
        rlfl.fov(self.mapnum, self.origos[p], rad, rlfl.FOV_SHADOW, light_walls);  
        self.print_map(self.origos[p])    

    def show_restrictive(self, p, rad, light_walls=True):
        rlfl.fov(self.mapnum, self.origos[p], rad, rlfl.FOV_RESTRICTIVE, light_walls);  
        self.print_map(self.origos[p])  
        
    def show_permissive(self, p, rad, light_walls=True):
        rlfl.fov(self.mapnum, self.origos[p], rad, rlfl.FOV_PERMISSIVE, light_walls);  
        self.print_map(self.origos[p])  
        
    def show_diamond(self, p, rad, light_walls=True):
        rlfl.fov(self.mapnum, self.origos[p], rad, rlfl.FOV_DIAMOND, light_walls);  
        self.print_map(self.origos[p]) 
        
    def show_digital(self, p, rad, light_walls=True):
        rlfl.fov(self.mapnum, self.origos[p], rad, rlfl.FOV_DIGITAL, light_walls);  
        self.print_map(self.origos[p]) 
        

if __name__ == '__main__':
    example = FOV_example()
    example.create_map('tmap')
    
    example.show_circular(0, 3)
    example.show_circular(0, 3, False)
    example.show_circular(1, 6)
    example.show_circular(1, 6, False)
    
    example.show_shadowcasting(0, 3)
    example.show_shadowcasting(0, 3, False)
    example.show_shadowcasting(1, 6)
    example.show_shadowcasting(1, 6, False)
    
    example.show_restrictive(0, 3)
    example.show_restrictive(0, 3, False)
    example.show_restrictive(1, 6)
    example.show_restrictive(1, 6, False)
    
    example.show_permissive(0, 3)
    example.show_permissive(0, 3, False)
    example.show_permissive(1, 6)
    example.show_permissive(1, 6, False)
    
    example.show_diamond(0, 6)
    example.show_diamond(0, 6, False)
    example.show_diamond(1, 6)
    example.show_diamond(1, 6, False)
    
    example.show_digital(0, 3)
    example.show_digital(0, 3, False)
    example.show_digital(1, 6)
    example.show_digital(1, 6, False)
    
    
    