/*
 +-----------------------------------------------------------+
 * @desc	Diamond raycasting
 * @file	fov_diamond_raycasting.c
 * @package RLF
 * @license GPL
 * @from	libtcod - http://doryen.eptalys.net/libtcod/
 * <jtm@robot.is>
 +-----------------------------------------------------------+
 */
#include "headers/rlfl.h"

#define IS_OBSCURE(r) ((r->xerr > 0 && r->xerr <= r->xob) || (r->yerr > 0 && r->yerr <= r->yob) )

typedef struct _ray_data_t {
	int xloc, yloc; // position
	int xob, yob; // obscurity vector
	int xerr, yerr; // bresenham error
	struct _ray_data_t * xinput, * yinput; // offset of input rays
	bool added; // already in the fov
	bool ignore; // non visible. don't bother processing it
} ray_data_t;

// variables
static int origx, origy; // fov origin
static ray_data_t **raymap; // result rays
static ray_data_t *raymap2; // temporary rays
static int perimidx;

// functions
static ray_data_t *new_ray(RLFL_map_t *m,int x, int y);
static void processRay(RLFL_map_t *m, RLFL_list_t perim, ray_data_t *new_ray, ray_data_t *input_ray);
static void process_x_input(ray_data_t *new_ray, ray_data_t *xinput);
static void process_y_input(ray_data_t *new_ray, ray_data_t *yinput);
static void merge_input(RLFL_map_t *m, ray_data_t *r);
static void expandPerimeterFrom(RLFL_map_t *m,RLFL_list_t perim,ray_data_t *r);
/*
 +-----------------------------------------------------------+
 * @desc	Diamond raycasting
 +-----------------------------------------------------------+
 */
err
RLFL_fov_diamond_raycasting(unsigned int m, unsigned int ox, unsigned int oy,
						   unsigned int radius, bool light_walls)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	RLFL_map_t *map = RLFL_map_store[m];
	ray_data_t **rd;
	int nbcells = map->width*map->height;
	RLFL_list_t perim = RLFL_list_create_size(nbcells);
	int r2 = radius * radius;

	perimidx = 0;
	raymap = (ray_data_t **)calloc(sizeof(ray_data_t*), nbcells);
	raymap2 = (ray_data_t *)calloc(sizeof(ray_data_t), nbcells);
	origx = ox;
	origy = oy;

	expandPerimeterFrom(map, perim, new_ray(map, 0, 0));
	while(perimidx < RLFL_list_size(perim))
	{
		ray_data_t *ray = (ray_data_t *)RLFL_list_get(perim, perimidx);
		int distance = 0;
		if (r2 > 0)
		{
			distance = ((ray->xloc * ray->xloc) + (ray->yloc * ray->yloc));
		}

		perimidx++;

		if (distance <= r2)
		{
			merge_input(map, ray);
			if (!ray->ignore)
			{
				expandPerimeterFrom(map, perim, ray);
			}
		} else ray->ignore=true;
	}

	// set fov data
	rd = raymap;
	int c = nbcells;
	while(c)
	{
		if ( *rd == NULL || (*rd)->ignore
			|| ((*rd)->xerr > 0 && (*rd)->xerr <= (*rd)->xob )
			|| ((*rd)->yerr > 0 && (*rd)->yerr <= (*rd)->yob ))
		{
			// pass
		}
		else
		{
			map->cells[(nbcells - c)] |= CELL_FOV;
		}
		c--;
		rd++;
	}

	// Origin always seen
	RLFL_set_flag(m, origx, origy, CELL_FOV);

	// light walls
	if (light_walls) {
		int xmin=0, ymin=0, xmax=map->width, ymax=map->height;
		RLFL_fov_finish(m, xmin, ymin, ox, oy, -1, -1);
		RLFL_fov_finish(m, ox, ymin, xmax-1, oy, 1, -1);
		RLFL_fov_finish(m, xmin, oy, ox, ymax-1, -1, 1);
		RLFL_fov_finish(m, ox, oy, xmax-1, ymax-1, 1, 1);
	}

	free(raymap);
	free(raymap2);
	RLFL_list_delete(perim);
	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static ray_data_t *
new_ray(RLFL_map_t *m, int x, int y)
{
    ray_data_t *r;
	if ((unsigned) (x+origx) >= (unsigned)m->width)
		return NULL;
	if ((unsigned) (y+origy) >= (unsigned)m->height)
		return NULL;
	r = &raymap2[x + origx + ((y+origy) * m->width)];
	r->xloc = x;
	r->yloc = y;
	return r;
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static void
processRay(RLFL_map_t *m, RLFL_list_t perim, ray_data_t *new_ray, ray_data_t *input_ray)
{
	if(new_ray)
	{
		int mapx = origx + new_ray->xloc;
		int mapy = origy + new_ray->yloc;
		int newrayidx;
		newrayidx = mapx + (mapy * m->width);
		if (new_ray->yloc == input_ray->yloc)
		{
			new_ray->xinput = input_ray;
		}
		else
		{
			new_ray->yinput = input_ray;
		}
		if (!new_ray->added)
		{
			RLFL_list_append(perim, new_ray);
			new_ray->added = true;
			raymap[newrayidx] = new_ray;
		}
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static void
process_x_input(ray_data_t *new_ray, ray_data_t *xinput)
{
	if (xinput->xob == 0 && xinput->yob == 0)
		return;
	if (xinput->xerr > 0 && new_ray->xob == 0)
	{
		new_ray->xerr = xinput->xerr - xinput->yob;
		new_ray->yerr = xinput->yerr + xinput->yob;
		new_ray->xob = xinput->xob;
		new_ray->yob = xinput->yob;
	}
	if (xinput->yerr <= 0 && xinput->yob > 0 && xinput->xerr > 0)
	{
		new_ray->yerr = xinput->yerr + xinput->yob;
		new_ray->xerr = xinput->xerr - xinput->yob;
		new_ray->xob = xinput->xob;
		new_ray->yob = xinput->yob;
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 * @param	FIXME
 * @return 	FIXME
 +-----------------------------------------------------------+
 */
static void
process_y_input(ray_data_t *new_ray, ray_data_t *yinput)
{
	if (yinput->xob == 0 && yinput->yob == 0)
		return;
	if (yinput->yerr > 0 && new_ray->yob == 0)
	{
		new_ray->yerr = yinput->yerr - yinput->xob;
		new_ray->xerr = yinput->xerr + yinput->xob;
		new_ray->xob = yinput->xob;
		new_ray->yob = yinput->yob;
	}
	if (yinput->xerr <= 0 && yinput->xob > 0 && yinput->yerr > 0)
	{
		new_ray->yerr = yinput->yerr - yinput->xob;
		new_ray->xerr = yinput->xerr + yinput->xob;
		new_ray->xob = yinput->xob;
		new_ray->yob = yinput->yob;
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static void
merge_input(RLFL_map_t *m, ray_data_t *r)
{
	ray_data_t *xi = r->xinput;
	ray_data_t *yi = r->yinput;
	if ( xi ) process_x_input(r, xi);
	if ( yi ) process_y_input(r, yi);
	if ( ! xi )
	{
		if ( IS_OBSCURE(yi) )
			r->ignore=true;
	}
	else if ( ! yi )
	{
		if ( IS_OBSCURE(xi) )
			r->ignore=true;
	}
	else if ( IS_OBSCURE(xi) && IS_OBSCURE(yi) )
	{
		r->ignore=true;
	}
	if (! r->ignore && !RLFL_has_flag(m->mnum, r->xloc+origx, r->yloc+origy, CELL_OPEN)) {
		r->xerr = r->xob = ABS(r->xloc);
		r->yerr = r->yob = ABS(r->yloc);
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static void
expandPerimeterFrom(RLFL_map_t *m,RLFL_list_t perim,ray_data_t *r) {
	if ( r->xloc >= 0 )
	{
		processRay(m, perim, new_ray(m, r->xloc+1, r->yloc), r);
	}
	if ( r->xloc <= 0 )
	{
		processRay(m, perim, new_ray(m, r->xloc-1, r->yloc), r);
	}
	if ( r->yloc >= 0 )
	{
		processRay(m, perim, new_ray(m, r->xloc, r->yloc+1), r);
	}
	if ( r->yloc <= 0 )
	{
		processRay(m, perim, new_ray(m, r->xloc, r->yloc-1), r);
	}
}
