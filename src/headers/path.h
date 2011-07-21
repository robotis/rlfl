/* Structure for path elements */
typedef struct path_element {
    int x;
    int y;
    int cost;
    int estimate;
    unsigned int state;
    struct path_element* parent;
} path_element;

/* Dijkstra grid */
typedef struct {
	/* Stack of unprocessed nodes */
	path_element ** open;

	/* map of all nodes */
	path_element * nodes;

	/* */
	int top;

	/* Diagonal cost */
	float dcost;

	/* Use estimates */
	bool astar;

	unsigned int cx, cy;
} path_int_t;
path_int_t* PATH;

#define STATE_EMPTY		0
#define STATE_OPEN		1
#define STATE_CLOSED	2

extern void delete_path(void);
