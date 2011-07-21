#include "headers/rlf.h"
#include "headers/path.h"

static err add_step(unsigned int m, int x, int y);
static bool test_step(unsigned int m, unsigned int x, unsigned int y, unsigned int dx, unsigned int dy, int range, unsigned int flg);

static path_t* path;

err
RLF_path_basic(unsigned int map, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,
			   int range, unsigned int flg)
{
	/* assert map */
	if(!map_store[map]) return RLF_ERR_NO_MAP;
	unsigned int i;
	for(i=0; i<MAX_PATH; i++){
		if(!path_store[i]) break;
	}
	/* assert path */
	if(i >= MAX_PATH) return RLF_ERR_FLAG;

	/* assert cells */
	if(!RLF_cell_valid(map, x1, y1)) return RLF_ERR_GENERIC;
	if(!RLF_cell_valid(map, x2, y2)) return RLF_ERR_GENERIC;

	/* No path necessary (or allowed) */
	if ((x1 == x2) && (y1 == y2)) return RLF_ERR_GENERIC;

	if(range < 0) range = MAX_RANGE;

	/* init path */
	path = (path_t *)calloc(sizeof(path_t), 1);
	if(path == NULL) return RLF_ERR_GENERIC;
	path->path = RLF_list_create();
	path->ox = x1;
	path->oy = y1;
	path->dx = x2;
	path->dy = y2;
	path->map = map;
	path->size = 0;
	path->valid = true;
	RLF_clear_map(map, CELL_PATH);

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
			if(test_step(map, x, y, x2, y2, range, flg)) break;

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
			if(test_step(map, x, y, x2, y2, range, flg)) break;

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
			if(test_step(map, x, y, x2, y2, range, flg)) break;

			/* Advance (Y) */
			y += sy;

			/* Advance (X) */
			x += sx;
		}
	}
	/* Store it */
	path_store[i] = path;
	path = NULL;

	/* OK */
	return RLF_SUCCESS;
}
static bool
test_step(unsigned int m, unsigned int x, unsigned int y, unsigned int dx, unsigned int dy,
		  int range, unsigned int flg)
{
	/* Save grid */
	add_step(m, x, y);

	if (!(flg & (PROJECT_BEAM)))
	{
		/* Stop at destination grid */
		if ((x == dx) && (y == dy)) return true;
	}

	if (flg & (PROJECT_THRU))
	{
		/* Tunnel through features */
		if(RLF_has_flag(m, x, y, CELL_PERM)) return true;
	}
	else
	{
		/* Always stop at non-initial wall grids */
		if(!RLF_has_flag(m, x, y, CELL_OPEN)) return true;
	}

	if (flg & (PROJECT_STOP))
	{
		/* Stop at non-initial monsters/players */
		if(RLF_has_flag(m, x, y, CELL_OCUP)) return true;
	}

	/* Stay sane */
	if (!RLF_cell_valid(m, x, y)) return true;

	/* OK */
	return false;
}
static err
add_step(unsigned int m, int x, int y) {
	step_t *step = (step_t *)calloc(sizeof(step_t), 1);
	if(step == NULL) return RLF_ERR_GENERIC;
	step->X = x;
	step->Y = y;
	RLF_list_append(path->path, step);
	path->size++;
	RLF_set_flag(m, step->X, step->Y, CELL_PATH);
	return RLF_SUCCESS;
}
