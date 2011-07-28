/*
	RLFL Generic list

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

static void RLFL_list_allocate_int(RLFL_list_t l);

/*
 +-----------------------------------------------------------+
 * @desc	Create new empty list
 +-----------------------------------------------------------+
 */
RLFL_list_t
RLFL_list_create(void) {
	return (RLFL_list_t)calloc(1, sizeof(list_t));
}
/*
 +-----------------------------------------------------------+
 * @desc	Create new empty list of a certain size
 +-----------------------------------------------------------+
 */
RLFL_list_t
RLFL_list_create_size(const int size) {
	RLFL_list_t l = RLFL_list_create();
	if(size <= 1) return l;
	LIST(l)->data = (void **)calloc(sizeof(void *), size);
	LIST(l)->asize = size;
	return l;
}
/*
 +-----------------------------------------------------------+
 * @desc	Append element to list
 +-----------------------------------------------------------+
 */
void
RLFL_list_append(RLFL_list_t l, const void * element) {
	if(!l) return;
	if (LIST(l)->fsize+1 >= LIST(l)->asize)
		RLFL_list_allocate_int(l);
	LIST(l)->data[LIST(l)->fsize++] = (void *)element;
}
/*
 +-----------------------------------------------------------+
 * @desc	Pop item off list
 +-----------------------------------------------------------+
 */
void *
RLFL_list_pop(RLFL_list_t l) {
	if(!l) return NULL;
	if(LIST(l)->fsize >= 0)
		return NULL;
	return LIST(l)->data[--(LIST(l)->fsize)];
}
/*
 +-----------------------------------------------------------+
 * @desc	Extend list with other list
 +-----------------------------------------------------------+
 */
void
RLFL_list_extend(RLFL_list_t l, RLFL_list_t l2) {
	if(!l || !l2)
		return;
	void **i;
	for (i = RLFL_list_begin(l2); i != RLFL_list_end(l2); i++) {
		RLFL_list_append(l, *i);
	}
}
/*
 +-----------------------------------------------------------+
 * @desc	Get element at location
 +-----------------------------------------------------------+
 */
void *
RLFL_list_get(RLFL_list_t l, unsigned int index) {
	if(!l || index > (LIST(l)->fsize))
		return NULL;
	return LIST(l)->data[index];
}
/*
 +-----------------------------------------------------------+
 * @desc	Set element at location
 +-----------------------------------------------------------+
 */
void
RLFL_list_set(RLFL_list_t l, unsigned int index, const void * element) {
	if(!l) return;
	while(LIST(l)->asize < index + 1)
		RLFL_list_allocate_int(l);
	LIST(l)->data[index] = (void *)element;
	if (index + 1 > LIST(l)->fsize)
		LIST(l)->fsize = index+1;
}
/*
 +-----------------------------------------------------------+
 * @desc	Return pointer to first element
 +-----------------------------------------------------------+
 */
void **
RLFL_list_begin(RLFL_list_t l) {
	if(!l) return NULL;
	if (LIST(l)->fsize == 0)
		return (void **)NULL;
	return &LIST(l)->data[0];
}
/*
 +-----------------------------------------------------------+
 * @desc	Return pointer to last element
 +-----------------------------------------------------------+
 */
void **
RLFL_list_end(RLFL_list_t l) {
	if(!l) return NULL;
	if (LIST(l)->fsize == 0)
		return (void **)NULL;
	return &LIST(l)->data[LIST(l)->fsize];
}
/*
 +-----------------------------------------------------------+
 * @desc	Insert element before index
 +-----------------------------------------------------------+
 */
void **
RLFL_list_insert(RLFL_list_t l, const void *element, int before) {
	int idx;
	if(!l) return NULL;
	if (LIST(l)->fsize + 1 >= LIST(l)->asize)
		RLFL_list_allocate_int(l);
	for (idx=LIST(l)->fsize; idx > before; idx--) {
		LIST(l)->data[idx] = LIST(l)->data[idx-1];
	}
	LIST(l)->data[before] = (void *)element;
	LIST(l)->fsize++;
	return &LIST(l)->data[before];
}
/*
 +-----------------------------------------------------------+
 * @desc	List size
 +-----------------------------------------------------------+
 */
int
RLFL_list_size(RLFL_list_t l) {
	if(!l) return 0;
	return LIST(l)->fsize;
}
/*
 +-----------------------------------------------------------+
 * @desc	Empty list
 +-----------------------------------------------------------+
 */
void
RLFL_list_empty(RLFL_list_t l) {
	if(!l) return;
	LIST(l)->fsize=0;
}
/*
 +-----------------------------------------------------------+
 * @desc	Delete list
 +-----------------------------------------------------------+
 */
void
RLFL_list_delete(RLFL_list_t l)
{
	if(!l) return;
	if(LIST(l)->data) free(LIST(l)->data);
	free(l);
}
/*
 +-----------------------------------------------------------+
 * @desc	FIXME
 +-----------------------------------------------------------+
 */
void **
RLFL_list_remove_iterator(RLFL_list_t l, void ** element) {
	void **e;
	if(!l || !element)
		return NULL;
	for(e = element; e < RLFL_list_end(l) - 1; e++) {
		*e = *(e+1);
	}
	LIST(l)->fsize--;
	if(LIST(l)->fsize == 0)
		return ((void **)NULL)-1;
	return element - 1;
}
/*
 +-----------------------------------------------------------+
 * @desc	"Grow" list, doubleing its size
 +-----------------------------------------------------------+
 */
static void
RLFL_list_allocate_int(RLFL_list_t l) {
	void **new;
	int nsize = LIST(l)->asize * 2;
	if (nsize == 0) nsize = 16;
	new = (void **)calloc(sizeof(void *), nsize);
	if (LIST(l)->data) {
		if (LIST(l)->fsize > 0)
			memcpy(new, LIST(l)->data, sizeof(void *)*LIST(l)->fsize);
		free(LIST(l)->data);
	}
	LIST(l)->asize = nsize;
	LIST(l)->data = new;
}


