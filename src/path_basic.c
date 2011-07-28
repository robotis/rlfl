/*
	RLFL basic path.

	Used to determine the path taken by a projection, non-smart monsters.

	The projection will always start from the grid (y1,x1), and will travel
	towards the grid (y2,x2), touching one grid per unit of distance along
	the major axis, and stopping when it enters the destination grid or a
	wall grid, or has travelled the maximum legal distance of "range".

	The "flg" flags can be used to modify the behavior of this function.

	In particular, the "PROJECT_STOP" and "PROJECT_THRU" flags have the same
	semantics as they do for the "project" function, namely, that the path
	will stop as soon as it hits a monster, or that the path will continue
	through the destination grid, respectively.

	"PROJECT_STOP" will stop at first occupied cell
	"PROJECT_THRU" will travel though non-open cells

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
#include "headers/rlfl.h"
#include "headers/path.h"

static err add_step(unsigned int m, int x, int y);
static bool test_step(unsigned int m, unsigned int x, unsigned int y, unsigned int dx, unsigned int dy, int range, unsigned int flg);

static RLFL_path_t* path;

err
RLFL_path_basic(unsigned int map, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,
			   int range, unsigned long flags)
{
	/* assert map */
	if(!RLFL_map_valid(map))
		return RLFL_ERR_NO_MAP;

	if(!(RLFL_cell_valid(map, x1, y1) && RLFL_cell_valid(map, x2, y2)))
		return RLFL_ERR_OUT_OF_BOUNDS;

	unsigned int path_n;
	for(path_n=0; path_n<RLFL_MAX_PATHS; path_n++){
		if(!RLFL_path_store[path_n]) break;
	}
	/* assert path */
	if(path_n >= RLFL_MAX_PATHS) return RLFL_ERR_FLAG;

	/* assert cells */
	if(!RLFL_cell_valid(map, x1, y1)) return RLFL_ERR_GENERIC;
	if(!RLFL_cell_valid(map, x2, y2)) return RLFL_ERR_GENERIC;

	/* No path necessary (or allowed) */
	if ((x1 == x2) && (y1 == y2)) return RLFL_ERR_GENERIC;

	if(range < 0) range = RLFL_MAX_RANGE;

	/* init path */
	path = (RLFL_path_t *)calloc(sizeof(RLFL_path_t), 1);
	if(path == NULL) return RLFL_ERR_GENERIC;
	path->path = RLFL_list_create();
	path->ox = x1;
	path->oy = y1;
	path->dx = x2;
	path->dy = y2;
	path->map = map;
	path->size = 0;
	path->valid = true;
	RLFL_clear_map(map, CELL_PATH);

	int y, x;

	int n = 0;
	int k = 0;

	/* Absolute */
	int ay, ax;

	/* Offsets */
	int sy, sx;

	/* Fractions */
	int frac;

	/* Scale factors */
	int full, half;

	/* Slope */
	int m;

	/* Analyze "dy" */
	if (y2 < y1)
	{
		ay = (y1 - y2);
		sy = -1;
	}
	else
	{
		ay = (y2 - y1);
		sy = 1;
	}

	/* Analyze "dx" */
	if (x2 < x1)
	{
		ax = (x1 - x2);
		sx = -1;
	}
	else
	{
		ax = (x2 - x1);
		sx = 1;
	}

	/* Reverse path */
	if(flags & PROJECT_REVR)
	{
		ax = -ax;
		ay = -ay;
		sx = -sx;
		sy = -sy;
	}


	/* Number of "units" in one "half" grid */
	half = (ay * ax);

	/* Number of "units" in one "full" grid */
	full = half << 1;

	/* Vertical */
	if (ay > ax)
	{
		/* Let m = ((dx/dy) * full) = (dx * dx * 2) */
		m = ax * ax * 2;

		/* Start */
		y = y1 + sy;
		x = x1;

		frac = m;

		if (frac > half)
		{
			/* Advance (X) part 2 */
			x += sx;

			/* Advance (X) part 3 */
			frac -= full;

			/* Track distance */
			k++;
		}

		/* Create the projection path */
		while (1)
		{
			/* Hack -- Check maximum range */
			if ((n + (k >> 1)) >= range) break;

			/* Test step */
			if(test_step(map, x, y, x2, y2, range, flags)) break;

			/* Slant */
			if (m)
			{
				/* Advance (X) part 1 */
				frac += m;

				/* Horizontal change */
				if (frac > half)
				{
					/* Advance (X) part 2 */
					x += sx;

					/* Advance (X) part 3 */
					frac -= full;

					/* Track distance */
					k++;
				}
			}

			/* Advance (Y) */
			y += sy;
		}
	}

	/* Horizontal */
	else if (ax > ay)
	{
		/* Let m = ((dy/dx) * full) = (dy * dy * 2) */
		m = ay * ay * 2;

		/* Start */
		y = y1;
		x = x1 + sx;

		frac = m;

		/* Vertical change */
		if (frac > half)
		{
			/* Advance (Y) part 2 */
			y += sy;

			/* Advance (Y) part 3 */
			frac -= full;

			/* Track distance */
			k++;
		}

		/* Create the projection path */
		while (1)
		{
			/* Hack -- Check maximum range */
			if ((n + (k >> 1)) >= range) break;

			/* Test step */
			if(test_step(map, x, y, x2, y2, range, flags)) break;

			/* Slant */
			if (m)
			{
				/* Advance (Y) part 1 */
				frac += m;

				/* Vertical change */
				if (frac > half)
				{
					/* Advance (Y) part 2 */
					y += sy;

					/* Advance (Y) part 3 */
					frac -= full;

					/* Track distance */
					k++;
				}
			}

			/* Advance (X) */
			x += sx;
		}
	}

	/* Diagonal */
	else
	{
		/* Start */
		y = y1 + sy;
		x = x1 + sx;

		/* Create the projection path */
		while (1)
		{
			/* Hack -- Check maximum range */
			if ((n + (n >> 1)) >= range) break;

			/* Test step */
			if(test_step(map, x, y, x2, y2, range, flags)) break;

			/* Advance (Y) */
			y += sy;

			/* Advance (X) */
			x += sx;
		}
	}
	/* Store it */
	RLFL_path_store[path_n] = path;
	path = NULL;

	/* OK */
	return RLFL_SUCCESS;
}
/*
 * Test the current step for sanity and special effects
 *
 * */
static bool
test_step(unsigned int m, unsigned int x, unsigned int y, unsigned int dx, unsigned int dy,
		  int range, unsigned int flg)
{
	/* Stay sane */
	if (!RLFL_cell_valid(m, x, y)) return true;

	/* Save grid */
	add_step(m, x, y);

	if (flg & (PROJECT_THRU))
	{
		/* Tunnel through features */
		if(RLFL_has_flag(m, x, y, CELL_PERM)) return true;
	}
	else
	{
		/* Always stop at non-initial wall grids */
		if(!RLFL_has_flag(m, x, y, (CELL_OPEN | CELL_WALK))) return true;
	}

	if (flg & (PROJECT_STOP))
	{
		/* Stop at non-initial monsters/players */
		if(RLFL_has_flag(m, x, y, CELL_OCUP)) return true;
	}

	/* OK */
	return false;
}
/**
 * Add current step to the path structure
 *
 */
static err
add_step(unsigned int m, int x, int y) {
	RLFL_step_t *step = (RLFL_step_t *)calloc(sizeof(RLFL_step_t), 1);
	if(step == NULL) return RLFL_ERR_GENERIC;
	step->X = x;
	step->Y = y;
	RLFL_list_append(path->path, step);
	path->size++;
	RLFL_set_flag(m, x, y, CELL_PATH);
	return RLFL_SUCCESS;
}
