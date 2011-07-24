/*
 +-----------------------------------------------------------+
 * @desc	Access functions to the RLFL packages
 * @file	rlfl.c
 * @package RLFL
 * @license GPL
 * <jtm@robot.is>
 +-----------------------------------------------------------+
 */
#include "headers/rlfl.h"

/* Storage for maps */
RLFL_map_t * RLFL_map_store[RLFL_MAX_MAPS];

/* Storage for paths */
RLFL_path_t * RLFL_path_store[RLFL_MAX_PATHS];

/* Storage for paths */
RLFL_list_t * RLFL_project_store[RLFL_MAX_PROJECTS];

// Private
static int alloc_map(unsigned int m, unsigned int w, unsigned int h);
static inline bool flag_valid(unsigned short flag);
/*
 +-----------------------------------------------------------+
 * @desc	Create new map, destroy old if exists
 +-----------------------------------------------------------+
 */
int
RLFL_new_map(unsigned int w, unsigned int h)
{
	unsigned int i;
	for(i=0; i<RLFL_MAX_MAPS; i++)
	{
		if(!RLFL_map_store[i]) break;
	}

	if(i >= RLFL_MAX_MAPS)
	{
		return RLFL_ERR_NO_MAP;
	}

	int e = alloc_map(i, w, h);

	if(e) return e;

	return i;
}
/*
 +-----------------------------------------------------------+
 * @desc	Destroy map if exists
 +-----------------------------------------------------------+
 */
err
RLFL_wipe_map(unsigned int m)
{
	if((m < RLFL_MAX_MAPS) && RLFL_map_store[m])
	{
		free(RLFL_map_store[m]->cells);
		free(RLFL_map_store[m]);
		RLFL_map_store[m] = NULL;
		return RLFL_SUCCESS;
	}
	return RLFL_ERR_GENERIC;
}
/*
 +-----------------------------------------------------------+
 * @desc	Wipe all maps
 +-----------------------------------------------------------+
 */
void
RLFL_wipe_all()
{
	unsigned int i;
	for(i=0; i<RLFL_MAX_MAPS; i++) {
		RLFL_wipe_map(i);
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	Allocate memory to new map
 +-----------------------------------------------------------+
 */
static err
alloc_map(unsigned int m, unsigned int w, unsigned int h)
{
	if((m < RLFL_MAX_MAPS) && !RLFL_map_store[m])
	{
		if(w >= RLFL_MAX_WIDTH || h >= RLFL_MAX_HEIGHT) {
			return RLFL_ERR_SIZE;
		}

		RLFL_map_t *map;
		map = (RLFL_map_t*) calloc(sizeof(RLFL_map_t), 1);
		if(map == NULL)
			return RLFL_ERR_GENERIC;

		map->height = h;
		map->width = w;
		map->cells = (unsigned short *)calloc(sizeof(unsigned short), w * h);
		map->mnum = m;
		RLFL_map_store[m] = map;
		return RLFL_SUCCESS;
	}
	return RLFL_ERR_NO_MAP;
}
/*
 +-----------------------------------------------------------+
 * @desc	Check cell sanity
 +-----------------------------------------------------------+
 */
bool
RLFL_cell_valid(unsigned int m, unsigned int x, unsigned int y)
{
	if((m >= RLFL_MAX_MAPS) || !RLFL_map_store[m])
		return false;

	if(x < 0 || y < 0)
		return false;

	RLFL_map_t *map = RLFL_map_store[m];
	if(x >= map->width || y >= map->height)
		return false;

	return true;
}
/*
 +-----------------------------------------------------------+
 * @desc	Check map sanity
 +-----------------------------------------------------------+
 */
bool
RLFL_map_valid(unsigned int m)
{
	if((m >= RLFL_MAX_MAPS) || !RLFL_map_store[m])
		return false;

	return true;
}
/*
 +-----------------------------------------------------------+
 * @desc	Map size
 +-----------------------------------------------------------+
 */
err
RLFL_map_size(unsigned int m, unsigned int *w, unsigned int *h)
{
	if(!RLFL_map_valid(m)) {
		return RLFL_ERR_NO_MAP;
	}

	RLFL_map_t *map = RLFL_map_store[m];
	(*w) = map->width;
	(*h) = map->height;

	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Set flag
 +-----------------------------------------------------------+
 */
err
RLFL_set_flag(unsigned int m, unsigned int x, unsigned int y, unsigned short flag)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	if(!RLFL_cell_valid(m, x, y))
		return RLFL_ERR_OUT_OF_BOUNDS;

	if(!flag_valid(flag))
		return RLFL_ERR_FLAG;

	CELL(m, x, y) |= flag;

	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Query cell for flag
 +-----------------------------------------------------------+
 */
err
RLFL_has_flag(unsigned int m, unsigned int x, unsigned int y, unsigned short flag)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	if(!RLFL_cell_valid(m, x, y))
		return RLFL_ERR_OUT_OF_BOUNDS;

	if(!flag_valid(flag))
		return RLFL_ERR_FLAG;

	if(CELL(m, x, y) & flag)
		return true;

	return false;
}
/*
 +-----------------------------------------------------------+
 * @desc	Clear cell flag
 +-----------------------------------------------------------+
 */
err
RLFL_clear_flag(unsigned int m, unsigned int x, unsigned int y, unsigned short flag)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	if(!RLFL_cell_valid(m, x, y))
		return RLFL_ERR_OUT_OF_BOUNDS;

	if(!flag_valid(flag))
		return RLFL_ERR_FLAG;

	CELL(m, x, y) &= ~flag;
	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Query cell for flags
 * @param	unsigned int 	map number
 * @param	X
 * @param	Y
 +-----------------------------------------------------------+
 */
int
RLFL_get_flags(unsigned int m, unsigned int x, unsigned int y)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	if(!RLFL_cell_valid(m, x, y))
		return RLFL_ERR_OUT_OF_BOUNDS;

	return CELL(m, x, y);
}
/*
 +-----------------------------------------------------------+
 * @desc	Clear `flag` from entire map
 +-----------------------------------------------------------+
 */
err
RLFL_clear_map(unsigned int m, unsigned short flag)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	if(!flag_valid(flag))
		return RLFL_ERR_FLAG;

	int i;
	for(i=0; i<(RLFL_map_store[m]->width * RLFL_map_store[m]->height); i++)
	{
		RLFL_map_store[m]->cells[i] &= ~flag;
	}

	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Set flag on entire map
 +-----------------------------------------------------------+
 */
err
RLFL_fill_map(unsigned int m, unsigned short flag)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	if(!flag_valid(flag))
		return RLFL_ERR_FLAG;

	int i;
	for(i=0; i<(RLFL_map_store[m]->width * RLFL_map_store[m]->height); i++)
	{
		RLFL_map_store[m]->cells[i] |= flag;
	}

	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Check if flag is valid
 +-----------------------------------------------------------+
 */
static inline bool
flag_valid(unsigned short flag)
{
	if(flag < CELL_NONE || flag > CELL_MASK)
	{
		return false;
	}
	return true;
}
/*
* Approximate Distance between two points.
*
* When either the X or Y component dwarfs the other component,
* this function is almost perfect, and otherwise, it tends to
* over-estimate about one grid per fifteen grids of distance.
*
* Algorithm: hypot(dy,dx) = max(dy,dx) + min(dy,dx) / 2
* FI: Angband
* */
int
RLFL_distance(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	int dy, dx, d;

	/* Find the absolute y/x distance components */
	dy = (y1 > y2) ? (y1 - y2) : (y2 - y1);
	dx = (x1 > x2) ? (x1 - x2) : (x2 - x1);

	/* Approximate the distance */
	d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));

	/* Return the distance */
	return (d);
}
/*
 +-----------------------------------------------------------+
 * @desc	Standard "find me a location" function
 * 			Finds a point within range
 +-----------------------------------------------------------+
 */
err
RLFL_scatter(unsigned int m, unsigned int ox, unsigned int oy, unsigned int *dx, unsigned int *dy,
			 int range, unsigned int flag, bool need_los)
{
	if(!RLFL_map_valid(m))
			return RLFL_ERR_NO_MAP;

	if(!RLFL_cell_valid(m, ox, oy))
		return RLFL_ERR_OUT_OF_BOUNDS;

	if(!flag_valid(flag))
		return RLFL_ERR_FLAG;

	int nx, ny, fs = 0;

	if(range < 0) range = RLFL_MAX_RANGE;

	/* Pick a location */
	while(true)
	{
		/* Pick a new location */
		ny = RLFL_randspread(oy, range);
		nx = RLFL_randspread(ox, range);

		/* Ignore annoying locations */
		if(!RLFL_cell_valid(m, nx, ny))
			continue;

		/* Ignore "excessively distant" locations */
		if((range > 1) && (RLFL_distance(ox, oy, nx, ny) > range))
			continue;

		/* Require "line of sight" */
		if(!need_los || RLFL_los(m, ox, oy, nx, ny))
		{
			if(flag && !RLFL_has_flag(m, nx, ny, flag))
				continue;

			/* Found */
			break;
		}

		/* Failsafe */
		if(fs++ > 5000)
		{
			(*dy) = oy;
			(*dx) = ox;
			return RLFL_ERR_GENERIC;
		}
	}

	/* Save the location */
	(*dy) = ny;
	(*dx) = nx;

	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Create new path
 +-----------------------------------------------------------+
 */
err
RLFL_path_create(unsigned int m, unsigned int ox, unsigned int oy, unsigned int dx, unsigned int dy,
				 unsigned int algorithm, int range, unsigned int flags, float dcost)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	if(!(RLFL_cell_valid(m, ox, oy) && RLFL_cell_valid(m, dx, dy)))
		return RLFL_ERR_OUT_OF_BOUNDS;

	switch(algorithm) {
		case PATH_BASIC :
			return RLFL_path_basic(m, ox, oy, dx, dy, range, flags, false);
		case PATH_ASTAR :
			return RLFL_path_astar(m, ox, oy, dx, dy, range, flags, dcost);
	}

	return RLFL_ERR_GENERIC;
}
/*
 +-----------------------------------------------------------+
 * @desc	Path size
 +-----------------------------------------------------------+
 */
int
RLFL_path_size(unsigned int p)
{
	if((p >= RLFL_MAX_PATHS) || !RLFL_path_store[p])
		return RLFL_ERR_NO_PATH;

	return RLFL_path_store[p]->size;
}
/*
 +-----------------------------------------------------------+
 * @desc	Delete path
 +-----------------------------------------------------------+
 */
err
RLFL_path_delete(unsigned int p)
{
	if((p >= RLFL_MAX_PATHS) || !RLFL_path_store[p])
		return RLFL_ERR_NO_PATH;

	RLFL_list_delete(RLFL_path_store[p]->path);
	free(RLFL_path_store[p]);
	RLFL_path_store[p] = NULL;

	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Fetch step from path
 +-----------------------------------------------------------+
 */
err
RLFL_path_step(unsigned int p, unsigned int s, unsigned int *x, unsigned int *y)
{
	if((p >= RLFL_MAX_PATHS) || !RLFL_path_store[p])
		return RLFL_ERR_NO_MAP;

	RLFL_path_t *path = RLFL_path_store[p];

	if(!path)
		return RLFL_ERR_NO_MAP;

	RLFL_step_t *step = RLFL_list_get(path->path, s);
	if(!step)
		return RLFL_ERR_GENERIC;

	*x = step->X;
	*y = step->Y;

	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Field of view.
 +-----------------------------------------------------------+
 */
err
RLFL_fov(unsigned int m, unsigned int ox, unsigned int oy, unsigned int radius,
		unsigned int algorithm, bool light_walls)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	if(!RLFL_cell_valid(m, ox, oy))
		return RLFL_ERR_OUT_OF_BOUNDS;

	if(radius >= RLFL_MAX_RADIUS)
		return RLFL_ERR_GENERIC;

	RLFL_map_t *map = RLFL_map_store[m];
	if(radius == 0)
	{
		int max_radius_x = map->width - ox;
		int max_radius_y = map->height - oy;
		max_radius_x = MAX(max_radius_x, ox);
		max_radius_y = MAX(max_radius_y, oy);
		radius = (int)(sqrt(max_radius_x * max_radius_x + max_radius_y * max_radius_y)) + 1;
	}

	RLFL_clear_map(m, CELL_SEEN|CELL_LIT);
	switch(algorithm)
	{
		case FOV_CIRCULAR :
			return RLFL_fov_circular_raycasting(m, ox, oy, radius, light_walls);
		case FOV_DIAMOND :
			return RLFL_fov_diamond_raycasting(m, ox, oy, radius, light_walls);
		case FOV_SHADOW :
			return RLFL_fov_recursive_shadowcasting(m, ox, oy, radius, light_walls);
		case FOV_PERMISSIVE :
			return RLFL_fov_permissive(m, ox, oy, radius, light_walls);
		case FOV_DIGITAL :
			return RLFL_fov_digital(m, ox, oy, radius, light_walls);
		case FOV_RESTRICTIVE:
			return RLFL_fov_restrictive_shadowcasting(m, ox, oy, radius, light_walls);
	}

	return RLFL_ERR_OUT_OF_BOUNDS;
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
err
RLFL_fov_finish(unsigned int m, int x0, int y0, int x1, int y1, int dx, int dy)
{
	if((m >= RLFL_MAX_MAPS) || !RLFL_map_store[m])
		return RLFL_ERR_NO_MAP;

	RLFL_map_t *map = RLFL_map_store[m];
	int cx, cy, x2, y2;
	int nc = (map->width * map->height);
	unsigned int offset, offset2;

	for(cx=x0; cx <= x1; cx++)
	{
		for(cy=y0; cy <= y1; cy ++)
		{
			x2 = cx + dx;
			y2 = cy + dy;
			offset = cx + (cy * map->width);
			if (offset < (unsigned)nc && RLFL_has_flag(m, cx, cy, CELL_SEEN) && RLFL_has_flag(m, cx, cy, CELL_OPEN))
			{
				if (x2 >= x0 && x2 <= x1)
				{
					offset2 = x2 + (cy * map->width);
					if (offset2 < (unsigned)nc && !RLFL_has_flag(m, cx, cy, CELL_OPEN))
					{
						RLFL_set_flag(m, cx, cy, CELL_FOV);
					}
				}
				if ( y2 >= y0 && y2 <= y1 )
				{
					offset2 = cx + (y2 * map->width);
					if (offset2 < (unsigned)nc && !RLFL_has_flag(m, cx, cy, CELL_OPEN))
					{
						RLFL_set_flag(m, cx, cy, CELL_FOV);
					}
				}
				if ( x2 >= x0 && x2 <= x1 && y2 >= y0 && y2 <= y1 )
				{
					offset2 = x2 + (y2 * map->width);
					if (offset2 < (unsigned)nc && !RLFL_has_flag(m, cx, cy, CELL_OPEN))
					{
						RLFL_set_flag(m, cx, cy, CELL_FOV);
					}
				}
			}
		}
	}
	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME.
 +-----------------------------------------------------------+
 */
err
RLFL_project_ball(unsigned int m, unsigned int x1, unsigned int y1, unsigned int x2,
			     unsigned int y2, unsigned int rad, int range, unsigned int flags)
{
	if((m >= RLFL_MAX_MAPS) || !RLFL_map_store[m])
		return RLFL_ERR_NO_MAP;

	return RLFL_project(m, x1, y1, x2, y2, rad, range, flags | PROJECT_JUMP);
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME.
 +-----------------------------------------------------------+
 */
err
RLFL_project_beam(unsigned int m, unsigned int x1, unsigned int y1, unsigned int x2,
				 unsigned int y2, int range, unsigned int flags)
{
	if((m >= RLFL_MAX_MAPS) || !RLFL_map_store[m])
		return RLFL_ERR_NO_MAP;

	return RLFL_project(m, x1, y1, x2, y2, 0, range, flags);
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME.
 +-----------------------------------------------------------+
 */
err
RLFL_project_wave(unsigned int m, unsigned int x1, unsigned int y1, unsigned int rad, int range, unsigned int flags)
{
	if((m >= RLFL_MAX_MAPS) || !RLFL_map_store[m])
		return RLFL_ERR_NO_MAP;

	return RLFL_project(m, x1, y1, -1, -1, rad, range, PROJECT_WAVE|flags);
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME.
 +-----------------------------------------------------------+
 */
err
RLFL_project_cone(unsigned int m, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,
				 unsigned int rad, int range, unsigned int flags)
{
	if((m >= RLFL_MAX_MAPS) || !RLFL_map_store[m])
		return RLFL_ERR_NO_MAP;

	return RLFL_project(m, x1, y1, x2, y2, rad, range, PROJECT_CONE|flags);
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME.
 +-----------------------------------------------------------+
 */
err
RLFL_project_cloud(unsigned int m, unsigned int x1, unsigned int y1, unsigned int rad, unsigned int flags)
{
	if((m >= RLFL_MAX_MAPS) || !RLFL_map_store[m])
		return RLFL_ERR_NO_MAP;

	return RLFL_project(m, x1, y1, -1, -1, rad, -1, flags);
}
