Line Of Sight
=============

Example use ::

	import rlfl
	
	# Check line of sight between two points
	have_los = rlfl.los(map_number, p1, p2)
	
Function list:
--------------

.. function:: rlfl.los(map_number, p1, p2)

	Determine if there is a line of sight between two points. All cells
	in between have to be seen and non-blocking.
	
	All cells NOT marked rlfl.CELL_OPEN are considered to block LOS
	
	Returns True or False.
	
	
	