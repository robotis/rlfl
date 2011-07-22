/*
 *
 */
#include "headers/rlf.h"
#include "headers/project.h"

static err add_step(int p, int x, int y);
static void breath_shape(unsigned short m, unsigned short path_n, int dist, int *pgrids,
						 unsigned short *gm, int *pgm_rad, int rad, int y1, int x1, int y2,
						 int x2, bool disint_ball, bool real_breath);
/*
 * Hengband
 *
 * Generic "beam"/"bolt"/"ball" projection routine.
 *
 * Input:
 *   who: Index of "source" monster (zero for "player")
 *   rad: Radius of explosion (0 = beam/bolt, 1 to 9 = ball)
 *   y,x: Target location (or location to travel "towards")
 *   dam: Base damage roll to apply to affected monsters (or player)
 *   typ: Type of damage to apply to monsters (and objects)
 *   flg: Extra bit flags (see PROJECT_xxxx in "defines.h")
 *
 * Return:
 *   TRUE if any "effects" of the projection were observed, else FALSE
 *
 * Allows a monster (or player) to project a beam/bolt/ball of a given kind
 * towards a given location (optionally passing over the heads of interposing
 * monsters), and have it do a given amount of damage to the monsters (and
 * optionally objects) within the given radius of the final location.
 *
 * A "bolt" travels from source to target and affects only the target grid.
 * A "beam" travels from source to target, affecting all grids passed through.
 * A "ball" travels from source to the target, exploding at the target, and
 *   affecting everything within the given radius of the target location.
 *
 * Traditionally, a "bolt" does not affect anything on the ground, and does
 * not pass over the heads of interposing monsters, much like a traditional
 * missile, and will "stop" abruptly at the "target" even if no monster is
 * positioned there, while a "ball", on the other hand, passes over the heads
 * of monsters between the source and target, and affects everything except
 * the source monster which lies within the final radius, while a "beam"
 * affects every monster between the source and target, except for the casting
 * monster (or player), and rarely affects things on the ground.
 *
 * Two special flags allow us to use this function in special ways, the
 * "PROJECT_HIDE" flag allows us to perform "invisible" projections, while
 * the "PROJECT_JUMP" flag allows us to affect a specific grid, without
 * actually projecting from the source monster (or player).
 *
 * The player will only get "experience" for monsters killed by himself
 * Unique monsters can only be destroyed by attacks from the player
 *
 * Only 256 grids can be affected per projection, limiting the effective
 * "radius" of standard ball attacks to nine units (diameter nineteen).
 *
 * One can project in a given "direction" by combining PROJECT_THRU with small
 * offsets to the initial location (see "line_spell()"), or by calculating
 * "virtual targets" far away from the player.
 *
 * One can also use PROJECT_THRU to send a beam/bolt along an angled path,
 * continuing until it actually hits somethings (useful for "stone to mud").
 *
 * Bolts and Beams explode INSIDE walls, so that they can destroy doors.
 *
 * Balls must explode BEFORE hitting walls, or they would affect monsters
 * on both sides of a wall.  Some bug reports indicate that this is still
 * happening in 2.7.8 for Windows, though it appears to be impossible.
 *
 * We "pre-calculate" the blast area only in part for efficiency.
 * More importantly, this lets us do "explosions" from the "inside" out.
 * This results in a more logical distribution of "blast" treasure.
 * It also produces a better (in my opinion) animation of the explosion.
 * It could be (but is not) used to have the treasure dropped by monsters
 * in the middle of the explosion fall "outwards", and then be damaged by
 * the blast as it spreads outwards towards the treasure drop location.
 *
 * Walls and doors are included in the blast area, so that they can be
 * "burned" or "melted" in later versions.
 *
 * This algorithm is intended to maximize simplicity, not necessarily
 * efficiency, since this function is not a bottleneck in the code.
 *
 * We apply the blast effect from ground zero outwards, in several passes,
 * first affecting features, then objects, then monsters, then the player.
 * This allows walls to be removed before checking the object or monster
 * in the wall, and protects objects which are dropped by monsters killed
 * in the blast, and allows the player to see all affects before he is
 * killed or teleported away.  The semantics of this method are open to
 * various interpretations, but they seem to work well in practice.
 *
 * We process the blast area from ground-zero outwards to allow for better
 * distribution of treasure dropped by monsters, and because it provides a
 * pleasing visual effect at low cost.
 *
 * Note that the damage done by "ball" explosions decreases with distance.
 * This decrease is rapid, grids at radius "dist" take "1/dist" damage.
 *
 * Notice the "napalm" effect of "beam" weapons.  First they "project" to
 * the target, and then the damage "flows" along this beam of destruction.
 * The damage at every grid is the same as at the "center" of a "ball"
 * explosion, since the "beam" grids are treated as if they ARE at the
 * center of a "ball" explosion.
 *
 * Currently, specifying "beam" plus "ball" means that locations which are
 * covered by the initial "beam", and also covered by the final "ball", except
 * for the final grid (the epicenter of the ball), will be "hit twice", once
 * by the initial beam, and once by the exploding ball.  For the grid right
 * next to the epicenter, this results in 150% damage being done.  The center
 * does not have this problem, for the same reason the final grid in a "beam"
 * plus "bolt" does not -- it is explicitly removed.  Simply removing "beam"
 * grids which are covered by the "ball" will NOT work, as then they will
 * receive LESS damage than they should.  Do not combine "beam" with "ball".
 *
 * The array "gy[],gx[]" with current size "grids" is used to hold the
 * collected locations of all grids in the "blast area" plus "beam path".
 *
 * Note the rather complex usage of the "gm[]" array.  First, gm[0] is always
 * zero.  Second, for N>1, gm[N] is always the index (in gy[],gx[]) of the
 * first blast grid (see above) with radius "N" from the blast center.  Note
 * that only the first gm[1] grids in the blast area thus take full damage.
 * Also, note that gm[rad+1] is always equal to "grids", which is the total
 * number of blast grids.
 *
 * Note that once the projection is complete, (y2,x2) holds the final location
 * of bolts/beams, and the "epicenter" of balls.
 *
 * Note also that "rad" specifies the "inclusive" radius of projection blast,
 * so that a "rad" of "one" actually covers 5 or 9 grids, depending on the
 * implementation of the "distance" function.  Also, a bolt can be properly
 * viewed as a "ball" with a "rad" of "zero".
 *
 * Note that if no "target" is reached before the beam/bolt/ball travels the
 * maximum distance allowed (MAX_RANGE), no "blast" will be induced.  This
 * may be relevant even for bolts, since they have a "1x1" mini-blast.
 *
 * Note that for consistency, we "pretend" that the bolt actually takes "time"
 * to move from point A to point B, even if the player cannot see part of the
 * projection path.  Note that in general, the player will *always* see part
 * of the path, since it either starts at the player or ends on the player.
 *
 * Hack -- we assume that every "projection" is "self-illuminating".
 *
 * Hack -- when only a single monster is affected, we automatically track
 * (and recall) that monster, unless "PROJECT_JUMP" is used.
 *
 * Note that all projections now "explode" at their final destination, even
 * if they were being projected at a more distant destination.  This means
 * that "ball" spells will *always* explode.
 *
 * Note that we must call "handle_stuff()" after affecting terrain features
 * in the blast radius, in case the "illumination" of the grid was changed,
 * and "update_view()" and "update_monsters()" need to be called.
 */
err
RLF_project(int m, int ox, int oy, int tx, int ty, int rad, int range, unsigned short flg)
{
	//printf("(%d, %d)(%d, %d), %d, %d, %d\n", ox, oy, tx, ty, rad, range, flg);
	if(!map_store[m]) return RLF_ERR_NO_MAP;
	int i, dist, project_n, path_size;
	for(project_n=0; project_n<MAX_PROJECT; project_n++){
		if(!project_store[project_n]) break;
	}
	/* assert path */
	if(project_n >= MAX_PROJECT) return RLF_ERR_FLAG;

	/* assert cells */
	if(!RLF_cell_valid(m, ox, oy)) return RLF_ERR_GENERIC;
	if(!RLF_cell_valid(m, tx, ty)) return RLF_ERR_GENERIC;

	/* prepare */
	list_t * projection = RLF_list_create();
	if(projection == NULL) return RLF_ERR_FLAG;
	project_store[project_n] = projection;

	if(range < 0) range = MAX_RANGE;

	int y1, x1, y, x;
	int y2, x2;
	int by, bx;

	int dist_hack = 0;

	int y_saver, x_saver; /* For reflecting monsters */

	/* Assume to be a normal ball spell */
	bool breath = false;

	/* Number of grids in the "path" */
	unsigned short path_n = 0;

	/* Number of grids in the "blast area" (including the "beam" path) */
	int grids = 0;

	/* Encoded "radius" info (see above) */
	unsigned short gm[32];

	/* Actual radius encoded in gm[] */
	int gm_rad = rad;
	bool jump = false;

	/* Hack -- Jump to target */
	if (flg & (PROJECT_JUMP))
	{
		/* Clear the flag */
		flg &= ~(PROJECT_JUMP);
		jump = true;
	}

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
		return RLF_ERR_GENERIC;
	}

	/* Calculate the projection path */
	path_n = RLF_path_create(m, x1, y1, x2, y2, PATH_BASIC, range, flg, 0);

	if(path_n >= 0) {
		path_size = RLF_path_size(path_n);
		/* Project along the path */
		for (i=0; i < path_size && i < range; ++i)
		{
			unsigned int ny, nx;
			RLF_path_step(path_n, i, &nx, &ny);

			/* Hack -- Balls explode before reaching walls */
			if (!RLF_has_flag(m, nx, ny, CELL_OPEN) && (rad > 0)) break;

			/* Handle PROJECT_THRU */
			// FIXME

			/* Advance */
			y = ny;
			x = nx;

			/* Collect beam grids */
			if(add_step(project_n, x, y)) {
				return RLF_ERR_GENERIC;
			}
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
			/* Determine the blast area, work from the inside out */
			for (dist = 0; dist <= rad; dist++)
			{
				/* Scan the maximal blast area of radius "dist" */
				for (y = by - dist; y <= by + dist; y++)
				{
					for (x = bx - dist; x <= bx + dist; x++)
					{
						/* Ignore "illegal" locations */
						if (!RLF_cell_valid(m, x, y))
							continue;

						/* Enforce a circular "ripple" */
						if (RLF_distance(bx, by, x, y) != dist)
							continue;

						/* The blast is sometimes stopped by walls */
						if(!(flg & PROJECT_THRU) && !RLF_has_flag(m, x, y, CELL_OPEN))
							continue;

						/* Save this grid */
						if(add_step(project_n, x, y)) {
							return RLF_ERR_GENERIC;
						}
					}
				}

				/* Encode some more "radius" info */
				gm[dist+1] = grids;
			}
		}
	}
	/* Clear the path */
	RLF_path_delete(path_n);

	/* Return "something was noticed" */
	return RLF_SUCCESS;
}
 /*
  +-----------------------------------------------------------+
  * @desc	Add cell to projection
  +-----------------------------------------------------------+
  */
 static
 err add_step(int p, int x, int y) {
 	if(!project_store[p]) return RLF_ERR_NO_MAP;
 	int i;
 	/* Ignore duplicates */
 	for(i=0; i<RLF_list_size(project_store[p]); i++) {
 		step_t *step = RLF_list_get(project_store[p], i);
 		if(step->X == x && step->Y == y) {
 			return RLF_SUCCESS;
 		}
 	}
 	step_t *step = (step_t *)calloc(sizeof(step_t), 1);
 	if(step == NULL) {
 		return RLF_ERR_GENERIC;
 	}
 	step->Y = y;
 	step->X = x;
 	RLF_list_append(project_store[p], step);
 	return RLF_SUCCESS;
 }
 /*
  +-----------------------------------------------------------+
  * @desc	Step over projection
  +-----------------------------------------------------------+
  */
 err
 RLF_project_step(int p, int i, unsigned int *x, unsigned int *y) {
 	if(!project_store[p]) return RLF_ERR_NO_MAP;
 	step_t *step = RLF_list_get(project_store[p], i);
 	if(!step) {
 		return RLF_ERR_GENERIC;
 	}
 	*x = step->X;
 	*y = step->Y;
 	return RLF_SUCCESS;
 }
 /*
  +-----------------------------------------------------------+
  * @desc	Delete projection
  +-----------------------------------------------------------+
  */
 err
 RLF_project_delete(int p) {
 	if(!project_store[p]) return RLF_ERR_NO_MAP;
 	int i, size = RLF_list_size(project_store[p]);
 	for(i=0; i<size; i++) {
 		free(RLF_list_get(project_store[p], i));
 	}
 	RLF_list_delete(project_store[p]);
 	project_store[p] = NULL;
 	return RLF_SUCCESS;
 }
 /*
  +-----------------------------------------------------------+
  * @desc	Projection size
  +-----------------------------------------------------------+
  */
 unsigned int
 RLF_project_size(int p) {
 	if(!project_store[p]) return RLF_ERR_NO_MAP;
 	return RLF_list_size(project_store[p]);
 }
/*
 * breath shape
 */
static void breath_shape(unsigned short m, unsigned short path_n, int dist, int *pgrids,
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
	int mdis = RLF_distance(x1, y1, x2, y2) + rad;

	while (bdis <= mdis)
	{
		int x, y;

		if ((0 < dist) && (path_index < dist))
		{
			unsigned int ny, nx;
			RLF_path_step(path_n, path_index, &nx, &ny);
			int nd = RLF_distance(nx, ny, x1, y1);

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
					if (!RLF_cell_valid(m, x, y))
						continue;

					/* Enforce a circular "ripple" */
					if (RLF_distance(x1, y1, x, y) != bdis)
						continue;

					/* Enforce an arc */
					if (RLF_distance(bx, by, x, y) != cdis)
						continue;

					/* The blast is sometimes stopped by walls */
					if (!disint_ball && !RLF_has_flag(m, x, y, CELL_OPEN))
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
