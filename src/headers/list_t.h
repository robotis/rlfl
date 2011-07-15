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
typedef void *RLF_list_t;

#define LIST(l) ((list_t *)l)

extern RLF_list_t RLF_list_create(void);
extern RLF_list_t RLF_list_create_size(const int size);
extern void RLF_list_append(RLF_list_t l, const void * element);
extern void RLF_list_extend(RLF_list_t l, RLF_list_t l2);
extern void RLF_list_empty(RLF_list_t l);
extern void RLF_list_delete(RLF_list_t l);
extern int RLF_list_size(RLF_list_t l);
extern void RLF_list_set(RLF_list_t l, unsigned int index, const void * element);
extern void * RLF_list_get(RLF_list_t l, unsigned int index);
extern void * RLF_list_pop(RLF_list_t l);
extern void ** RLF_list_begin(RLF_list_t l);
extern void ** RLF_list_end(RLF_list_t l);
extern void ** RLF_list_remove_iterator(RLF_list_t l, void ** element);
extern void ** RLF_list_insert(RLF_list_t l, const void * element, int before);
