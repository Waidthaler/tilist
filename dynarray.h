#ifndef DYNARRAY_H

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

//##############################################################################
//# Implements a dynamic array of pointers.
//##############################################################################

typedef struct {
	uint32_t nmemb;    // number of members
    uint32_t used;     // member slots full
	void **ary;        // pointer to beginning of array of pointers
} Dynarray;


void *dynarray_create(Dynarray *hdr);
void *dynarray_resize(Dynarray *hdr, uint32_t nmemb);
int dynarray_push(Dynarray *hdr, void *item);
void *dynarray_pop(Dynarray *hdr);
void *dynarray_shift(Dynarray *hdr);
int dynarray_unshift(Dynarray *hdr, void *item);
void dynarray_free(Dynarray *hdr);

#endif // DYNARRAY_H
