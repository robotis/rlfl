/*
 +-----------------------------------------------------------+
 * @desc	Björn Bergström's excellent recursive shadowcasting
 * 			FOV algorithm.
 * 			bjorn.bergstrom@roguelikedevelopment.org
 * @file	fov_recursive_shadowcasting.c
 * @package RLF
 * <jtm@robot.is>
 +-----------------------------------------------------------+
 */

#include "headers/rlfl.h"
/*
 *	Multipliers for transforming coordinates to other octant
 * */
static int
mult[4][8]= {
	{1,	 0,	 0,  -1,  -1,	0,	0,	1},
	{0,	 1, -1,   0,   0,  -1,  1,  0},
	{0,  1,  1,   0,   0,  -1, -1,  0},
	{1,  0,  0,   1,  -1,   0,  0, -1},
};
// functions
static void cast_light(unsigned int m, int cx, int cy,int row,float start, float end, int radius,
					   int xx, int xy, int yx, int yy, int id, bool light_walls);
/*
 +-----------------------------------------------------------+
 * @desc	Field of view
 +-----------------------------------------------------------+
 */
err
RLFL_fov_recursive_shadowcasting(unsigned int m, unsigned int ox, unsigned int oy, int radius, bool light_walls)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	int oct;
	for(oct=0; oct<8; oct++)
	{
		cast_light(m, ox, oy, 1, 1.0, 0.0, radius, mult[0][oct], mult[1][oct],
				   mult[2][oct], mult[3][oct], 0, light_walls);
	}
	/* The origin is always seen */
	RLFL_set_flag(m, ox, oy, CELL_FOV);

	return 0;
}
/*
 +-----------------------------------------------------------+
 * @desc	Recursive lightcasting function
 +-----------------------------------------------------------+
 */
static void
cast_light(unsigned int m, int cx, int cy,int row,float start, float end, int radius,
		   int xx, int xy, int yx, int yy, int id, bool light_walls)
{
	if (start < end)
		return;
	RLFL_map_t *map = RLFL_map_store[m];
	int r2 = radius * radius;
	int j, dx, dy;
	float new_start = 0.0f;
	float l_slope, r_slope;
	bool blocked;
	for(j=row; j < (radius + 1); j++) {
		dx = -j - 1;
		dy = -j;
		blocked = false;
		while (dx <= 0) {
			int X, Y;
			dx++;
			/* Translate the dx, dy coordinates into map coordinates */
			X = cx + dx * xx + dy * xy;
			Y = cy + dx * yx + dy * yy;
			/* l_slope and r_slope store the slopes of the left and right
               extremities of the square we're considering */
			if ((unsigned)X < (unsigned)map->width && (unsigned)Y < (unsigned)map->height) {
				l_slope = (dx - 0.5f) / (dy + 0.5f);
				r_slope = (dx + 0.5f) / (dy - 0.5f);
				if(start < r_slope)
					continue;
				else if(end > l_slope)
					break;
				if(dx * dx + dy * dy <= r2) {
					if(light_walls || RLFL_has_flag(m, X, Y, CELL_OPEN)) {
						/* Our light beam is touching this square; light it */
						RLFL_set_flag(m, X, Y, CELL_FOV);
					}
				}
				if(blocked) {
					/* we're scanning a row of blocked squares */
					if (!RLFL_has_flag(m, X, Y, CELL_OPEN)) {
						new_start = r_slope;
						continue;
					} else {
						blocked = false;
						start = new_start;
					}
				} else {
					if(!RLFL_has_flag(m, X, Y, CELL_OPEN) && j < radius) {
						/* This is a blocking square, start a child scan */
						blocked = true;
						cast_light(m, cx, cy, (j + 1), start, l_slope, radius,
								   xx, xy, yx, yy, (id + 1), light_walls);
						new_start = r_slope;
					}
				}
			}
		}
		/* Row is scanned; do next row unless last square was blocked */
		if (blocked) break;
	}
}

