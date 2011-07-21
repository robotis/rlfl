/*
 +-----------------------------------------------------------+
 * @desc	RLF main header
 * @file	rlf.h
 * @package RLF
 * @license FIXME
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

// defines
#ifndef MAX_MAPS
#define MAX_MAPS 12
#endif
#ifndef MAX_PATH
#define MAX_PATH 12
#endif
#ifndef MAX_PROJECT
#define MAX_PROJECT 12
#endif
#ifndef MAX_RANGE
#define MAX_RANGE 300
#endif

#define RLF_SUCCESS			0
#define RLF_ERR_GENERIC		-1
#define RLF_ERR_NO_MAP		-2
#define RLF_ERR_FLAG		-3

/* CELL flags */
#define CELL_NONE      		0x0000    /* No state */
#define CELL_DARK      		0x0001    /* Cell unknown */
#define CELL_OPEN      		0x0002    /* Cell does not block LOS */
#define CELL_VIEW      		0x0004    /* Cell seen, yet dark */
#define CELL_LIT       		0x0008    /* Cell lit */
#define CELL_WALK  			0x0010    /* Cell walkable */
#define CELL_MEMO  			0x0020    /* Cell memorized */
#define CELL_SEEN  			0x0040    /* Cell in fov */
#define CELL_ROOM  			0x0080    /* Cell in room */
#define CELL_GLOW  			0x0100    /* Self self illuminated */
#define CELL_PATH  			0x0200    /* Cell in path */
#define CELL_OCUP  			0x0400    /* Cell occupied */
#define CELL_REFL  			0x0800    /* Cell reflects bolts/beams */
#define CELL_PERM  			0x1000    /* Cell is permanent wall */
#define CELL_MASK (CELL_DARK|CELL_OPEN|CELL_VIEW|CELL_LIT|CELL_WALK|CELL_MEMO \
				   |CELL_SEEN|CELL_ROOM|CELL_GLOW|CELL_PATH|CELL_OCUP|CELL_REFL \
				   |CELL_PERM)

/* Projection */
#define PROJECT_NONE      	0x0000	/* No state */
#define PROJECT_BEAM		0x0001  /* Beam */
#define PROJECT_STOP		0x0002	/* Stop at first hit */
#define PROJECT_BALL		0x0004  /* Ball */
#define PROJECT_CONE		0x0008	/* Cone */
#define PROJECT_JUMP		0x0010
#define PROJECT_THRU		0x0020	/* Pass terrain features */
#define PROJECT_WAVE		0x0040

#define MAX_SIGHT			16
#define VINFO_MAX_SLOPES 	135

/* FOV algorithms */
#define FOV_BASIC			1
#define FOV_DIAMOND			2
#define FOV_SHADOW			3
#define FOV_DIGITAL			4
#define FOV_RESTRICTIVE		5
#define FOV_PERMISSIVE		6

/* Path algorithms */
#define PATH_BASIC			1
#define PATH_ASTAR			2

/* Access cell */
#define CELL(m, x, y) map_store[m]->cells[x + (y * map_store[m]->width)]
/* helpers */
#define ABS(a) ((a)<0?-(a):(a))
#define MAX(a,b) ((a)<(b)?(b):(a))
#define MIN(a,b) ((a)>(b)?(b):(a))

// types
typedef int err;
typedef struct {
	unsigned int width;
	unsigned int height;
	unsigned int cellcnt;
	unsigned int mnum;
	unsigned short *cells;
} map_t;

typedef struct {
	unsigned int X;
	unsigned int Y;
} step_t;

typedef struct {
	/* Origin */
	unsigned int ox, oy;

	/* Destination */
	unsigned int dx, dy;

	/* Bounds */
	unsigned int w, h;

	/* computed path */
	RLF_list_t path;

	/* path size */
	int size;

	/* Valid */
	bool valid;

	/* Map for path */
	unsigned int map;
} path_t;

/* Map */
extern map_t * map_store[];
extern int RLF_new_map(unsigned int w, unsigned int h);
extern err RLF_wipe_map(unsigned int m);
extern void RLF_wipe_all(void);
extern bool RLF_cell_valid(unsigned int m, unsigned int x, unsigned int y);
/* Flags */
extern err RLF_set_flag(unsigned int m, unsigned int x, unsigned int y, unsigned short flag);
extern int RLF_has_flag(unsigned int m, unsigned int x, unsigned int y, unsigned short flag);
extern err RLF_clear_flag(unsigned int m, unsigned int x, unsigned int y, unsigned short flag);
extern err RLF_clear_map(unsigned int m, unsigned short flag);
extern err RLF_fill_map(unsigned int m, unsigned short flag);
extern int RLF_get_flags(unsigned int m, unsigned int x, unsigned int y);
/* Random */
extern int RLF_randint(int limit);
extern int RLF_randrange(int min, int max);
extern int RLF_randspread(int origin, int range);
/* LOS */
extern err RLF_los(unsigned int map, unsigned int y1, unsigned int x1, unsigned int y2, unsigned int x2);
/* Path */
extern path_t * path_store[];
extern err RLF_path_create(unsigned int m, unsigned int ox, unsigned int oy, unsigned int dx, unsigned int dy,
						   unsigned int algorithm, int range, unsigned int flags, float dcost);
extern int RLF_path_size(unsigned int p);
extern err RLF_path_delete(unsigned int p);
extern err RLF_path_step(unsigned int p, unsigned int s, unsigned int *x, unsigned int *y);
extern err RLF_path_basic(unsigned int m, unsigned int ox, unsigned int oy, unsigned int dx, unsigned int dy,
						  int range, unsigned int flg);
extern err RLF_path_astar(unsigned int m, unsigned int ox, unsigned int oy, unsigned int dx, unsigned int dy,
						  int range, unsigned int flags, float dcost);
/* Utility */
extern int RLF_distance(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
extern err RLF_scatter(unsigned int m, unsigned int ox, unsigned int oy, unsigned int *dx, unsigned int *dy,
					   unsigned int range, unsigned int flag, bool need_los);
/* FOV */
extern err RLF_fov(unsigned int m, unsigned int ox, unsigned int oy, unsigned int radius,
				   unsigned int algorithm, bool light_walls);
extern err RLF_fov_finish(unsigned int m, int x0, int y0, int x1, int y1, int dx, int dy);
extern err RLF_fov_circular_raycasting(unsigned int m, unsigned int ox, unsigned int oy, unsigned int radius,
									   bool light_walls);
extern err RLF_fov_diamond_raycasting(unsigned int m, unsigned int ox, unsigned int oy, unsigned int radius,
									  bool light_walls);
extern err RLF_fov_recursive_shadowcasting(unsigned int m, unsigned int ox, unsigned int oy, int radius,
										   bool light_walls);
extern err RLF_fov_digital(unsigned int m, unsigned int ox, unsigned int oy, int radius, bool light_walls);
extern err RLF_fov_permissive(unsigned int m, unsigned int ox, unsigned int oy, unsigned int radius,
							  bool light_walls);
extern err RLF_fov_restrictive_shadowcasting(unsigned int m, unsigned int ox, unsigned int oy, int radius,
							  bool light_walls);
/* Project */
extern list_t * project_store[];
extern err RLF_project_delete(int p);
extern unsigned int RLF_project_size(int p);
extern err RLF_project_step(int p, int i, unsigned int *x, unsigned int *y);
extern err RLF_project(int m, int ox, int oy, int tx, int ty, int rad, int range, unsigned short flg);
extern err RLF_project_bolt(unsigned int m, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
extern err RLF_project_ball(unsigned int m, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int rad);
extern err RLF_project_beam(unsigned int m, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
extern err RLF_project_wave(unsigned int m, unsigned int x1, unsigned int y1, unsigned int rad);
extern err RLF_project_breath(unsigned int m, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int rad);
