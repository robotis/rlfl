.. RLFL documentation master file, created by
   sphinx-quickstart on Sun Jul 24 11:33:36 2011.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

RogueLike Function Library
================================

.. module:: rlfl

This is a collection of functions for roguelike games for use with the python programming language. FOV, 
pathfinding, projection, LOS and various helper functions. This is here to aid me in my hobby to create
a python roguelike. The aim is to build on alot more AI and fun stuff.

This is not a display library and contains no display functions.

Here's an example, to give you an impression::

	import rlfl
	
	# MAP defined here as an example is a list of strings
	self.map = MAP
    
	# Create the RLFL internal map
	width = len(self.map)
	height = len(self.map[0])
	self.map_number = rlfl.create_map(width, height)
    
	# We now have a map number representing the 
	# internal map in rlfl
    
	# initialize the map
	# `#` represent walls
	for row in range(len(self.map)):
		for col in range(len(self.map[row])):
			if self.map[row][col] != '#':
				p = (row, col)
                
				# Set non-wall grids as open (do not block sight)
				rlfl.set_flag(self.map_number, p, rlfl.CELL_OPEN)
				
	# Lets calculate FOV from (10, 10) using recursive shadowcasting
	# with a light source radius of 6
	rlfl.fov(self.map_number, (10, 10), rlfl.FOV_SHADOW, 6)

Contents
========

.. toctree::
   :maxdepth: 2
   
   Working with the map <map>
   Pathfinding <path>
   Line of sight <los>
   FOV, Field of view <fov>
   Projections <project>
   Misc <misc>

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

