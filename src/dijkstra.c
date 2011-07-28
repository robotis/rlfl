/*
	RLFL dijkstra maps.

	Improvised from and inspired by code found in
	Brogue <http://sites.google.com/site/broguegame/>
	and various resources about Dijkstra's algorithm.

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
#include "headers/dijkstra.h"

const short nbDirs[8][2] = {{0,-1},{0,1},{-1,0},{1,0},{-1,-1},{-1,1},{1,-1},{1,1}};

static err dijkstra_scan(RLFL_dijkstra_map* map);
static RLFL_dijkstra_map* init_dijkstra_map(unsigned int m, float dcost);
static err add_goal_point(RLFL_dijkstra_map* map, unsigned int x, unsigned int y);
//static err add_cost_point(RLFL_dijkstra_map* map, unsigned int x, unsigned int y);
static void free_dijkstra_map(RLFL_dijkstra_map* map);
//static void DEBUG_print_map(RLFL_dijkstra_map* map);
static err save_dijkstra_map(unsigned int m, unsigned int p, RLFL_dijkstra_map* dmap);

/*
 +-----------------------------------------------------------+
 * @desc	Compute safety map
 +-----------------------------------------------------------+
 */
err
RLFL_path_fill_map(unsigned int m, unsigned int ox, unsigned int oy, float dcost, bool safety)
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
	RLFL_dijkstra_map* dmap = init_dijkstra_map(m, dcost);

	/* Add the origin as a goal */
	add_goal_point(dmap, ox, oy);

//	DEBUG_print_map(dmap);

	/* plot */
	dijkstra_scan(dmap);

//	DEBUG_print_map(dmap);

	if(safety)
	{
		int i;
		for(i=0; i<map->cellcnt; i++) {
			if(dmap->links[i].state == PATH_IMPASSIBLE)
				continue;

			dmap->links[i].distance = ((dmap->links[i].distance) / (50.0 + dmap->links[i].distance));
			dmap->links[i].distance *= 50.0;
			dmap->links[i].distance *= -3.5;
		}

		/* re-plot */
		dijkstra_scan(dmap);

//		DEBUG_print_map(dmap);
	}

	/* Save the map */
	save_dijkstra_map(m, pm, dmap);

	/* Cleanup */
	free_dijkstra_map(dmap);

	return pm;
}
err
RLFL_path_fill_map_custom(unsigned int m, unsigned long flags, float dcost)
{
	return RLFL_SUCCESS;
}
/*
 *
 * */
err
save_dijkstra_map(unsigned int m, unsigned int p, RLFL_dijkstra_map* dmap)
{
	if(!RLFL_map_valid(m) || !RLFL_map_store[m]->path_map[p])
		return RLFL_ERR_NO_MAP;

	RLFL_map_t* map = RLFL_map_store[m];
	int i;
	for(i=0; i<map->cellcnt; i++)
	{
		if(dmap->links[i].state == PATH_IMPASSIBLE)
			map->path_map[p][i] = PATH_UNKNOWN;
		else
			map->path_map[p][i] = (unsigned int)dmap->links[i].distance;
	}

	return RLFL_SUCCESS;
}
/*
 * One step on the path_map
 * */
err
RLFL_path_step_map(unsigned int m, unsigned int p, unsigned int ox, unsigned int oy,
				   unsigned int *x, unsigned int *y)
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
	int n = INT_MAX;
	for(i=0; i<8; i++)
	{
		xx = (ox + nbDirs[i][0]);
		yy = (oy + nbDirs[i][1]);

		if(!RLFL_cell_valid(m, xx, yy))
			continue;

		int cc = map->path_map[p][(xx + (yy * map->width))];

		if(cc == PATH_UNKNOWN) continue;

		if(n > cc)
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
	(*x) = (ox + nbDirs[j][0]);
	(*y) = (oy + nbDirs[j][1]);

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
/**
 *
 *
 */
err
dijkstra_scan(RLFL_dijkstra_map* map)
{
	int x, y;

	link *left = NULL;
	link *right = NULL;
	link *current = NULL;

	for(x=0; x<map->w; x++)
		for(y=0; y<map->h; y++)
	{
		current = &map->links[(x + (y * map->w))];

		if(current->state != PATH_IMPASSIBLE)
		{
			if(current->distance < MAX_DISTANCE)
			{
				if (right == NULL || right->distance > current->distance)
				{
					// left and right are used to traverse the list; if many cells have similar values,
					// some time can be saved by not clearing them with each insertion.  this time,
					// sadly, we have to start from the front.

					left = &map->front;
					right = map->front.right;
				}

				while (right != NULL && right->distance < current->distance)
				{
					left = right;
					right = right->right;
				}

				current->right = right;
				current->left = left;
				left->right = current;
				if (right != NULL) right->left = current;

				left = current;
			} else {
				current->right = NULL;
				current->left = NULL;
			}
		} else {
			current->right = NULL;
			current->left = NULL;
		}
	}

	/* Update */
	short dir;

	current = NULL, right = NULL, left = NULL;
	link *head = map->front.right;
	map->front.right = NULL;
	bool diagonal;

	while (head != NULL)
	{
		for (dir = 0; dir < 8; dir++)
		{
			diagonal = (dir >= 4);
			current = head + (nbDirs[dir][0] + ((int)map->w * nbDirs[dir][1]));

			// verify passability
			if (current->state == PATH_IMPASSIBLE)
				continue;

			if (diagonal)
			{
				// Diagonal move
				link *way1, *way2;
				way1 = head + nbDirs[dir][0];
				way2 = head + ((int)map->w * nbDirs[dir][1]);

				if (way1->state == PATH_IMPASSIBLE || way2->state == PATH_IMPASSIBLE)
					continue;
			}

			if (head->distance + current->cost < current->distance)
			{
				// update distance
				current->distance = diagonal
					? (head->distance + (current->cost + map->dcost))
					: (head->distance + current->cost);

				// reinsert the touched cell; it'll be close to the beginning of the list now, so
				// this will be very fast.  start by removing it.

				if (current->right != NULL) current->right->left = current->left;
				if (current->left != NULL) current->left->right = current->right;

				left = head;
				right = head->right;
				while (right != NULL && right->distance < current->distance)
				{
					left = right;
					right = right->right;
				}
				if (left != NULL) left->right = current;
				current->right = right;
				current->left = left;
				if (right != NULL) right->left = current;
			}
		}

		right = head->right;

		head->left = NULL;
		head->right = NULL;

		head = right;
	}

	return RLFL_SUCCESS;
}
/**
 *
 *
 */
void
DEBUG_print_map(RLFL_dijkstra_map* map)
{
	int x, y;
	for (x=0; x<map->w; x++) {
		for (y=0; y<map->h; y++) {
			link link = map->links[x + (y * map->w)];
			if(link.state != PATH_IMPASSIBLE)
			{
				int c = (int)link.distance;
				if(c < 0) c *= -1;

				if(c >= 0 && c < 10)
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
			else
				printf(" ");
		}
		printf("\n");
	}
}
/**
 *
 *
 */
RLFL_dijkstra_map*
init_dijkstra_map(unsigned int m, float dcost)
{
	RLFL_map_t* map = RLFL_map_store[m];

	RLFL_dijkstra_map* dmap = (RLFL_dijkstra_map *)calloc(sizeof(RLFL_dijkstra_map), 1);
	dmap->links = (link *)calloc(sizeof(link), (map->width * map->height));

	int x, y;
	for (x = 0; x < map->width; x++)
	{
		for (y = 0; y < map->height; y++)
		{
			unsigned int k = (x + (y * map->width));

			dmap->links[k].x = x;
			dmap->links[k].y = y;

			if(RLFL_has_flag(m, x, y, (CELL_OPEN|CELL_WALK)))
			{
				// Terrain cost
				dmap->links[k].cost = 1;
				dmap->links[k].distance = PATH_UNKNOWN;
			}
			else
			{
				dmap->links[k].state = PATH_IMPASSIBLE;
			}
		}
	}

	dmap->dcost = dcost;
	dmap->w = map->width;
	dmap->h = map->height;

	return dmap;
}
/**
 *
 *
 */
err
add_goal_point(RLFL_dijkstra_map* map, unsigned int x, unsigned int y)
{
	map->links[x + (y * map->w)].distance = 0;

	return RLFL_SUCCESS;
}
/**
 *
 *
 */
void
free_dijkstra_map(RLFL_dijkstra_map* map)
{
	if(map)
	{
		free(map->links);
		free(map);
	}
}
