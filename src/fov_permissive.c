/*
 +-----------------------------------------------------------+
 * @desc	Permissive FOV algorithm
 * @file	fov_permissive.c
 * @package RLFL
 * @license GPL
 * @from	libtcod - http://doryen.eptalys.net/libtcod/
 * <jtm@robot.is>
 +-----------------------------------------------------------+
 */

#include "headers/rlfl.h"

#define RELATIVE_SLOPE(l,x,y) (((l)->yf-(l)->yi)*((l)->xf-(x)) - ((l)->xf-(l)->xi)*((l)->yf-(y)))
#define BELOW(l,x,y) (RELATIVE_SLOPE(l,x,y) > 0)
#define BELOW_OR_COLINEAR(l,x,y) (RELATIVE_SLOPE(l,x,y) >= 0)
#define ABOVE(l,x,y) (RELATIVE_SLOPE(l,x,y) < 0)
#define ABOVE_OR_COLINEAR(l,x,y) (RELATIVE_SLOPE(l,x,y) <= 0)
#define COLINEAR(l,x,y) (RELATIVE_SLOPE(l,x,y) == 0)
#define LINE_COLINEAR(l,l2) (COLINEAR(l,(l2)->xi,(l2)->yi) && COLINEAR(l,(l2)->xf,(l2)->yf))

typedef struct {
	int xi,yi,xf,yf;
} line_t;

typedef struct _viewbump_t {
	int x,y;
	int refcount;
	struct _viewbump_t *parent;
} viewbump_t;

typedef struct {
	line_t shallow_line;
	line_t steep_line;
	viewbump_t *shallow_bump;
	viewbump_t *steep_bump;
} view_t;

static view_t **current_view=NULL;
static view_t *views=NULL;
static viewbump_t *bumps=NULL;
static int bumpidx=0;

static void add_shallow_bump(int x, int y, view_t *view);
static void add_steep_bump(int x, int y, view_t *view);
static bool check_view(RLFL_list_t active_views, view_t **it);
static void check_quadrant(RLFL_map_t *m,int startX,int startY,int dx, int dy, int extentX,int extentY,
						   bool light_walls);
static void visit_coords(RLFL_map_t *m,int startX, int startY, int x, int y, int dx, int dy,
						 RLFL_list_t active_views, bool light_walls);
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
err
RLFL_fov_permissive(unsigned int m, unsigned int ox, unsigned int oy, unsigned int radius, bool light_walls)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	if(!RLFL_cell_valid(m, ox, oy))
		return RLFL_ERR_OUT_OF_BOUNDS;

	if(radius >= RLFL_MAX_RADIUS)
		return RLFL_ERR_GENERIC;

	RLFL_map_t *map = RLFL_map_store[m];
	int minx, maxx, miny, maxy;

	/* The origin is always seen */
	RLFL_set_flag(m, ox, oy, CELL_FOV);

	/* preallocate views and bumps */
	views = (view_t *)calloc(sizeof(view_t), map->width*map->height);
	bumps = (viewbump_t *)calloc(sizeof(viewbump_t), map->width*map->height);

	/* set the fov range */
	if (radius > 0)
	{
		minx = MIN(ox, radius);
		maxx = MIN(map->width-ox - 1, radius);
		miny = MIN(oy, radius);
		maxy = MIN(map->height-oy - 1, radius);
	}
	else
	{
		minx = ox;
		maxx = map->width - ox - 1;
		miny = oy;
		maxy = map->height - oy -1;
	}

	/* calculate fov. precise permissive field of view */
	bumpidx = 0;
	check_quadrant(map, ox, oy, 1, 1, maxx, maxy, light_walls);
	bumpidx = 0;
	check_quadrant(map, ox, oy, 1, -1, maxx, miny, light_walls);
	bumpidx = 0;
	check_quadrant(map, ox, oy, -1, -1, minx, miny, light_walls);
	bumpidx = 0;
	check_quadrant(map, ox, oy, -1, 1, minx, maxy, light_walls);

	/* cleanup */
	free(bumps);
	free(views);

	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static void
add_shallow_bump(int x, int y, view_t *view) {
	viewbump_t *shallow, *curbump;
	view->shallow_line.xf = x;
	view->shallow_line.yf = y;
	shallow= &bumps[bumpidx++];
	shallow->x=x;
	shallow->y=y;
	shallow->parent=view->shallow_bump;
	view->shallow_bump=shallow;
	curbump=view->steep_bump;
	while(curbump)
	{
		if ( ABOVE(&view->shallow_line,curbump->x,curbump->y))
		{
			view->shallow_line.xi=curbump->x;
			view->shallow_line.yi=curbump->y;
		}
		curbump=curbump->parent;
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static void
add_steep_bump(int x, int y, view_t *view)
{
	viewbump_t *steep, *curbump;
	view->steep_line.xf=x;
	view->steep_line.yf=y;
	steep=&bumps[bumpidx++];
	steep->x=x;
	steep->y=y;
	steep->parent=view->steep_bump;
	view->steep_bump=steep;
	curbump=view->shallow_bump;
	while ( curbump )
	{
		if ( BELOW(&view->steep_line,curbump->x,curbump->y))
		{
			view->steep_line.xi=curbump->x;
			view->steep_line.yi=curbump->y;
		}
		curbump=curbump->parent;
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static bool
check_view(RLFL_list_t active_views, view_t **it)
{
	view_t *view = *it;
	line_t *shallow_line = &view->shallow_line;
	line_t *steep_line = &view->steep_line;
	if (LINE_COLINEAR(shallow_line, steep_line)
		&& (COLINEAR(shallow_line, 0, 1) || COLINEAR(shallow_line, 1, 0)) ){
		// slow !
		RLFL_list_remove_iterator(active_views, (void **)it);
		return false;
	}
	return true;
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static void
visit_coords(RLFL_map_t *m,int startX, int startY, int x, int y, int dx, int dy,
			 RLFL_list_t active_views, bool light_walls)
{
	// top left
	int tlx = x, tly = (y+1);

	// bottom right
	int brx = (x+1), bry = y;

	int realX = (x*dx), realY = (y*dy);
	int offset;
	view_t *view = NULL;

	while (current_view != (view_t **)RLFL_list_end(active_views))
	{
		view = *current_view;
		if ( !BELOW_OR_COLINEAR(&view->steep_line, brx, bry) ) {
			break;
		}
		current_view++;
	}
	if(current_view == (view_t **)RLFL_list_end(active_views)
			|| ABOVE_OR_COLINEAR(&view->shallow_line, tlx, tly)) {
		return;
	}

	offset = (startX + realX + ((startY+realY) * m->width));
	if (light_walls || RLFL_has_flag(m->mnum, startX+realX, startY+realY, CELL_OPEN)) {
		RLFL_set_flag(m->mnum, startX+realX, startY+realY, CELL_FOV);
	}

	if (RLFL_has_flag(m->mnum, startX+realX, startY+realY, CELL_OPEN))
		return;

	if ( ABOVE(&view->shallow_line, brx, bry)
		&& BELOW(&view->steep_line, tlx, tly)) {
		// slow !
		RLFL_list_remove_iterator(active_views, (void **)current_view);
	}
	else if( ABOVE(&view->shallow_line, brx, bry))
	{
		add_shallow_bump(tlx, tly, view);
		check_view(active_views, current_view);
	}
	else if(BELOW(&view->steep_line, tlx, tly))
	{
		add_steep_bump(brx,bry,view);
		check_view(active_views,current_view);
	}
	else
	{
		view_t *shallower_view = &views[offset];
		int view_index = (current_view - (view_t **)RLFL_list_begin(active_views));
		view_t **shallower_view_it;
		view_t **steeper_view_it;
		*shallower_view = **current_view;
		// slow !
		shallower_view_it = (view_t **)RLFL_list_insert(active_views, shallower_view, view_index);
		steeper_view_it = shallower_view_it+1;
		current_view = shallower_view_it;
		add_steep_bump(brx, bry, shallower_view);
		if (!check_view(active_views,shallower_view_it)) {
			steeper_view_it--;
		}
		add_shallow_bump(tlx, tly, *steeper_view_it);
		check_view(active_views, steeper_view_it);
		if ( view_index > RLFL_list_size(active_views)) {
			current_view = (view_t **)RLFL_list_end(active_views);
		}
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static void
check_quadrant(RLFL_map_t *m, int startX, int startY, int dx, int dy, int extentX,
			   int extentY, bool light_walls)
{
	RLFL_list_t active_views = RLFL_list_create();
	line_t shallow_line = { 0, 1, extentX, 0 };
	line_t steep_line 	= { 1, 0, 0, extentY };

	int maxI = (extentX + extentY);
	int i = 1;

	view_t *view= &views[(startX + (startY * m->width))];
	view->shallow_line	= shallow_line;
	view->steep_line	= steep_line;
	view->shallow_bump	= NULL;
	view->steep_bump	= NULL;

	RLFL_list_append(active_views, view);
	current_view = (view_t **)RLFL_list_begin(active_views);

	while ( (i != maxI + 1) && RLFL_list_size(active_views))
	{
		int startJ	= MAX(i-extentX, 0);
		int maxJ 	= MIN(i, extentY);
		int j 		= startJ;
		while ( (j != maxJ + 1)
				&& RLFL_list_size(active_views)
				&& (current_view != (view_t **)RLFL_list_end(active_views)))
		{
			int x = (i - j);
			int y = j;
			visit_coords(m, startX, startY, x, y, dx, dy, active_views, light_walls);
			j++;
		}
		i++;
		current_view=(view_t **)RLFL_list_begin(active_views);
	}
}
