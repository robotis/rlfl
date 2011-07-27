Miscellaneous functions
=======================

.. function:: rlfl.scatter(map_number, p[, range, flags, need_los])

	Find and return a random location within `range`.
	
	`flags` defaults to  rlfl.CELL_OPEN.
	
	`need_los` is default False.
	
	`range` defaults to max range.
	
.. function:: rlfl.distance(p1, p2)

	returns distance between points.
	
.. function:: rlfl.randint(max)

	return a random number (0 <= n < max)
	
Miscellaneous constants
=======================

	These constants are decided at compile time.

.. attribute:: rlfl.MAX_WIDTH

	Max width of internal maps
	
.. attribute:: rlfl.MAX_HEIGHT

	Max height of internal maps

.. attribute:: rlfl.MAX_MAPS

	Maximum number of internal maps stored. Also applies to the path maps. 
	Each map has rlfl.MAX_MAPS path_maps available
	
.. attribute:: rlfl.MAP_PATHS

	Maximum number of internal paths stored.
	
.. attribute:: rlfl.MAP_PROJECTIONS

	Maximum number of internal projections stored.	
	
.. attribute:: rlfl.MAP_RADIUS

	Maximum radius. Valid in projections and FOV.
	
.. attribute:: rlfl.MAP_RANGE

	Maximum range of projections and paths