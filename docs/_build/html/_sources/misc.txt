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