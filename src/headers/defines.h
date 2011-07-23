/*
 +-----------------------------------------------------------+
 * @desc	RLFL various defines
 * @file	rlf.h
 * @package RLFL
 * @license GPL
 * <jtm@robot.is>
 +-----------------------------------------------------------+
 */
// defines
#ifndef RLFL_MAX_MAPS
#define RLFL_MAX_MAPS 12
#endif
#ifndef RLFL_MAX_PATHS
#define RLFL_MAX_PATHS 12
#endif
#ifndef RLFL_MAX_PROJECTS
#define RLFL_MAX_PROJECTS 12
#endif
#ifndef RLFL_MAX_RANGE
#define RLFL_MAX_RANGE 60
#endif
#ifndef RLFL_MAX_RADIUS
#define RLFL_MAX_RADIUS 60
#endif
#ifndef RLFL_MAX_WIDTH
#define RLFL_MAX_WIDTH 5000
#endif
#ifndef RLFL_MAX_HEIGHT
#define RLFL_MAX_HEIGHT 5000
#endif

#define RLFL_SUCCESS			0
#define RLFL_ERR_GENERIC		-1
#define RLFL_ERR_NO_MAP			-2
#define RLFL_ERR_FLAG 			-3
#define RLFL_ERR_NO_PATH		-4
#define RLFL_ERR_OUT_OF_BOUNDS 	-5
#define RLFL_ERR_NO_PROJECTION	-6
#define RLFL_ERR_SIZE			-7

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
#define CELL_FOV (CELL_SEEN|CELL_MEMO)
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
#define FOV_CIRCULAR		1
#define FOV_DIAMOND			2
#define FOV_SHADOW			3
#define FOV_DIGITAL			4
#define FOV_RESTRICTIVE		5
#define FOV_PERMISSIVE		6

/* Path algorithms */
#define PATH_BASIC			1
#define PATH_ASTAR			2

/* Access cell */
#define CELL(m, x, y) RLFL_map_store[m]->cells[x + (y * RLFL_map_store[m]->width)]
/* helpers */
#define ABS(a) ((a)<0?-(a):(a))
#define MAX(a,b) ((a)<(b)?(b):(a))
#define MIN(a,b) ((a)>(b)?(b):(a))
