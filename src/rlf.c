/*
 +-----------------------------------------------------------+
 * @desc	Access functions to the RLE packages
 * @file	rlf.c
 * @package RLF
 * @license FIXME
 * <jtm@robot.is>
 +-----------------------------------------------------------+
 */
#include "headers/rlf.h"

/* Storage for maps */
map_t * map_store[MAX_MAPS];

/* Storage for paths */
path_t * path_store[MAX_PATH];

// Private
static int RLF_alloc_map(unsigned int m, unsigned int w, unsigned int h);
static inline bool RLF_flag_valid(unsigned short flag);
/*
 +-----------------------------------------------------------+
 * @desc	Create new map, destroy old if exists
 * @param	unsigned int	width
 * @param	unsigned int	height
 * @return 	int
 +-----------------------------------------------------------+
 */
int
RLF_new_map(unsigned int w, unsigned int h) {
	if(DEBUG) printf("NEW MAP\n");

	unsigned int i;
	for(i=0; i<MAX_MAPS; i++){
		if(!map_store[i]) break;
	}
	if(i >= MAX_MAPS) return RLF_ERR_NO_MAP;

	if(RLF_alloc_map(i, w, h)) {
		return i;
	}
	return RLF_ERR_GENERIC;
}
/*
 +-----------------------------------------------------------+
 * @desc	Destroy map if exists
 * @param	unsigned int 	map number
 * @return 	int
 +-----------------------------------------------------------+
 */
err
RLF_wipe_map(unsigned int m) {
	if(map_store[m]) {
		if(DEBUG) printf("WIPE MAP %d\n", m);
		free(map_store[m]->cells);
		free(map_store[m]);
		map_store[m] = NULL;
		return 0;
	}
	return -1;
}
/*
 +-----------------------------------------------------------+
 * @desc	Wipe all maps
 +-----------------------------------------------------------+
 */
void
RLF_wipe_all() {
	unsigned int i;
	for(i=0; i<MAX_MAPS; i++) {
		RLF_wipe_map(i);
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	Allocate memory to new map
 * @param	unsigned int 	map number
 * @return 	int
 +-----------------------------------------------------------+
 */
static int
RLF_alloc_map(unsigned int m, unsigned int w, unsigned int h) {
	if(!map_store[m]) {
		map_t *map;
		map = (map_t*) calloc(sizeof(map_t), 1);
		map->height = h;
		map->width = w;
		map->cells = (unsigned short *)calloc(sizeof(unsigned short), w * h);
		map->mnum = m;
		map_store[m] = map;
	}
	return -1;
}
/*
 +-----------------------------------------------------------+
 * @desc	Wipe all maps
 +-----------------------------------------------------------+
 */
bool
RLF_cell_valid(unsigned int m, unsigned int x, unsigned int y) {
	if(!map_store[m])
		return false;
	if(x < 0 || y < 0)
		return false;
	map_t *map = map_store[m];
	if(x >= map->width || y >= map->height)
		return false;
	return true;
}
/*
 +-----------------------------------------------------------+
 * @desc	Set flag
 * @param	unsigned int 	map number
 * @param	X
 * @param	Y
 * @param	flag
 +-----------------------------------------------------------+
 */
err
RLF_set_flag(unsigned int m, unsigned int x, unsigned int y, unsigned short flag) {
	if(!map_store[m]) return RLF_ERR_NO_MAP;
	if(!RLF_flag_valid(flag)) return RLF_ERR_GENERIC;
	CELL(m, x, y) |= flag;
	return 0;
}
/*
 +-----------------------------------------------------------+
 * @desc	Query cell for flag
 * @param	unsigned int 	map number
 * @param	X
 * @param	Y
 * @param	flag
 * @return 	bool
 +-----------------------------------------------------------+
 */
err
RLF_has_flag(unsigned int m, unsigned int x, unsigned int y, unsigned short flag) {
	if(!map_store[m]) return RLF_ERR_NO_MAP;
	if(!RLF_flag_valid(flag)) return RLF_ERR_GENERIC;
	if(CELL(m, x, y) & flag)
		return (int)true;
	return (int)false;
}
/*
 +-----------------------------------------------------------+
 * @desc	Clear cell flag
 * @param	unsigned int 	map number
 * @param	X
 * @param	Y
 * @param	flag
 +-----------------------------------------------------------+
 */
err
RLF_clear_flag(unsigned int m, unsigned int x, unsigned int y, unsigned short flag) {
	if(!map_store[m]) return RLF_ERR_NO_MAP;
	if(!RLF_flag_valid(flag)) return RLF_ERR_GENERIC;
	CELL(m, x, y) &= ~flag;
	return RLF_SUCCESS;
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
RLF_get_flags(unsigned int m, unsigned int x, unsigned int y) {
	if(!map_store[m]) return RLF_ERR_NO_MAP;
	return CELL(m, x, y);
}
/*
 +-----------------------------------------------------------+
 * @desc	Clear all flags from entire map
 * @param	unsigned int 	map number
 * @return 	VOID
 +-----------------------------------------------------------+
 */
err
RLF_clear_map(unsigned int m, unsigned short flag) {
	if(!map_store[m]) return RLF_ERR_NO_MAP;
	if(!RLF_flag_valid(flag)) return RLF_ERR_GENERIC;
	int i;
	for(i=0; i<(map_store[m]->width * map_store[m]->height); i++) {
		map_store[m]->cells[i] &= ~flag;
	}
	return RLF_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Set flag on entire map
 * @param	unsigned int 	map number
 * @param	flag
 * @return 	VOID
 +-----------------------------------------------------------+
 */
err
RLF_fill_map(unsigned int m, unsigned short flag) {
	if(!map_store[m]) return RLF_ERR_NO_MAP;
	if(!RLF_flag_valid(flag)) return RLF_ERR_GENERIC;
	int i;
	for(i=0; i<(map_store[m]->width * map_store[m]->height); i++) {
		map_store[m]->cells[i] |= flag;
	}
	return RLF_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Check if flag is valid
 * @param	flag
 * @return 	bool
 +-----------------------------------------------------------+
 */
static inline bool
RLF_flag_valid(unsigned short flag) {
	if(flag < CELL_NONE || flag > CELL_MASK) {
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
RLF_distance(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
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
RLF_scatter(unsigned int m, unsigned int ox, unsigned int oy, unsigned int *dx, unsigned int *dy,
			unsigned int range, unsigned int flag, bool need_los) {
	if(!map_store[m])
		return RLF_ERR_NO_MAP;
	int nx, ny, fs = 0;
	map_t *map = map_store[m];
	/* Pick a location */
	while(true) {
		/* Pick a new location */
		ny = RLF_randspread(oy, range);
		nx = RLF_randspread(ox, range);

		/* Ignore annoying locations */
		if(nx < 0 || nx >= map->width) continue;
		if(ny < 0 || ny >= map->height) continue;

		/* Ignore "excessively distant" locations */
		if((range > 1) && (RLF_distance(ox, oy, nx, ny) > range)) continue;

		/* Require "line of sight" */
		if(!need_los || RLF_los(m, ox, oy, nx, ny)) {
			if(flag && !RLF_has_flag(m, nx, ny, flag)) continue;
			break;
		}

		/* Failsafe */
		if(fs++ > 5000) {
			(*dy) = oy;
			(*dx) = ox;
			return RLF_ERR_GENERIC;
		}
	}

	/* Save the location */
	(*dy) = ny;
	(*dx) = nx;

	return RLF_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Create new path
 +-----------------------------------------------------------+
 */
err
RLF_path_create(unsigned int m, unsigned int ox, unsigned int oy, unsigned int dx, unsigned int dy,
				float dcost, unsigned int algorithm) {
	switch(algorithm) {
		case PATH_BASIC :
			return RLF_path_basic(m, ox, oy, dx, dy, dcost, false, false);
		case PATH_ASTAR :
			return RLF_path_basic(m, ox, oy, dx, dy, dcost, true, false);
	}
	return RLF_ERR_GENERIC;
}
/*
 +-----------------------------------------------------------+
 * @desc	Path size
 +-----------------------------------------------------------+
 */
int
RLF_path_size(unsigned int p) {
	if(!path_store[p])
		return RLF_ERR_GENERIC;
	return path_store[p]->size;
}
/*
 +-----------------------------------------------------------+
 * @desc	Delete path
 +-----------------------------------------------------------+
 */
err
RLF_path_delete(unsigned int p) {
	if(!path_store[p])
		return RLF_ERR_GENERIC;
	RLF_list_delete(path_store[p]->path);
	free(path_store[p]);
	path_store[p] = NULL;
	return RLF_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Fetch step from path
 +-----------------------------------------------------------+
 */
err
RLF_path_step(unsigned int p, unsigned int s, unsigned int *x, unsigned int *y) {
	path_t *path = path_store[p];
	if(!path) {
		return RLF_ERR_GENERIC;
	}
	step_t *step = RLF_list_get(path->path, s);
	if(!step) {
		return RLF_ERR_GENERIC;
	}
	*x = step->X;
	*y = step->Y;
	return RLF_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Field of view.
 +-----------------------------------------------------------+
 */
err
RLF_fov(unsigned int m, unsigned int ox, unsigned int oy, unsigned int radius,
		unsigned int algorithm, bool light_walls) {
	if(!map_store[m])
		return RLF_ERR_NO_MAP;
	map_t *map = map_store[m];
	if(radius == 0) {
		int max_radius_x = map->width - ox;
		int max_radius_y = map->height - oy;
		max_radius_x = MAX(max_radius_x, ox);
		max_radius_y = MAX(max_radius_y, oy);
		radius = (int)(sqrt(max_radius_x * max_radius_x + max_radius_y * max_radius_y)) + 1;
	}
	RLF_clear_map(m, CELL_SEEN|CELL_LIT);
	switch(algorithm) {
		case FOV_BASIC :
			return RLF_fov_circular_raycasting(m, ox, oy, radius, light_walls);
		case FOV_DIAMOND :
			return RLF_fov_diamond_raycasting(m, ox, oy, radius, light_walls);
		case FOV_SHADOW :
			return RLF_fov_recursive_shadowcasting(m, ox, oy, radius, light_walls);
		case FOV_PERMISSIVE :
			return RLF_fov_permissive(m, ox, oy, radius, light_walls);
		case FOV_DIGITAL :
			return RLF_fov_digital(m, ox, oy, radius, light_walls);
		case FOV_RESTRICTIVE:
			return RLF_fov_restrictive_shadowcasting(m, ox, oy, radius, light_walls);
	}
	return RLF_ERR_GENERIC;
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
err
RLF_fov_finish(unsigned int m, int x0, int y0, int x1, int y1, int dx, int dy) {
	if(!map_store[m]) return RLF_ERR_NO_MAP;
	map_t *map = map_store[m];
	int cx, cy, x2, y2;
	int nc = (map->width * map->height);
	unsigned int offset, offset2;
	for(cx=x0; cx <= x1; cx++) {
		for(cy=y0; cy <= y1; cy ++) {
			x2 = cx + dx;
			y2 = cy + dy;
			offset = cx + (cy * map->width);
			if (offset < (unsigned)nc && RLF_has_flag(m, cx, cy, CELL_SEEN) && RLF_has_flag(m, cx, cy, CELL_OPEN)) {
				if (x2 >= x0 && x2 <= x1) {
					offset2 = x2 + (cy * map->width);
					if (offset2 < (unsigned)nc && !RLF_has_flag(m, cx, cy, CELL_OPEN)) {
						RLF_set_flag(m, cx, cy, CELL_SEEN);
					}
				}
				if ( y2 >= y0 && y2 <= y1 ) {
					offset2 = cx + (y2 * map->width);
					if (offset2 < (unsigned)nc && !RLF_has_flag(m, cx, cy, CELL_OPEN)) {
						RLF_set_flag(m, cx, cy, CELL_SEEN);
					}
				}
				if ( x2 >= x0 && x2 <= x1 && y2 >= y0 && y2 <= y1 ) {
					offset2 = x2 + (y2 * map->width);
					if (offset2 < (unsigned)nc && !RLF_has_flag(m, cx, cy, CELL_OPEN)) {
						RLF_set_flag(m, cx, cy, CELL_SEEN);
					}
				}
			}
		}
	}
	return RLF_SUCCESS;
}
