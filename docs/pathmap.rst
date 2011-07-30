Path maps
=========

Path maps are Dijkstra maps that can be created once from a single origin (player position), then used
to make entitys move both towards origin. rlfl.path_step_map() is called for the next step
on the map.

Example: ::
		
	# Define origin
	origin = (3, 31)
	
	# Create a path map from origin on existing RLFL map. 
	path_map_n = rlfl.path_fill_map(map_number, origin, 0.7)
	
	# Bootstrap
	p = (19, 23)
	
	# Create a path at most 110 steps long towards origin
	path = []
	for i in range(110):
		p = rlfl.path_step_map(example.mapnum, path_map_n, p)
		path.append(p)
	
	# Will produce (S is moving towards T):

	########################################################
	#..........#.........#################################.#
	#...*****************########################........+.#
	#..*.......#.........**********S######################.#
	#.*........#.........#####################.###########.#
	##*#########.........#####################.#############
	##*#########.........##########............####........#
	##*##......+.........#####################.##########..#
	##*#############+######.......############.............#
	##*######.....+.......#.......############.##########..#
	##*############.......#.......#######################..#
	##*############......./.......#.......####.............#
	#+*############.......#.......#.......########.######..#
	##*############.......#.......#.......####.........#####
	##*####################.......#.......####.........#####
	##*####.........###############.......####.........#####
	##*####.........#.............#.......####.........#####
	##*####.........#.............#.......####.........#...#
	#+*####.........#.............#.......######.#######...#
	##*####........********T..............###.......####...#
	##*####********.#.............####.######.......####...#
	#..****.........#.............#.................####...#
	#######.........#.............###########.......########
	########################################################
	
Function list
-------------

.. function:: rlfl.path_fill_map(map_number, origin[, diagonal_cost])

	Create a map and return its ID.
	
.. function:: rlfl.path_step_map(map_number, path_map_number, from_position)

	Returns the next step from `from_position` towards the `origin` used to create the path map.
	
.. function:: rlfl.path_clear_map(map_number, path_map_number)

	Delete the path map.
	
.. function:: rlfl.path_clear_all_maps(map_number)

	Delete all path maps on this map.
	
Safety maps
===========

Safety maps are used to move enitys away from the origin. Usefull for fleeing monsters.

Example:	::

	# Define origin
	origin = (17, 34)
	
	# Create a path map from origin on existing RLFL map. 
	safety_map_n = rlfl.path_fill_safety_map(map_number, origin, 0.7)
	
	# Bootstrap
	p = (19, 23)
	
	# Create a path at most 110 steps long away from origin
	path = []
	for i in range(110):
		p = rlfl.path_step_map(example.mapnum, safety_map_n, p)
		path.append(p)
	
	# Will produce (S is fleeing T):

	########################################################
	#..........#.........#################################.#
	#....................########################........+.#
	#..........#....................######################.#
	#..........#.........#####################.###########.#
	##.#########.........#####################.#############
	##.#########.........##########0...........####........#
	##.##3.....+.........#####################.##########..#
	##.#############+######2......############.............#
	##.######.....+.......#.......############.##########..#
	##.############.......#.......#######################..#
	##.############......./.......#.......####****.........#
	#+.############.......#.......#.......########*######..#
	##.############.......#.......#.......####....*....#####
	##.####################.......#.......####...*.....#####
	##.####.........###############.......####...*.....#####
	##.####.........#.............#.......####...*.....#####
	##.####.........#.............#...S...####...*.....#...#
	#+.####.........#.............#...*...######*#######...#
	##.####................T..........*...###.**....####...#
	##.####.........#.............####*######*......####...#
	#...............#.............#....******......5####...#
	#######.........#.............###########.......########
	########################################################
	
Functions
---------

..	function:: rlfl.path_fill_safety_map(map_number, origin[, diagonal_cost])

	Create a safety map and return its ID.
	
Autoexplore maps
================

Example:	::

	# Use CELL_PASS for any LOS blocking features that the player 
	# wants to cross. Remember to clear the flag after the player 
	# has (for example) opened the door.
	for x in map.width:
		for y in map.height:
			if map.cell(x, y).terrain == terrain_door:
				rlfl.set_flag(map_number, (x, y), rlfl.CELL_PASS)
				
	# Create a path map from origin on existing RLFL map. 
	explore_map_n = path_fill_autoexplore_map(map_number)
	
	# Find the step to the next unexplored cell
	p = rlfl.path_step_map(example.mapnum, explore_map_n, p)
	
Functions
---------

..	function:: path_fill_autoexplore_map(map_number)

	Create a auto explore map and return its ID.
