/*
 +-----------------------------------------------------------+
 * @desc	RLF main header
 * @file	rlf.h
 * @package RLF
 * @license GPL
 * <jtm@robot.is>
 +-----------------------------------------------------------+
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>	// memcpy
#include <time.h>
// includes
#include "list_t.h"
#include "defines.h"

// types
typedef int err;
typedef struct {
	unsigned int width;
	unsigned int height;
	unsigned int cellcnt;
	unsigned int mnum;
	unsigned short *cells;
	int * path_map[RLFL_MAX_MAPS];
} RLFL_map_t;

typedef struct {
	unsigned int X;
	unsigned int Y;
} RLFL_step_t;

typedef struct {
	/* Origin */
	unsigned int ox, oy;

	/* Destination */
	unsigned int dx, dy;

	/* Bounds */
	unsigned int w, h;

	/* computed path */
	RLFL_list_t path;

	/* path size */
	int size;

	/* Valid */
	bool valid;

	/* Map for path */
	unsigned int map;
} RLFL_path_t;

/* Map */
extern RLFL_map_t * RLFL_map_store[];
extern int RLFL_new_map(unsigned int w, unsigned int h);
extern err RLFL_wipe_map(unsigned int m);
extern void RLFL_wipe_all(void);
extern bool RLFL_cell_valid(unsigned int m, unsigned int x, unsigned int y);
extern bool RLFL_map_valid(unsigned int m);
extern err RLFL_map_size(unsigned int m, unsigned int *w, unsigned int *h);
extern err RLFL_translate_xy(unsigned int m, int i, unsigned int *x, unsigned int *y);
/* Flags */
extern err RLFL_set_flag(unsigned int m, unsigned int x, unsigned int y, unsigned short flag);
extern int RLFL_has_flag(unsigned int m, unsigned int x, unsigned int y, unsigned short flag);
extern err RLFL_clear_flag(unsigned int m, unsigned int x, unsigned int y, unsigned short flag);
extern err RLFL_clear_map(unsigned int m, unsigned short flag);
extern err RLFL_fill_map(unsigned int m, unsigned short flag);
extern int RLFL_get_flags(unsigned int m, unsigned int x, unsigned int y);
/* Random */
extern int RLFL_randint(int limit);
extern int RLFL_randrange(int min, int max);
extern int RLFL_randspread(int origin, int range);
/* LOS */
extern err RLFL_los(unsigned int map, unsigned int y1, unsigned int x1, unsigned int y2, unsigned int x2);
/* Path */
extern RLFL_path_t * RLFL_path_store[];
extern err RLFL_path_create(unsigned int m, unsigned int ox, unsigned int oy, unsigned int dx, unsigned int dy,
						    unsigned int algorithm, int range, unsigned int flags, float dcost);
extern int RLFL_path_size(unsigned int p);
extern err RLFL_path_delete(unsigned int p);
extern err RLFL_path_step(unsigned int p, unsigned int s, unsigned int *x, unsigned int *y);
extern err RLFL_path_basic(unsigned int m, unsigned int ox, unsigned int oy, unsigned int dx, unsigned int dy,
						  int range, unsigned int flags);
extern err RLFL_path_astar(unsigned int m, unsigned int ox, unsigned int oy, unsigned int dx, unsigned int dy,
						  int range, unsigned int flags, float dcost);
extern err RLFL_path_fill_map(unsigned int m, unsigned int x, unsigned int y, float dcost);
extern err RLFL_path_wipe_map(unsigned int m, unsigned int p);
extern err RLFL_path_wipe_all_maps(unsigned int m);
extern err RLFL_path_step_map(unsigned int m, unsigned int p, unsigned int ox, unsigned int oy,
							  unsigned int *x, unsigned int *y, bool away);
/* Utility */
extern int RLFL_distance(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
extern err RLFL_scatter(unsigned int m, unsigned int ox, unsigned int oy, unsigned int *dx, unsigned int *dy,
					    int range, unsigned int flag, bool need_los);
/* FOV */
extern err RLFL_fov(unsigned int m, unsigned int ox, unsigned int oy, unsigned int radius,
				   unsigned int algorithm, bool light_walls);
extern err RLFL_fov_finish(unsigned int m, int x0, int y0, int x1, int y1, int dx, int dy);
extern err RLFL_fov_circular_raycasting(unsigned int m, unsigned int ox, unsigned int oy, unsigned int radius,
									   bool light_walls);
extern err RLFL_fov_diamond_raycasting(unsigned int m, unsigned int ox, unsigned int oy, unsigned int radius,
									  bool light_walls);
extern err RLFL_fov_recursive_shadowcasting(unsigned int m, unsigned int ox, unsigned int oy, int radius,
										   bool light_walls);
extern err RLFL_fov_digital(unsigned int m, unsigned int ox, unsigned int oy, int radius, bool light_walls);
extern err RLFL_fov_permissive(unsigned int m, unsigned int ox, unsigned int oy, unsigned int radius,
							  bool light_walls);
extern err RLFL_fov_restrictive_shadowcasting(unsigned int m, unsigned int ox, unsigned int oy, int radius,
							  bool light_walls);
/* Project */
extern RLFL_list_t * RLFL_project_store[];
extern err RLFL_project_delete(int p);
extern unsigned int RLFL_project_size(int p);
extern err RLFL_project_step(int p, int i, unsigned int *x, unsigned int *y);
extern err RLFL_project(unsigned int m, unsigned int ox, unsigned int oy, unsigned int tx, unsigned int ty,
						int rad, int range, unsigned short flg);
extern err RLFL_project_bolt(unsigned int m, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,
							 int range, unsigned int flags);
extern err RLFL_project_ball(unsigned int m, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,
							 unsigned int rad, int range, unsigned int flags);
extern err RLFL_project_beam(unsigned int m, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,
							 int range, unsigned int flags);
extern err RLFL_project_wave(unsigned int m, unsigned int x1, unsigned int y1, unsigned int rad, int range,
							 unsigned int flags);
extern err RLFL_project_cone(unsigned int m, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,
							 unsigned int rad, int range, unsigned int flags);
extern err RLFL_project_cloud(unsigned int m, unsigned int x1, unsigned int y1, unsigned int rad,
							  unsigned int flags);
/* DEBUG */
extern void RLFL_DEBUG_print_path_map(unsigned int m, unsigned int p);
