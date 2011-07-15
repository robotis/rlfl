/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 * @file	path.h
 * @package FIXME
 * @license FIXME
 * <jtm@robot.is>
 +-----------------------------------------------------------+
 */
#include "list_t.h"

#define MAX_PATH	12

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

	/* step */
	unsigned int step;

	/* path size */
	int size;

	/* Map for path */
	unsigned int map;
} path_t;

extern path_t * path_store[];

