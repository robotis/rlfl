/*
	RLFL projection.

	Mostly from hengband:
	<http://hengband.sourceforge.jp/en/>

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

static err add_step(int p, int x, int y);
static void breath_shape(unsigned short m, unsigned short path_n, int dist, int *pgrids,
						 unsigned short *gm, int *pgm_rad, int rad, int y1, int x1, int y2,
						 int x2, bool disint_ball, bool real_breath);
static err ball_shape(unsigned short m, unsigned short project_n, int dist, int bx, int by, int rad, unsigned short flg);
/*
 *
 * */
err
RLFL_project(unsigned int m, unsigned int ox, unsigned int oy, unsigned int tx, unsigned int ty,
			 int rad, int range, unsigned short flg)
{
//	printf("(%d, %d)(%d, %d), %d, %d, %d\n", ox, oy, tx, ty, rad, range, flg);
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	int i = 0, dist, project_n, path_size;
	for(project_n=0; project_n<RLFL_MAX_PROJECTS; project_n++){
		if(!RLFL_project_store[project_n]) break;
	}
	/* assert path */
	if(project_n >= RLFL_MAX_PROJECTS)
		return RLFL_ERR_FLAG;

	/* assert cells */
	if(!RLFL_cell_valid(m, ox, oy)) return RLFL_ERR_OUT_OF_BOUNDS;
	if(!RLFL_cell_valid(m, tx, ty)) return RLFL_ERR_OUT_OF_BOUNDS;

	/* prepare */
	RLFL_list_t * projection = RLFL_list_create();
	if(projection == NULL) return RLFL_ERR_FLAG;
	RLFL_project_store[project_n] = projection;

	if(range < 0) range = RLFL_MAX_RANGE;

	int y1, x1, y, x;
	int y2, x2;
	int by, bx;

	int dist_hack = 0;

	int y_saver, x_saver; /* For reflecting monsters */

	/* Assume to be a normal ball spell */
	bool breath = false;

	/* Path ID */
	short path_n;

	/* Number of grids in the "blast area" (including the "beam" path) */
	int grids = 0;

	/* Encoded "radius" info (see above) */
	unsigned short gm[32];

	/* Actual radius encoded in gm[] */
	int gm_rad = rad;

	/* Default "Source" */
	x1 = ox;
	y1 = oy;

	y_saver = y1;
	x_saver = x1;

	/* Default "destination" */
	y2 = ty;
	x2 = tx;

	/* Handle a breath attack */
	if (flg & PROJECT_CONE)
	{
		breath = true;
	}

	/* Hack -- Assume there will be no blast (max radius 32) */
	for (dist = 0; dist < 32; dist++) gm[dist] = 0;

	/* Initial grid */
	y = y1;
	x = x1;
	dist = 0;

	/* Collect beam grids */
	if(add_step(project_n, ox, oy)) {
		return RLFL_ERR_GENERIC;
	}

	/* Calculate the projection path */
	path_n = RLFL_path_create(m, x1, y1, x2, y2, PATH_BASIC, range, flg, 0);
	if(path_n >= 0) {
		path_size = RLFL_path_size(path_n);

		/* Project along the path */
		for (i=0; i < path_size && i < range; ++i)
		{
			unsigned int ny, nx;
			RLFL_path_step(path_n, i, &nx, &ny);

			/* Handle PROJECT_THRU */
			if (!RLFL_has_flag(m, nx, ny, CELL_OPEN) && (!(flg & PROJECT_THRU)))
				break;

			/* Advance */
			y = ny;
			x = nx;

			/* Collect beam grids */
			if(add_step(project_n, x, y)) {
				return RLFL_ERR_GENERIC;
			}

			/* Target */
			if((!(flg & PROJECT_PASS)) && (x == tx && y == ty))
				break;
		}
	}

	path_size = i;

	/* Save the "blast epicenter" */
	by = y;
	bx = x;

	if (breath && !path_size)
	{
		breath = false;
		rad = 1;
	}

	/* Start the "explosion" */
	gm[0] = 0;

	/* Hack -- make sure beams get to "explode" */
	gm[1] = grids;

	dist = path_size;
	dist_hack = dist;

	/* If we found a "target", explode there */
	if (dist <= range)
	{
		/*
		 * Create a conical breath attack
		 *
		 *         ***
		 *     ********
		 * D********@**
		 *     ********
		 *         ***
		 */
		if (breath && dist > rad)
		{
			breath_shape(m, path_n, dist, &grids, gm, &gm_rad,
						 rad, y1, x1, by, bx, (bool)(flg & PROJECT_THRU), true);
		}
		else
		{
			dist = (flg & PROJECT_SHEL) ? rad : 0;
			if(ball_shape(m, project_n, dist, bx, by, rad, flg))
				return RLFL_ERR_GENERIC;
		}
	}
	/* Clear the path */
	RLFL_path_delete(path_n);

	/* Return "something was noticed" */
	return RLFL_SUCCESS;
}
 /*
  +-----------------------------------------------------------+
  * @desc	Add cell to projection
  +-----------------------------------------------------------+
  */
 static
 err add_step(int p, int x, int y) {
 	if((p >= RLFL_MAX_PROJECTS) || !RLFL_project_store[p])
 		return RLFL_ERR_NO_MAP;

 	int i;
 	/* Ignore duplicates */
 	for(i=0; i<RLFL_list_size(RLFL_project_store[p]); i++) {
 		RLFL_step_t *step = RLFL_list_get(RLFL_project_store[p], i);
 		if(step->X == x && step->Y == y) {
 			return RLFL_SUCCESS;
 		}
 	}
 	RLFL_step_t *step = (RLFL_step_t *)calloc(sizeof(RLFL_step_t), 1);
 	if(step == NULL) {
 		return RLFL_ERR_GENERIC;
 	}
 	step->Y = y;
 	step->X = x;
 	RLFL_list_append(RLFL_project_store[p], step);
 	return RLFL_SUCCESS;
 }
 /*
  +-----------------------------------------------------------+
  * @desc	Step over projection
  +-----------------------------------------------------------+
  */
 err
 RLFL_project_step(int p, int i, unsigned int *x, unsigned int *y) {
 	if((p >= RLFL_MAX_PROJECTS) || !RLFL_project_store[p])
 		return RLFL_ERR_NO_MAP;

 	RLFL_step_t *step = RLFL_list_get(RLFL_project_store[p], i);
 	if(!step) {
 		return RLFL_ERR_GENERIC;
 	}
 	*x = step->X;
 	*y = step->Y;
 	return RLFL_SUCCESS;
 }
 /*
  +-----------------------------------------------------------+
  * @desc	Delete projection
  +-----------------------------------------------------------+
  */
 err
 RLFL_project_delete(int p) {
 	if((p >= RLFL_MAX_PROJECTS) || !RLFL_project_store[p])
 		return RLFL_ERR_NO_MAP;

 	int i, size = RLFL_list_size(RLFL_project_store[p]);
 	for(i=0; i<size; i++) {
 		free(RLFL_list_get(RLFL_project_store[p], i));
 	}
 	RLFL_list_delete(RLFL_project_store[p]);
 	RLFL_project_store[p] = NULL;

 	return RLFL_SUCCESS;
 }
 /*
  +-----------------------------------------------------------+
  * @desc	Projection size
  +-----------------------------------------------------------+
  */
 unsigned int
 RLFL_project_size(int p) {
 	if((p >= RLFL_MAX_PROJECTS) || !RLFL_project_store[p])
 		return RLFL_ERR_NO_MAP;

 	return RLFL_list_size(RLFL_project_store[p]);
 }
 /**
  *
  */
static err
ball_shape(unsigned short m, unsigned short project_n, int dist, int bx, int by,
		   int rad, unsigned short flg) {
	int x, y;
	/* Determine the blast area, work from the inside out */
	for (;dist <= rad; dist++)
	{
		/* Scan the maximal blast area of radius "dist" */
		for (y = by - dist; y <= by + dist; y++)
		{
			for (x = bx - dist; x <= bx + dist; x++)
			{
				/* Ignore "illegal" locations */
				if (!RLFL_cell_valid(m, x, y))
					continue;

				if(flg & PROJECT_SQUARE)
				{
					/* Enforce a square "ripple" */
					if(abs(bx - x) != dist && abs(by - y) != dist)
						continue;
				}
				else if(flg & PROJECT_DIAMOND)
				{
					/* Enforce a diamond "ripple" */
					int ox = (x - bx);
					int oy = (y - by);
					if(ox && oy)
					{
						if((abs(ox) + abs(oy)) != dist)
							continue;
					}
					else
					{
						if (RLFL_distance(bx, by, x, y) != dist)
							continue;
					}
				}
				else
				{
					/* Enforce a circular "ripple" */
					if (RLFL_distance(bx, by, x, y) != dist)
						continue;
				}

				/* The blast is sometimes stopped by walls */
				if(!(flg & PROJECT_THRU) && !RLFL_has_flag(m, x, y, CELL_OPEN))
					continue;

				/* Save this grid */
				if(add_step(project_n, x, y)) {
					return RLFL_ERR_GENERIC;
				}
			}
		}
	}

	return RLFL_SUCCESS;
}
/*
 * breath shape
 */
static void
breath_shape(unsigned short m, unsigned short path_n, int dist, int *pgrids,
 			 unsigned short *gm, int *pgm_rad, int rad, int y1, int x1, int y2,
 			 int x2, bool disint_ball, bool real_breath)
{
	int by = y1;
	int bx = x1;
	int brad = 0;
	int brev = rad * rad / dist;
	int bdis = 0;
	int cdis;
	int path_index = 0;
	int mdis = RLFL_distance(x1, y1, x2, y2) + rad;

	while (bdis <= mdis)
	{
		int x, y;

		if ((0 < dist) && (path_index < dist))
		{
			unsigned int ny, nx;
			RLFL_path_step(path_n, path_index, &nx, &ny);
			int nd = RLFL_distance(nx, ny, x1, y1);

			/* Get next base point */
			if (bdis >= nd)
			{
				by = ny;
				bx = nx;
				path_index++;
			}
		}

		/* Travel from center outward */
		for (cdis = 0; cdis <= brad; cdis++)
		{
			/* Scan the maximal blast area of radius "cdis" */
			for (y = by - cdis; y <= by + cdis; y++)
			{
				for (x = bx - cdis; x <= bx + cdis; x++)
				{
					/* Ignore "illegal" locations */
					if (!RLFL_cell_valid(m, x, y))
						continue;

					/* Enforce a circular "ripple" */
					if (RLFL_distance(x1, y1, x, y) != bdis)
						continue;

					/* Enforce an arc */
					if (RLFL_distance(bx, by, x, y) != cdis)
						continue;

					/* The blast is sometimes stopped by walls */
					if (!disint_ball && !RLFL_has_flag(m, x, y, CELL_OPEN))
						continue;

					/* Save this grid */
					add_step(path_n, x, y);
				}
			}
		}

		/* Encode some more "radius" info */
		gm[bdis + 1] = *pgrids;

		/* Increase the size */
		brad = rad * (path_index + brev) / (dist + brev);

		/* Find the next ripple */
		bdis++;
	}

	/* Store the effect size */
	*pgm_rad = bdis;
}
