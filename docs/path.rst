Pathfinding
===========

Example: ::

	map_number = rlfl.create_map(width, height)
	
	# Create the a basic path
	# ..............................#
	# .S**..........................#
	# ....*****.....................#
	# .........*****................#
	# ..............****####........#
	# ..................*###........#
	# ..................####...T....#
	# ..................####........#
	path = rlfl.path(map_number, p1, p2)
    
	# Create a A* path
	# ..............................#
	# .S**..........................#
	# ....*****.....................#
	# .........*************........#
	# ..................####*.......#
	# ..................####.*......#
	# ..................####..*T....#
	# ..................####........#
	path = rlfl.path(map_number, p1, p2, rlfl.PATH_ASTAR)
	
Path flags
----------

.. attribute:: rlfl.PATH_BASIC

	A basic and cheap path between two points.
	
.. attribute:: rlfl.PATH_ASTAR

	A much better and more expensive pathfinding.
    
Function list
-------------

.. function:: rlfl.path(map_number, p1, p2[, algorithm, range, flags, diagonal_cost])

	Create and return a path between p1 and p2. Defaults to rlfl.PATH_BASIC.
	Returns a set of points (`(x, y)` tuples)
	
	This function creates and deletes a path internally.
	
.. function:: rlfl.create_path(map_number, p1, p2[, algorithm, range, flags, diagonal_cost])

	Create a path and return path_number.
	
.. function:: rlfl.delete_path(path_number)

	Delete a path.
	
.. function:: rlfl.path_size(path_number)

	Returns the size of a path
	
.. function:: rlfl.path_get(path_number)

	Returns path as a set of points (`(x, y)` tuples)
	
