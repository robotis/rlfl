Projection functions
====================

Example: ::
	
	# Produces line projection
	# ....................
	# ....................
	# .@*******#..........
	# ....................
	# ....................
	rlfl.project_beam(map_number, p, p1)
	    
	# Produces line to ball projection
	# Note: ball spells are usually a combination 
	#      of beam and ball.
	# ........*...........
	# .......***..........
	# .@....*****.........
	# .......***..........
	# ........*...........
	rlfl.project_beam(map_number, p, p1, 6)
	    
	# Produces breath projection
	# .........***.......
	# .....********......
	# .D********@**......
	# .....********......
	# .........***.......
	rlfl.project_cone(map_number, p, p1, 3)
	
Projection flags
----------------

.. attribute:: rlfl.PROJECT_THRU

	Causes the projection to ignore walls
	
.. attribute:: rlfl.PROJECT_STOP

	Causes projection to stop on first cell marked rlfl.CELL_OCUP
	
.. attribute:: rlfl.PROJECT_PASS

	Causes projection not to stop at target
	
Function list
-------------

.. function:: rlfl.project_beam(map_number, source, target[, range, flags])

	Return a beam projection as a set on tuples `(x, y)`.
	
	`range` defaults to max range.
	
.. function:: rlfl.project_ball(map_number, source, target, radius[, range, flags])

	Return a ball shaped projection as a set on tuples `(x, y)`.
	
	`range` defaults to max range.
	
.. function:: rlfl.project_cone(map_number, source, target, radius[, range, flags])

	Return a cone shaped projection as a set on tuples `(x, y)`.
	
	`range` defaults to max range.
	