/*
 +-----------------------------------------------------------+
 * @desc	Basic FOV algorithm
 * @file	fov_circular_raycasting.c
 * @package RLFL
 * <jtm@robot.is>
 +-----------------------------------------------------------+
 */
#include "headers/rlfl.h"

#define CELL_RADIUS 0.4f
#define RAY_RADIUS 0.2f

typedef struct {
	int stepx;
	int stepy;
	int e;
	int deltax;
	int deltay;
	int origx;
	int origy;
	int destx;
	int desty;
} RLFL_bresenham_data_t;
static RLFL_bresenham_data_t bresenham_data;

// Private
static void cast_ray(unsigned int m, int xo, int yo, int xd, int yd, int r2, bool light_walls);
static void RLFL_line_init(int xFrom, int yFrom, int xTo, int yTo);
static bool RLFL_line_step(int *xCur, int *yCur);
/*
 +-----------------------------------------------------------+
 * @desc	Circular ray casting
 +-----------------------------------------------------------+
 */
err
RLFL_fov_circular_raycasting(unsigned int m, unsigned int ox, unsigned int oy,
							unsigned int radius, bool light_walls)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	if(!RLFL_cell_valid(m, ox, oy))
		return RLFL_ERR_OUT_OF_BOUNDS;

	if(radius >= RLFL_MAX_RADIUS)
		return RLFL_ERR_GENERIC;

	int xo, yo;
	RLFL_map_t *map = RLFL_map_store[m];
	int xmin = 0, ymin = 0;
	int xmax = map->width, ymax = map->height;
	int r2 = radius * radius;
	if(radius > 0)
	{
		xmin = MAX(0, ox - radius);
		ymin = MAX(0, oy - radius);
		xmax = MIN(map->width, ox + radius + 1);
		ymax = MIN(map->height, oy + radius + 1);
	}
	xo = xmin;
	yo = ymin;
	while(xo < xmax)
	{
		cast_ray(m, ox, oy, xo++, yo, r2, light_walls);
	}
	xo = xmax - 1;
	yo = ymin + 1;
	while(yo < ymax)
	{
		cast_ray(m, ox, oy, xo, yo++, r2, light_walls);
	}
	xo = xmax-2;
	yo = ymax-1;
	while ( xo >= 0 )
	{
		cast_ray(m, ox, oy, xo--, yo, r2, light_walls);
	}
	xo = xmin;
	yo = ymax - 2;
	while (yo > 0)
	{
		cast_ray(m, ox, oy, xo, yo--, r2, light_walls);
	}
	if(light_walls)
	{
		RLFL_fov_finish(m, xmin, ymin, ox, oy, -1, -1);
		RLFL_fov_finish(m, ox, ymin, xmax - 1, oy, 1, -1);
		RLFL_fov_finish(m, xmin, oy, ox, ymax - 1, -1, 1);
		RLFL_fov_finish(m, ox, oy, xmax - 1, ymax - 1, 1, 1);
	}

	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Cast ray
 +-----------------------------------------------------------+
 */
static void
cast_ray(unsigned int m, int xo, int yo, int xd, int yd, int r2, bool light_walls)
{
	RLFL_map_t *map = RLFL_map_store[m];
	int curx = xo, cury = yo;
	bool in = false;
	bool blocked = false;
	bool end = false;
	RLFL_line_init(xo, yo, xd, yd);
	int nc = (map->width * map->height);
	int offset = curx + (cury * map->width);
	if (0 <= offset && offset < nc)
	{
		in = true;
		RLFL_set_flag(m, curx, cury, CELL_FOV);
	}
	while(!end)
	{
		end = RLFL_line_step(&curx, &cury);	// reached xd,yd
		offset = curx + (cury*map->width);
		if (r2 > 0)
		{
			// check radius
			int cur_radius = (curx-xo) * (curx-xo) + (cury-yo) * (cury-yo);
			if (cur_radius > r2)
				return;
		}
		if (0 <= offset && offset < nc)
		{
			in = true;
			if (!blocked && !RLFL_has_flag(m, curx, cury, CELL_OPEN))
			{
				blocked = true;
			}
			else if (blocked)
			{
				return; // wall
			}
			if (light_walls || !blocked)
			{
				RLFL_set_flag(m, curx, cury, CELL_FOV);
			}
		}
		else if (in)
		{
			// ray out of map
			return;
		}
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static void
RLFL_line_init(int xFrom, int yFrom, int xTo, int yTo)
{
	RLFL_bresenham_data_t *data = &bresenham_data;
	if(!data) return;
	data->origx = xFrom;
	data->origy = yFrom;
	data->destx = xTo;
	data->desty = yTo;
	data->deltax = xTo - xFrom;
	data->deltay = yTo - yFrom;
	if (data->deltax > 0)
	{
		data->stepx = 1;
	}
	else if (data->deltax < 0)
	{
		data->stepx = -1;
	}
	else
	{
		data->stepx = 0;
	}
	if (data->deltay > 0)
	{
		data->stepy = 1;
	}
	else if ( data->deltay < 0 )
	{
		data->stepy = -1;
	}
	else
	{
		data->stepy = 0;
	}
	if (data->stepx*data->deltax > data->stepy*data->deltay)
	{
		data->e = data->stepx * data->deltax;
		data->deltax *= 2;
		data->deltay *= 2;
	}
	else
	{
		data->e = data->stepy * data->deltay;
		data->deltax *= 2;
		data->deltay *= 2;
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 * @param	FIXME
 * @return 	FIXME
 +-----------------------------------------------------------+
 */
static bool
RLFL_line_step(int *xCur, int *yCur)
{
	RLFL_bresenham_data_t *data = &bresenham_data;
	if(!data || !xCur || !yCur)  return false;
	if((data->stepx * data->deltax) > (data->stepy * data->deltay))
	{
		if(data->origx == data->destx)
			return true;
		data->origx += data->stepx;
		data->e -= (data->stepy * data->deltay);
		if (data->e < 0)
		{
			data->origy += data->stepy;
			data->e += (data->stepx * data->deltax);
		}
	}
	else
	{
		if(data->origy == data->desty)
			return true;
		data->origy += data->stepy;
		data->e -= (data->stepx * data->deltax);
		if ( data->e < 0)
		{
			data->origx += data->stepx;
			data->e += (data->stepy * data->deltay);
		}
	}
	if(xCur) *xCur = data->origx;
	if(yCur) *yCur = data->origy;
	return false;
}

