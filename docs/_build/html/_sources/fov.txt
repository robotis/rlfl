Field of view
=============

Example: ::

	map_number = rlfl.create_map(width, height)
	rlfl.fov(map_number, (10, 10), 6, rlfl.FOV_SHADOW)
	
Algorithms available
--------------------

.. attribute:: rlfl.PROJECT_CIRCULAR

	Circular ray casting.

.. attribute:: rlfl.FOV_DIAMOND

	Diamond raycasting.

.. attribute:: rlfl.FOV_SHADOW

	Recursive shadowcasting.

.. attribute:: rlfl.FOV_DIGITAL

	Digital FOV algorithm. Rather slow.

.. attribute:: rlfl.FOV_RESTRICTIVE

	Restrictive Precise Angle Shadowcasting (MRPAS).

.. attribute:: rlfl.FOV_PERMISSIVE

	Permissive raycasting.

FOV
---

.. function:: rlfl.fov(map_number, origin, radius[, algorithm, light_walls])

	Computes a field of vision on the map, marking all cells in the fov with
	(rlfl.CELL_SEEN | rlfl.CELL_MEMO).
	
	
	
	