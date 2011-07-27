/*
 * 	pathfind.c - simple path finder - public domain
	The most generic algorithm is something like:

    1. Start with empty priority queue P
    2. Start with empty set of previously visited nodes V
    3. Add start node s to P with cost f(s)
    4. If P is empty, then stop, no solution.
    5. Remove node x with lowest f(x) from P.
    6. If x is goal, stop with success.
    7. For each nx in successors of x:
        a) calculate f(nx) (= f(x) + delta)
        b) if nx not visited yet OR
              nx is in the priority queue P but with higher f(nx) OR
              nx has been visited but with higher f(nx)
           then place/update nx to P with new cost and
                update V to include (nx, f(nx), x)
    8. Go to 4

Then if
  f(x) equals to cost from start node, it is djikstra
  f(x) equals to cost from start node + estimate to goal, the algorithm is A*.

	Based on pathfinding functions found at
	->	http://roguebasin.roguelikedevelopment.org
*/
#include "headers/rlfl.h"
#include "headers/path.h"

static int dirx[]	={ 0,-1, 1, 0,-1, 1,-1, 1};
static int diry[]	={-1, 0, 0, 1,-1,-1, 1, 1};

/* Private functions */
static err init_path(unsigned int m, float dcost);
static err find_path(unsigned int m, unsigned int ox, unsigned int oy, unsigned int dx, unsigned int dy, bool fill);
static int path_map(unsigned int m, int x, int y);
static path_element * path_element_map(unsigned int m, int x, int y);
static void path_push_open(path_element * element, int dx, int dy );
static path_element * path_pop_open(void);
static int path_cost(path_element* element, int dx, int dy );
static void path_check(unsigned int m, path_element* parent, int ox, int oy, int dx, int dy);
static inline void path_update_cost( path_element* parent, path_element* pos);
static void path_remove(path_element* element);
static err store_path(unsigned int i, unsigned int m, unsigned int ox, unsigned int oy, unsigned int dx, unsigned int dy, bool valid);

/*
 +-----------------------------------------------------------+
 * @desc	DEBUG
 +-----------------------------------------------------------+
 */
void
RLFL_DEBUG_print_path_step(unsigned int m)
{
	printf(">>>>>>>>>>>\n");
	RLFL_map_t* map = RLFL_map_store[m];
	int i, j;
	for(i=0; i<map->width; i++) {
		for(j=0; j<map->height; j++) {
			path_element* elm = path_element_map(m, i, j);
			int c = elm->cost;
			if(c < -1)
			{
				printf(" ");
			}
			else if(c >= 0 && c < 10)
			{
				printf("%d", c);
			}
			else
			{
				if(c < 65) c = 55 + c;
				if(c > 125) c = 125;
				printf("%c", c);
			}
		}
		printf("\n");
	}
	printf(">>>>>>>>>>>\n");
}
/*
 +-----------------------------------------------------------+
 * @desc	Compute safety map
 +-----------------------------------------------------------+
 */
err
RLFL_path_fill_map(unsigned int m, unsigned int ox, unsigned int oy, float dcost)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	if(!(RLFL_cell_valid(m, ox, oy)))
		return RLFL_ERR_OUT_OF_BOUNDS;

	RLFL_map_t* map = RLFL_map_store[m];
	unsigned int pm;
	for(pm=0; pm<RLFL_MAX_PATHS; pm++){
		if(!map->path_map[pm]) break;
	}
	if(pm >= RLFL_MAX_PATHS)
		return RLFL_ERR_NO_PATH;

	if(pm >= RLFL_MAX_PATHS)
		return RLFL_ERR_FLAG;

	map->path_map[pm] = (int *) calloc(sizeof(int), map->cellcnt);
	if(!map->path_map[pm])
		return RLFL_ERR_GENERIC;

	/* prepare */
	init_path(m, dcost);

	/* plot */
	find_path(m, ox, oy, ox, oy, true);

	int i;
	for(i=0; i<map->cellcnt; i++)
	{
		unsigned int x, y;
		RLFL_translate_xy(m, i, &x, &y);
		path_element* elm = path_element_map(m, x, y);
		if(elm && elm->cost && RLFL_has_flag(m, x, y, (CELL_OPEN | CELL_WALK)))
		{
			map->path_map[pm][i] = elm->cost;
		}
		else
		{
			map->path_map[pm][i] = -1;
		}
	}

	/* Cleanup */
	delete_path();

//	RLFL_DEBUG_print_path_map(m, pm);

	return pm;
}
/*
 +-----------------------------------------------------------+
 * @desc	DEBUG
 +-----------------------------------------------------------+
 */
void
RLFL_DEBUG_print_path_map(unsigned int m, unsigned int p)
{
	if(!RLFL_map_valid(m))
		return;

	RLFL_map_t* map = RLFL_map_store[m];
	if(!RLFL_map_store[m]->path_map[p])
		return;

	printf(">>>>>>>>>>>\n");

	int i, j;
	for(i=0; i<map->width; i++) {
		for(j=0; j<map->height; j++) {
			int c =  map->path_map[p][(i + (j * map->width))];
			if(c == -1)
			{
				printf(" ");
			}
			else if(c >= 0 && c < 10)
			{
				printf("%d", c);
			}
			else
			{
				if(c < 65) c = 55 + c;
				if(c > 125) c = 125;
				printf("%c", c);
			}
		}
		printf("\n");
	}
	printf(">>>>>>>>>>>\n");
}
/*
 * One step on the path_map
 * */
err
RLFL_path_step_map(unsigned int m, unsigned int p, unsigned int ox, unsigned int oy,
				   unsigned int *x, unsigned int *y, bool away)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	if(!(RLFL_cell_valid(m, ox, oy)))
		return RLFL_ERR_OUT_OF_BOUNDS;

	if(p > RLFL_MAX_PATHS)
		return RLFL_ERR_NO_PATH;

	RLFL_map_t* map = RLFL_map_store[m];
	if(!RLFL_map_store[m]->path_map[p])
		return RLFL_ERR_NO_PATH;

	int i, xx, yy, j=-1;
	int n = away ? -1 : INT_MAX;
	for(i=0; i<8; i++)
	{
		xx = (ox + dirx[i]);
		yy = (oy + diry[i]);

		if(!RLFL_cell_valid(m, xx, yy))
			continue;
		int cc = map->path_map[p][(xx + (yy * map->width))];
		if(cc < 0)
			continue;

		if((away && (n < cc)) || (!away && (n > cc)))
		{
			n = cc;
			j = i;
		}
	}
	if(j < 0)
	{
		(*x) = (ox);
		(*y) = (oy);
		return RLFL_ERR_GENERIC;
	}

	/* Save */
	(*x) = (ox + dirx[j]);
	(*y) = (oy + diry[j]);

	/* OK */
	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Clear weight map
 +-----------------------------------------------------------+
 */
err
RLFL_path_wipe_map(unsigned int m, unsigned int p)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	RLFL_map_t *map = RLFL_map_store[m];
	if(map->path_map[p]) {
		free(map->path_map[p]);
		map->path_map[p] = NULL;
	}

	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Wipe all weight maps
 +-----------------------------------------------------------+
 */
err
RLFL_path_wipe_all_maps(unsigned int m)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	/* Wipe any path maps */
	int i;
	for(i=0; i<RLFL_MAX_PATHS; i++)
		RLFL_path_wipe_map(m, i);

	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Pathfinding
 +-----------------------------------------------------------+
 */
err
RLFL_path_astar(unsigned int m, unsigned int ox, unsigned int oy, unsigned int dx, unsigned int dy,
			   int range, unsigned int flags, float dcost)
{
	/* assert map */
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	if(!(RLFL_cell_valid(m, ox, oy) && RLFL_cell_valid(m, dx, dy)))
		return RLFL_ERR_OUT_OF_BOUNDS;

	unsigned int i;
	for(i=0; i<RLFL_MAX_PATHS; i++){
		if(!RLFL_path_store[i]) break;
	}
	/* assert path */
	if(i >= RLFL_MAX_PATHS)
		return RLFL_ERR_FLAG;

	/* prepare */
	init_path(m, dcost);

	/* assume valid path */
	bool valid = true;

	/* -1 is max range */
	if(range < 0) range = RLFL_MAX_RANGE;

	/* plot */
	err res = find_path(m, ox, oy, dx, dy, false);

	/* Store it */
	if(res == RLFL_SUCCESS) store_path(i, m, ox, oy, dx, dy, valid);

	/* Cleanup */
	delete_path();

	/* We have a path */
	return (res == RLFL_SUCCESS) ? i : RLFL_ERR_GENERIC;
}
/*
 +-----------------------------------------------------------+
 * @desc	Create a path in path_store
 +-----------------------------------------------------------+
 */
static err
store_path(unsigned int i, unsigned int m, unsigned int ox, unsigned int oy,
		   unsigned int dx, unsigned int dy, bool valid) {
	RLFL_path_t *path = (RLFL_path_t *)calloc(sizeof(RLFL_path_t), 1);
	if(path == NULL) return RLFL_ERR_GENERIC;
	path->path = RLFL_list_create();
	path->ox = ox;
	path->oy = oy;
	path->dx = dx;
	path->dy = dy;
	path->map = m;
	path->size = 0;
	path->valid = valid;
	RLFL_clear_map(m, CELL_PATH);

	/* Trace from destination to origin */
	path_element* pos = path_element_map(m, dx, dy);
	while(true) {
		RLFL_step_t *step = (RLFL_step_t *)calloc(sizeof(RLFL_step_t), 1);
		if(step == NULL) return RLFL_ERR_GENERIC;
		step->X = pos->x;
		step->Y = pos->y;
		RLFL_list_append(path->path, step);
		path->size++;
		RLFL_set_flag(m, step->X, step->Y, CELL_PATH);
		if(!pos->parent) break;
		pos = path_element_map(m, pos->parent->x, pos->parent->y);
	}

	/* Store it */
	RLFL_path_store[i] = path;
	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Create new path-finding structure
 +-----------------------------------------------------------+
 */
static err
init_path(unsigned int m, float dcost) {
	int i, j, k;
	if(!RLFL_map_store[m]) return RLFL_ERR_NO_MAP;
	RLFL_map_t *map = RLFL_map_store[m];

	/* PATH exists */
	if(PATH) return RLFL_ERR_FLAG;

	path_int_t *p;
   	p = (path_int_t*) calloc(sizeof(path_int_t), 1);
   	if(p == NULL) return RLFL_ERR_GENERIC;

	p->nodes = (path_element *)calloc(sizeof(path_element), map->width * map->height);
	if(p->nodes == NULL) return RLFL_ERR_GENERIC;
	p->open = (path_element **)calloc(sizeof(path_element*), map->width * map->height);
	if(p->open == NULL) return RLFL_ERR_GENERIC;

	memset(p->nodes, 0, (map->height * map->width) * sizeof( path_element ));
	memset(p->open, 0, (map->height * map->width) * sizeof( path_element* ));

	p->top = 0;
	p->dcost = dcost;
	p->astar = true;
	PATH = p;

	k = 0;
	for (j = 0; j < map->height; j++) {
		for (i = 0; i < map->width; i++) {
	     	PATH->nodes[k].x = i;
	     	PATH->nodes[k].y = j;
	      	k++;
	  	}
	}
	return RLFL_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Free resources
 +-----------------------------------------------------------+
 */
void
delete_path(void) {
	if(PATH) {
		free(PATH->nodes);
		free(PATH->open);
		free(PATH);
		PATH = NULL;
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	Plot path in map
 +-----------------------------------------------------------+
 */
static err
find_path(unsigned int m, unsigned int ox, unsigned int oy, unsigned int dx, unsigned int dy, bool fill)
{
	if(!PATH)
		return RLFL_ERR_NO_MAP;
	path_element* pos = path_element_map(m, ox, oy);
	path_element* goal = path_element_map(m, dx, dy);
	path_element* current = NULL;
	int dir;

	if (pos && goal) {
		/* Bootstrap */
		pos->state = STATE_EMPTY;
		path_push_open(pos, dx, dy);

		while(PATH->top)
		{
			 current = path_pop_open();

			 /* Are we there yet */
			 if((!fill) && current->x == dx && current->y == dy)
			 {
				 return RLFL_SUCCESS;
			 }

			 /* Current is closed. */
			 current->state = STATE_CLOSED;

			 /* Generate positions reachable from current position. */
			 for(dir=0; dir<8; dir++)
			 {
				path_check(m, current, current->x + dirx[dir], current->y + diry[dir], dx, dy);
			 }
		}
	}
	return RLFL_ERR_GENERIC;
}
/*
 +-----------------------------------------------------------+
 * @desc	Maps coordinates to one dimensional array indices
 +-----------------------------------------------------------+
 */
static int
path_map(unsigned int m, int x, int y) {
	RLFL_map_t *map = RLFL_map_store[m];
    return x + (map->width * y);
}
/*
 +-----------------------------------------------------------+
 * @desc	Returns pointer to path element structure or NULL
 * 			if invalid position
 +-----------------------------------------------------------+
 */
static path_element*
path_element_map(unsigned int m, int x, int y) {
    path_element* result = NULL;
    RLFL_map_t *map = RLFL_map_store[m];
    if ( ( x >= 0 && x < map->width ) &&
         ( y >= 0 && y < map->height ) ) {
        result = &PATH->nodes[path_map(m, x, y )];
	}
    return result;
}
/*
 +-----------------------------------------------------------+
 * @desc	Push onto open and reorder heap;
 +-----------------------------------------------------------+
 */
static void
path_push_open(path_element* element, int dx, int dy ) {
	PATH->open[PATH->top] = element;

	int i, ntotal, ctotal;
	for(i=PATH->top; i >= 1; i--)
	{
		path_element* current = PATH->open[i];
		path_element* next = PATH->open[i - 1];

		ntotal = path_cost(next, dx, dy);
		ctotal = path_cost(current, dx, dy);

		if (ntotal < ctotal)
		{
			PATH->open[i] = next;
			PATH->open[i - 1] = current;
		}
	}
	PATH->top++;
}
/*
 +-----------------------------------------------------------+
 * @desc	Pops one element from the open stack;
 +-----------------------------------------------------------+
 */
static path_element *
path_pop_open(void) {
	PATH->top--;
    path_element* result = PATH->open[PATH->top];
    PATH->open[PATH->top] = NULL;
    return result;
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static void
path_check(unsigned int m, path_element* parent, int ox, int oy, int dx, int dy) {

	path_element* pos = path_element_map(m, ox, oy);

	if (pos)
	{
		/* We can ignore blocked positions (consider that cost is infinite).*/
		if (RLFL_has_flag(m, ox, oy, (CELL_OPEN | CELL_WALK)))
		{
			if(pos->state == STATE_EMPTY)
			{
				/* If not processed yet, add to open set */
				pos->state = STATE_OPEN;
				pos->parent = parent;
				path_update_cost(parent, pos);
				path_push_open(pos, dx, dy);
			}
			else
			{
				/* Now element is either in open or closed set. */
				const int oc = path_cost(pos, dx, dy);
				path_element tmp = *pos;
				path_update_cost(parent, &tmp);

//				printf("oc: %d, pc: %d\n", oc, path_cost(&tmp, dx, dy));
				if(oc > path_cost(&tmp, dx, dy))
				{
					/* New path is better than old. */
					bool was_open = (pos->state == STATE_OPEN);

					*pos = tmp;
					pos->parent = parent;
					pos->state = STATE_OPEN;
					if(was_open) {
						path_remove(pos);
					}
					path_push_open(pos, dx, dy);
				}
			}
		}
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	Updates cost to pos which is adjacent to parent
 +-----------------------------------------------------------+
 */
static inline void
path_update_cost( path_element* parent, path_element* pos) {
	pos->cost = parent->cost + 1;
}
/*
 +-----------------------------------------------------------+
 * @desc	Removes element from stack
 +-----------------------------------------------------------+
 */
static void
path_remove(path_element* element) {
    int i;
    for (i = 0; i < PATH->top; i++) {
        if(element == PATH->open[i]) {
            break;
      	}
   	}

    if (i != PATH->top) {
        /* Element was found. */
    	PATH->open[i] = NULL;
        for(; i < (PATH->top - 1); i++) {
        	PATH->open[i] = PATH->open[i + 1];
       	}
        PATH->top--;
 	}
}
/*
 +-----------------------------------------------------------+
 * @desc	Calculate move cost
 +-----------------------------------------------------------+
 */
static int
path_cost(path_element* element, int dx, int dy ) {
	int ox = element->x - dx;
	int oy = element->y - dy;
	int cost = 0;
	if (!element->estimate)
	{
		element->estimate = RLFL_distance(element->x, element->y, dx, dy);
	}
	if(ox && oy)
	{
		// Diagonal move
		cost += PATH->dcost;
	}
	return (element->cost + element->estimate + cost);
}
