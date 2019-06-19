#include "dynarray.h"

//##############################################################################
//# Implements a dynamic array of pointers.
//##############################################################################


//==============================================================================
// Takes a pointer to a Dynarray struct with nmemb already set, allocates the
// necessary space, and sets ary to point to it. Returns hdr->ary, which will be
// NULL if allocation failed.
//==============================================================================

void *dynarray_create(Dynarray *hdr) {
	hdr->ary = calloc(hdr->nmemb, sizeof(void *));
	if(hdr->ary)
		hdr->used = 0;
	return hdr->ary;
};


//==============================================================================
// Attempts to resize the array to nmemb units. (In general, this should be
// bigger rather than smaller, but both work.) Setting nmemb equal to hdr->nmemb
// * 2 is a good general starting point. Returns the new ary on success, NULL
// on failure.
//==============================================================================

void *dynarray_resize(Dynarray *hdr, uint32_t nmemb) {
	void *new_ary = realloc(hdr->ary, sizeof(void *) * nmemb);
	if(new_ary) {
		hdr->nmemb = nmemb;
		hdr->ary = new_ary;
	}
	return new_ary;
}


//==============================================================================
// Pushes a new item onto the array, resizing by 2x as needed. Returns 0 on
// memory allocation failure, 1 on success.
//==============================================================================

int dynarray_push(Dynarray *hdr, void *item) {
    if(hdr->used == hdr->nmemb) {
        if(!dynarray_resize(hdr, hdr->nmemb * 2))
            return 0;
    }
    hdr->ary[hdr->used] = item;
    hdr->used++;
    return 1;
}


//==============================================================================
// Pops an item off the end of the array, returning a pointer to it or NULL if
// the array is empty.
//==============================================================================

void *dynarray_pop(Dynarray *hdr) {
    if(!hdr->used)
        return NULL;
    hdr->used--;
    return hdr->ary[hdr->used];
}


//==============================================================================
// Shifts an item off the beginning of the array, returning a pointer to it or
// NULL if the array is empty.
//==============================================================================

void *dynarray_shift(Dynarray *hdr) {
    void *result;

    if(!hdr->used)
        return NULL;
    result = hdr->ary[0];
    hdr->used--;
    memmove(hdr->ary, hdr->ary + 1, hdr->used * sizeof(void *));

    return result;
}


//==============================================================================
// Unshifts an item onto the beginning of the array, resizing by 2x as needed.
// Returns 0 on memory allocation failure, 1 on success.
//==============================================================================

int dynarray_unshift(Dynarray *hdr, void *item) {
    if(hdr->used == hdr->nmemb) {
        if(!dynarray_resize(hdr, hdr->nmemb * 2))
            return 0;
    }
    memmove(hdr->ary + 1, hdr->ary, hdr->used * sizeof(void *));
    hdr->ary[0] = item;
    hdr->used++;
    return 1;
}


//==============================================================================
// Frees the allocated storage. Doesn't do anything with the hdr structure.
//==============================================================================

void dynarray_free(Dynarray *hdr) {
	free(hdr->ary);
}

