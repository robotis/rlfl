Using the RLFL map
==================

Example: ::

	map_number = rlfl.create_map(width, height)
	p = (10, 10)
	rlfl.set_flag(map_number, p, rlfl.CELL_LIT)
	
Function list
-------------

.. function:: rlfl.create_map(width, height)

	Return the number allocated to a new map.
	
.. function:: rlfl.delete_map(map_number)

	Delete map and free resources.
	
.. function:: rlfl.delete_all_maps()

	Delete all allocated maps.
	
.. function:: rlfl.map_size(map_number)

	Returns a `(width, height)` tuple.
	
.. function:: rlfl.fill_map(map_number, flags)

	Sets `flags` on all cells of the map.
	
.. function:: rlfl.clear_map(map_number[, flags])

	Clears `flags` on all cells of the map. If `flags` is omitted
	clears all flags.
	
.. function:: rlfl.has_flag(map_number, p, flags)

	Returns True if flag(s) are set on cell.
	
.. function:: rlfl.set_flag(map_number, p, flags)

	Set flag(s) on cell.
	
.. function:: rlfl.clear_flag(map_number, p, flags)

	Clear flag(s) on cell.
	
.. function:: rlfl.get_flags(map_number, p)

	Returns all flag(s) set on cell.
	
Map flags
---------

Example: ::

	rlfl.fill_map(map_number, rlfl.CELL_SEEN | rlfl.CELL_LIT)
	rlfl.clear_map(map_number, rlfl.CELL_MEMO);
	
.. attribute:: rlfl.CELL_DARK

	Special flag. used internally.
	
.. attribute:: rlfl.CELL_OPEN

	Cell is walkable and does not block LOS or FOV.
	
.. attribute:: rlfl.CELL_VIEW

	Special flag. used internally.
	
.. attribute:: rlfl.CELL_LIT

	Cell is lit
	
.. attribute:: rlfl.CELL_WALK

	Special flag. Cell is walkable. (Illusion walls)
	
.. attribute:: rlfl.CELL_MEMO

	Cell is memorized.
	
.. attribute:: rlfl.CELL_SEEN

	Cell is in FOV.
	
.. attribute:: rlfl.CELL_ROOM

	Cell is part of a room.
	
.. attribute:: rlfl.CELL_GLOW

	Cell is permalit. Has own light source.

.. attribute:: rlfl.CELL_PATH

	Special flag. Cell is in a path.
	
.. attribute:: rlfl.CELL_OCUP

	Cell is occupied.
	
.. attribute:: rlfl.CELL_PERM

	Cell permanent wall. Blocks everything. This flag is set at map creation on the outmost walls of the map.
	
.. attribute:: rlfl.CELL_REFL

	Cell reflects some projections.

.. attribute:: rlfl.CELL_PASS

	Used for autoexplore-maps to mark `passible` things (like doors).
	
.. attribute:: rlfl.CELL_GOAL

	Goal point for use on custom pathmaps.
	
.. attribute:: rlfl.CELL_MARK

	Extra flag, use for whatever you like.
	
.. attribute:: rlfl.CELL_MASK

	Cell mask. All flags.

