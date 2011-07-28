/*
	Mingos' Restrictive Precise Angle Shadowcasting (MRPAS)
 	by Dominik "Mingos" Marczuk.
 	Original implementation:
 	http://umbrarumregnum.110mb.com/downloads/MRPAS.zip

	Adapted from codethat was found in libtcod
	<http://doryen.eptalys.net/libtcod/>

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
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
static inline void
restrictive_shadowcasting_quadrant (RLFL_map_t *m, int player_x, int player_y, int max_radius,
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
                if (obstaclesInLastLine > 0 && !RLFL_has_flag(m->mnum, x, y, CELL_SEEN)) {
                    int idx = 0;
                    while(visible && idx < obstaclesInLastLine) {
                        if (RLFL_has_flag(m->mnum, x, y, CELL_OPEN)) {
                            if (centreSlope > startAngle[idx] && centreSlope < endAngle[idx])
                                visible = false;
                            }
                        else {
                            if (startSlope >= startAngle[idx] && endSlope <= endAngle[idx])
                                visible = false;
                        }
                        if (visible && !RLFL_has_flag(m->mnum, x, y-dy, CELL_SEEN | CELL_OPEN)
                        		&& (x-dx >= 0 && x-dx < m->width
                        		&& !RLFL_has_flag(m->mnum, x-dx, y-dy, CELL_SEEN | CELL_OPEN))) {
                        	visible = false;
                        }
                        idx++;
                    }
                }
                if (visible) {
                	RLFL_set_flag(m->mnum, x, y, CELL_FOV);
                    done = false;
                    //if the cell is opaque, block the adjacent slopes
                    if (!RLFL_has_flag(m->mnum, x, y, CELL_OPEN)) {
                        if (minAngle >= startSlope) minAngle = endSlope;
                        else {
                        	startAngle[totalObstacles] = startSlope;
                        	endAngle[totalObstacles++] = endSlope;
                        }
                        if (!light_walls) RLFL_clear_flag(m->mnum, x, y, CELL_SEEN);
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
                if (obstaclesInLastLine > 0 && !RLFL_has_flag(m->mnum, x, y, CELL_SEEN)) {
                    int idx = 0;
                    while(visible && idx < obstaclesInLastLine) {
                        if (RLFL_has_flag(m->mnum, x, y, CELL_OPEN)) {
                            if (centreSlope > startAngle[idx] && centreSlope < endAngle[idx])
                                visible = false;
                            }
                        else {
                            if (startSlope >= startAngle[idx] && endSlope <= endAngle[idx])
                                visible = false;
                        }
                        if (visible && !RLFL_has_flag(m->mnum, x-dx, y, CELL_SEEN | CELL_OPEN)
                                && (y-dy >= 0 && y-dy < m->height
                                && !RLFL_has_flag(m->mnum, x-dx, y-dy, CELL_SEEN | CELL_OPEN))) {
                        	visible = false;
                        }
                        idx++;
                    }
                }
                if (visible) {
                	RLFL_set_flag(m->mnum, x, y, CELL_FOV);
                    done = false;
                    //if the cell is opaque, block the adjacent slopes
                    if (!RLFL_has_flag(m->mnum, x, y, CELL_OPEN)) {
                        if (minAngle >= startSlope) minAngle = endSlope;
                        else {
                        	startAngle[totalObstacles] = startSlope;
                        	endAngle[totalObstacles++] = endSlope;
                        }
                        if (!light_walls) RLFL_clear_flag(m->mnum, x, y, CELL_SEEN);;
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
RLFL_fov_restrictive_shadowcasting(unsigned int m, unsigned int ox, unsigned int oy, int radius, bool light_walls)
{
	if(!RLFL_map_valid(m))
		return RLFL_ERR_NO_MAP;

	if(!RLFL_cell_valid(m, ox, oy))
		return RLFL_ERR_OUT_OF_BOUNDS;

	if(radius >= RLFL_MAX_RADIUS)
		return RLFL_ERR_GENERIC;

	RLFL_map_t *map = RLFL_map_store[m];

    //calculate an approximated (excessive, just in case) maximum number of obstacles per octant
    int maxObstacles = (map->width * map->height) / 7;

    /* The origin is always seen */
    RLFL_set_flag(m, ox, oy, CELL_FOV);

    //compute the 4 quadrants of the map
    restrictive_shadowcasting_quadrant(map, ox, oy, radius, light_walls, maxObstacles, 1, 1);
    restrictive_shadowcasting_quadrant(map, ox, oy, radius, light_walls, maxObstacles, 1, -1);
    restrictive_shadowcasting_quadrant(map, ox, oy, radius, light_walls, maxObstacles, -1, 1);
    restrictive_shadowcasting_quadrant(map, ox, oy, radius, light_walls, maxObstacles, -1, -1);

    return RLFL_SUCCESS;
}
