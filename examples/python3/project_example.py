"""
    Example projection generation
    
    Projection methods:
    rlfl.project_beam(
        map,
        origin,
        target,
        range,
        flags
    )
    Produces line projection
    ....................
    ....................
    .@*******#..........
    ....................
    ....................
    
    rlfl.project_ball(
        map,
        origin,
        target,
        radius,
        range,
        flags
    )
    Produces line to ball projection
    Note: ball spells are usually a combination 
          of beam and ball.
    ........*...........
    .......***..........
    .@....*****.........
    .......***..........
    ........*...........
    
    rlfl.project_cone(
        map,
        origin,
        target,
        radius,
        range,
        flags
    )
    Produces breath projection
    .........***.......
    .....********......
    .D********@**......
    .....********......
    .........***.......
    
    Available flags:
    
    rlfl.PROJECT_THRU    Tunnel through walls
    rlfl.PROJECT_STOP    Stop at first monster
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
                    
    def show_beam(self, source, target, rng=-1, flags=0):
        beam = rlfl.project_beam(self.mapnum, self.origos[source], self.origos[target], rng, flags)
        self.print_map(beam, self.origos[source], self.origos[target])
        
    def show_ball(self, source, target, rad=2, rng=-1, flags=0):
        ball = rlfl.project_ball(self.mapnum, self.origos[source], self.origos[target], rad, rng, flags)
        self.print_map(ball, self.origos[source], self.origos[target])
        
    def show_cone(self, source, target, rad=2, rng=-1, flags=0):
        cone = rlfl.project_cone(self.mapnum, self.origos[source], self.origos[target], rad, rng, flags)
        self.print_map(cone, self.origos[source], self.origos[target])
                    
if __name__ == '__main__':
    example = Project_example()
    example.create_map('emap')
    
    # Beam / bolt
    example.show_beam(1, 6)
    
    # Short range
    example.show_beam(1, 4, 5)
    
    # Travel through walls
    example.show_beam(1, 4, -1, rlfl.PROJECT_THRU)
    
    # stop on first enemy
    example.show_beam(1, 4, -1, rlfl.PROJECT_STOP)
    
    # simple ball
    example.show_ball(1, 6)
    
    # Huge ball
    example.show_ball(1, 4, 5)
    
    # Flash. Ball with range 0
    example.show_ball(1, 1, 2, 0)
    
    # Disintegration ball
    example.show_ball(1, 4, 2, -1, rlfl.PROJECT_THRU)
    
    # explode on first enemy
    example.show_ball(1, 4, 2, -1, rlfl.PROJECT_STOP)
    
    # Simple cone
    example.show_cone(1, 4)
    
    # Huge cone
    example.show_cone(1, 4, 5)
    
    # Disintegration cone
    example.show_cone(1, 4, 2,  -1, rlfl.PROJECT_THRU)
    
    
    