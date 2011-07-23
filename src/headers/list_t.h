/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 * @file	llist_t.h
 * @package FIXME
 * @license FIXME
 * <jtm@robot.is>
 +-----------------------------------------------------------+
 */

typedef struct {
	int fsize;
	int asize;

	void **data;
} list_t;
typedef void *RLFL_list_t;

#define LIST(l) ((list_t *)l)

extern RLFL_list_t RLFL_list_create(void);
extern RLFL_list_t RLFL_list_create_size(const int size);
extern void RLFL_list_append(RLFL_list_t l, const void * element);
extern void RLFL_list_extend(RLFL_list_t l, RLFL_list_t l2);
extern void RLFL_list_empty(RLFL_list_t l);
extern void RLFL_list_delete(RLFL_list_t l);
extern int RLFL_list_size(RLFL_list_t l);
extern void RLFL_list_set(RLFL_list_t l, unsigned int index, const void * element);
extern void * RLFL_list_get(RLFL_list_t l, unsigned int index);
extern void * RLFL_list_pop(RLFL_list_t l);
extern void ** RLFL_list_begin(RLFL_list_t l);
extern void ** RLFL_list_end(RLFL_list_t l);
extern void ** RLFL_list_remove_iterator(RLFL_list_t l, void ** element);
extern void ** RLFL_list_insert(RLFL_list_t l, const void * element, int before);
