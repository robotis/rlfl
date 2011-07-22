/*
 +-----------------------------------------------------------+
 * @desc	Digital FOV algorithm
 * @file	fov_digital.c
 * @package RLF
 * @license GPL
 * @from	libtcod - http://doryen.eptalys.net/libtcod/
 * <jtm@robot.is>
 +-----------------------------------------------------------+
 */

#include "headers/rlf.h"

#define CCW(x1,y1,x2,y2,x3,y3) ((x1)*(y2) + (x2)*(y3) + (x3)*(y1) - (x1)*(y3) - (x2)*(y1) - (x3)*(y2))

static void draw (map_t *m,int cx, int cy, int dis, int px, int py, short light_walls);
static void trace(map_t *m, int dir, int n, int h, int px, int py, bool light_walls);
/*
 +-----------------------------------------------------------+
 * @desc	RLF_fov_digital
 +-----------------------------------------------------------+
 */
err
RLF_fov_digital(unsigned int m, unsigned int ox, unsigned int oy, int radius, bool light_walls) {
	int dir, i;
	map_t *map = map_store[m];

	// Player cell
	RLF_set_flag(m, ox, oy, CELL_FOV);

	// calculate fov using digital lines
	for (dir=0; dir < 8; dir++) {
		for (i =0; i < radius+1; i++) {
			trace(map, dir, radius, i, ox, oy, light_walls);
		}
	}

	return RLF_SUCCESS;
}
/*
 +-----------------------------------------------------------+
 * @desc	Fill inn seen states
 +-----------------------------------------------------------+
 */
static void
draw(map_t *m,int cx, int cy, int dis, int px, int py, short light_walls) {
	if(!RLF_cell_valid(m->mnum, (unsigned)cx, (unsigned)cy)) {
		return;
	}
	// circular view - can be changed if you like
	if ((cx-px)*(cx-px) + (cy-py)*(cy-py) <= dis*dis + 1) {
		if(RLF_has_flag(m->mnum, cx, cy, CELL_OPEN) || light_walls) {
			RLF_set_flag(m->mnum, cx, cy, CELL_FOV);
		}
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static void
trace(map_t *m, int dir, int n, int h, int px, int py, bool light_walls) {
	/* convex hull of obstructions */
	int topx[n+2], topy[n+2], botx[n+2], boty[n+2];
	/* size of top and bottom convex hulls */
	int curt = 0, curb = 0;
	// too lazy to think of real variable names, four critical points on the convex hulls - these four points determine what is visible
	int s[2][2] = {{0, 0}, {0, 0}};
	int ad1,ad2[2]={0,0},eps[2] = {0, n-1},i;
	topx[0] = botx[0] = boty[0] = 0, topy[0] = 1;
	for (ad1 = 1; ad1 <= n; ++ad1) {
		for(i=0; i <2; i++) {
			// good old Bresenham
			eps[i] += h;
			if (eps[i] >= n) {
				eps[i] -= n;
				++ad2[i];
			}
		}
		for(i=0; i <2; i++)
			if (CCW(topx[s[!i][1]], topy[s[!i][1]], botx[s[i][0]], boty[s[i][0]], ad1, ad2[i]+i) <= 0)
				return;	// the relevant region is no longer visible. If we don't exit the loop now, strange things happen.
		int cx[2] = {ad1, ad1}, cy[2] = {ad2[0], ad2[1]};
		for(i=0; i <2; i++) {
			if (dir&1) cx[i] = -cx[i];
			if (dir&2) cy[i] = -cy[i];
			if (dir&4) cx[i] ^= cy[i], cy[i] ^= cx[i], cx[i] ^= cy[i];
			cx[i] += px, cy[i] += py;

			if (CCW(topx[s[i][1]], topy[s[i][1]], botx[s[!i][0]], boty[s[!i][0]], ad1, ad2[i]+1-i) > 0) {
				draw(m,cx[i], cy[i], n,px,py,light_walls);
			}
		}
		if ( (unsigned)cx[0] < (unsigned)m->width && (unsigned)cy[0] < (unsigned)m->height) {
			if (!RLF_has_flag(m->mnum, cx[0], cy[0], CELL_OPEN)) { // new obstacle, update convex hull
				++curb;
				botx[curb] = ad1, boty[curb] = ad2[0]+1;
				if (CCW(botx[s[0][0]], boty[s[0][0]], topx[s[1][1]], topy[s[1][1]], ad1, ad2[0]+1) >= 0)
					return;	// the obstacle obscures everything
				if (CCW(topx[s[0][1]], topy[s[0][1]], botx[s[1][0]], boty[s[1][0]], ad1, ad2[0]+1) >= 0) {
					s[1][0] = curb;	// updating visible region
					while (s[0][1] < curt && CCW(topx[s[0][1]], topy[s[0][1]], topx[s[0][1]+1], topy[s[0][1]+1], ad1, ad2[0]+1) >= 0) ++s[0][1];
				}
				while (curb > 1 && CCW(botx[curb-2], boty[curb-2], botx[curb-1], boty[curb-1], ad1, ad2[0]+1) >= 0) {	// not convex anymore, delete a point
					if (s[1][0] == curb)
						--s[1][0];	// s[0][0] won't be a problem
					--curb;
					botx[curb] = botx[curb+1], boty[curb] = boty[curb+1];
				}
			}
		}

		if ( (unsigned)cx[1] < (unsigned)m->width && (unsigned)cy[1] < (unsigned)m->height) {
			if (!RLF_has_flag(m->mnum, cx[1], cy[1], CELL_OPEN)) { // same as above
				++curt;
				topx[curt] = ad1, topy[curt] = ad2[1];
				if (CCW(botx[s[1][0]], boty[s[1][0]], topx[s[0][1]], topy[s[0][1]], ad1, ad2[1]) >= 0)
					return;
				if (CCW(topx[s[1][1]], topy[s[1][1]], botx[s[0][0]], boty[s[0][0]], ad1, ad2[1]) >= 0) {
					s[1][1] = curt;
					while (s[0][0] < curb && CCW(botx[s[0][0]], boty[s[0][0]], botx[s[0][0]+1], boty[s[0][0]+1], ad1, ad2[1]) <= 0) ++s[0][0];
				}
				while (curt > 1 && CCW(topx[curt-2], topy[curt-2], topx[curt-1], topy[curt-1], ad1, ad2[1]) <= 0) {
					if (s[1][1] == curt) --s[1][1];
					--curt;
					topx[curt] = topx[curt+1], topy[curt] = topy[curt+1];
				}
			}
		}
	}
}
