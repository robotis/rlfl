/*
 +-----------------------------------------------------------+
 * @desc	Mingos' Restrictive Precise Angle Shadowcasting (MRPAS)
 * 			This file was written by Dominik "Mingos" Marczuk.
 * 			Original implementation:
 * 			http://umbrarumregnum.110mb.com/downloads/MRPAS.zip
 * @file	fov_restrictive.c
 * @package RLF
 * @license FIXME
 * <jtm@robot.is>
 +-----------------------------------------------------------+
 */

#include "headers/rlf.h"
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static inline void
restrictive_shadowcasting_quadrant (map_t *m, int player_x, int player_y, int max_radius,
									bool light_walls, int maxObstacles, int dx, int dy)
{
    //octant: vertical edge
    {
        int iteration = 1; //iteration of the algo for this octant
        bool done = false;
        int totalObstacles = 0;
        int obstaclesInLastLine = 0;
        double startAngle[maxObstacles], endAngle[maxObstacles];
		double minAngle = 0.0f;

        //do while there are unblocked slopes left and the algo is within the map's boundaries
        //scan progressive lines/columns from the PC outwards
        int x, y = player_y+dy; //the outer slope's coordinates (first processed line)
        if (y < 0 || y >= m->height) done = true;
		while(!done) {
            done = true;
            //process cells in the line
			double slopesPerCell = 1.0f/(double)(iteration+1);
			double halfSlopes = slopesPerCell*0.5f;
			int processedCell = minAngle / slopesPerCell;
            int minx = MAX(0,player_x-iteration), maxx = MIN(m->width-1,player_x+iteration);
            for (x = player_x + (processedCell * dx); x >= minx && x <= maxx; x+=dx) {
                //calculate slopes per cell
                bool visible = true;
                double startSlope = (double)processedCell*slopesPerCell;
                double centreSlope = startSlope+halfSlopes;
                double endSlope = startSlope+slopesPerCell;
                if (obstaclesInLastLine > 0 && !RLF_has_flag(m->mnum, x, y, CELL_SEEN)) {
                    int idx = 0;
                    while(visible && idx < obstaclesInLastLine) {
                        if (RLF_has_flag(m->mnum, x, y, CELL_OPEN)) {
                            if (centreSlope > startAngle[idx] && centreSlope < endAngle[idx])
                                visible = false;
                            }
                        else {
                            if (startSlope >= startAngle[idx] && endSlope <= endAngle[idx])
                                visible = false;
                        }
                        if (visible && !RLF_has_flag(m->mnum, x, y-dy, CELL_SEEN | CELL_OPEN)
                        		&& (x-dx >= 0 && x-dx < m->width
                        		&& !RLF_has_flag(m->mnum, x-dx, y-dy, CELL_SEEN | CELL_OPEN))) {
                        	visible = false;
                        }
                        idx++;
                    }
                }
                if (visible) {
                	RLF_set_flag(m->mnum, x, y, CELL_SEEN);
                    done = false;
                    //if the cell is opaque, block the adjacent slopes
                    if (!RLF_has_flag(m->mnum, x, y, CELL_OPEN)) {
                        if (minAngle >= startSlope) minAngle = endSlope;
                        else {
                        	startAngle[totalObstacles] = startSlope;
                        	endAngle[totalObstacles++] = endSlope;
                        }
                        if (!light_walls) RLF_clear_flag(m->mnum, x, y, CELL_SEEN);
                    }
                }
                processedCell++;
            }
            if (iteration == max_radius) done = true;
            iteration++;
            obstaclesInLastLine = totalObstacles;
            y += dy;
            if (y < 0 || y >= m->height) done = true;
			if ( minAngle == 1.0f ) done=true;
        }
    }
    //octant: horizontal edge
    {
        int iteration = 1; //iteration of the algo for this octant
        bool done = false;
        int totalObstacles = 0;
        int obstaclesInLastLine = 0;
        double startAngle[maxObstacles], endAngle[maxObstacles];
		double minAngle = 0.0f;

        //do while there are unblocked slopes left and the algo is within the map's boundaries
        //scan progressive lines/columns from the PC outwards
        int x = player_x+dx, y; //the outer slope's coordinates (first processed line)
        if (x < 0 || x >= m->width) done = true;
		while(!done) {
            done = true;
            //process cells in the line
			double slopesPerCell = 1.0f/(double)(iteration+1);
			double halfSlopes = slopesPerCell*0.5f;
			int processedCell = minAngle / slopesPerCell;
            int miny = MAX(0,player_y-iteration), maxy = MIN(m->height-1,player_y+iteration);
            for (y = player_y + (processedCell * dy); y >= miny && y <= maxy; y+=dy) {
                //calculate slopes per cell
                bool visible = true;
                double startSlope = (double)processedCell*slopesPerCell;
                double centreSlope = startSlope+halfSlopes;
                double endSlope = startSlope+slopesPerCell;
                if (obstaclesInLastLine > 0 && !RLF_has_flag(m->mnum, x, y, CELL_SEEN)) {
                    int idx = 0;
                    while(visible && idx < obstaclesInLastLine) {
                        if (RLF_has_flag(m->mnum, x, y, CELL_OPEN)) {
                            if (centreSlope > startAngle[idx] && centreSlope < endAngle[idx])
                                visible = false;
                            }
                        else {
                            if (startSlope >= startAngle[idx] && endSlope <= endAngle[idx])
                                visible = false;
                        }
                        if (visible && !RLF_has_flag(m->mnum, x-dx, y, CELL_SEEN | CELL_OPEN)
                                && (y-dy >= 0 && y-dy < m->height
                                && !RLF_has_flag(m->mnum, x-dx, y-dy, CELL_SEEN | CELL_OPEN))) {
                        	visible = false;
                        }
                        idx++;
                    }
                }
                if (visible) {
                	RLF_set_flag(m->mnum, x, y, CELL_SEEN);
                    done = false;
                    //if the cell is opaque, block the adjacent slopes
                    if (!RLF_has_flag(m->mnum, x, y, CELL_OPEN)) {
                        if (minAngle >= startSlope) minAngle = endSlope;
                        else {
                        	startAngle[totalObstacles] = startSlope;
                        	endAngle[totalObstacles++] = endSlope;
                        }
                        if (!light_walls) RLF_clear_flag(m->mnum, x, y, CELL_SEEN);;
                    }
                }
                processedCell++;
            }
            if (iteration == max_radius) done = true;
            iteration++;
            obstaclesInLastLine = totalObstacles;
            x += dx;
            if (x < 0 || x >= m->width) done = true;
			if ( minAngle == 1.0f ) done=true;
        }
    }
}
/*
 +-----------------------------------------------------------+
 * @desc	RLF Fov
 +-----------------------------------------------------------+
 */
err
RLF_fov_restrictive_shadowcasting(unsigned int m, unsigned int ox, unsigned int oy, int radius, bool light_walls)
{
	if(!map_store[m])
		return RLF_ERR_NO_MAP;
	map_t *map = map_store[m];

    //calculate an approximated (excessive, just in case) maximum number of obstacles per octant
    int maxObstacles = (map->width * map->height) / 7;

    /* The origin is always seen */
    RLF_set_flag(m, ox, oy, CELL_SEEN);

    //compute the 4 quadrants of the map
    restrictive_shadowcasting_quadrant(map, ox, oy, radius, light_walls, maxObstacles, 1, 1);
    restrictive_shadowcasting_quadrant(map, ox, oy, radius, light_walls, maxObstacles, 1, -1);
    restrictive_shadowcasting_quadrant(map, ox, oy, radius, light_walls, maxObstacles, -1, 1);
    restrictive_shadowcasting_quadrant(map, ox, oy, radius, light_walls, maxObstacles, -1, -1);

    return RLF_SUCCESS;
}
