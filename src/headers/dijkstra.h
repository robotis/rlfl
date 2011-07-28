/*
	RLFL dijkstra types

    Copyright (C) 2011

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>

    <jtm@robot.is>
*/
#include <limits.h> // MAX_INT

typedef struct link link;
struct link
{
	unsigned int x;
	unsigned int y;

	float distance;
	float cost;

	int state;

	link* right;
	link* left;
};

typedef struct
{
	unsigned int w;
	unsigned int h;

	float dcost;

	link front;

	link* links;
} RLFL_dijkstra_map;
