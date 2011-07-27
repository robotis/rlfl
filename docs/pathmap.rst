Path / Safety maps
==================

Path maps are Dijkstra maps that can be created once from a single origin (player position), then used
to make entitys move both towards and away from origin. rlfl.path_step_map() is called for the next step
on the map.

Example: ::
		
	# Define origin
	origin = example.origos[1]
	
	# Create a path map. example.origos[1] is a point on the map
	# that is the origin
	path_map_n = rlfl.path_fill_map(example.mapnum, origin, 10.0)
	
	# Bootstrap
	p = example.origos[6]
	
	# Create a path at most 110 steps long TOWARDS origin
	path = []
	for i in range(110):
		p = rlfl.path_step_map(example.mapnum, path_map_n, p)
		path.append(p)
	example.print_map(path, example.origos[1], example.origos[6])
	
	# Will produce:
	# ######################################################
	# ..........#.........#################################.
	# ..........**********########################........+.
	# .........*#.........**********S######################.
	# ..*******.#.........#####################.###########.
	# #*#########.........#####################.############
	# #*#########.........##########0...........####........
	# #*##3.....+.........#####################.##########..
	# #*#############+######2......############.............
	# #*######.....+.......#.......############.##########..
	# #*############.......#.......#######################..
	# #*############......./.......#.......####.....***T....
	# +*############.......#.......#.......########*######..
	# #*############.......#.......#.......####....*....####
	# #*####################.......#.......####....*....####
	# #*####.........###############.......####....*....####
	# #*####.........#......4......#.......####....*....####
	# #*####.........#.............#.......####...*.....#...
	# +*####.........#.............#.......######*#######...
	# #*####.**************************....###.**....####...
	# #*####*........#.............####*######*......####...
	# ..****.........#.............#....******......5####...
	# ######.........#.............###########.......#######
	# ######################################################
	
	
Function list
-------------

.. function:: rlfl.path_fill_map(map_number, origin[, diagonal_cost])

	Create a path map on map defined by map_number
	
.. function:: rlfl.path_step_map(map_number, path_map_number, from_position[, flee])

	Returns the next step from `from_position` towards the `origin` used to create the path map.
	If flee is set to True returns the next step away from the origin.
	
.. function:: rlfl.path_clear_map(map_number, path_map_number)

	Delete the path map.
	
.. function:: rlfl.path_clear_all_maps(map_number)

	Delete all path maps on this map.
