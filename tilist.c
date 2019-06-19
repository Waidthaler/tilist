#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include "dynarray.h"
#include "tilist.h"

struct {               // global config structure
    Dynarray  dir;          // direction array
    Dynarray  vert;         // vertex array
    Dynarray  tile;         // tile master array
    uint16_t  image_width;
    uint16_t  image_height;
    Pixel     bgcolor;
    char     *output_png_name;
} config;


//==============================================================================
// Main loop.
//==============================================================================

int main(int argc, char **argv) {
    pre_init();
    return 0;
}


//==============================================================================
// Initial setup of data structures.
//==============================================================================

int pre_init() {
    config.dir.nmemb = 32;
    if(!dynarray_create(&config.dir)) {
        printf("Unable to initialize config.dir.\n");
        abort();
    }
    config.vert.nmemb = 256;
    if(!dynarray_create(&config.vert)) {
        printf("Unable to initialize config.vert.\n");
        abort();
    }
    config.tile.nmemb = 256;
    if(!dynarray_create(&config.tile)) {
        printf("Unable to initialize config.tile.\n");
        abort();
    }

    // Defaults

    config.image_width  = 800;
    config.image_height = 600;

    config.bgcolor.r    = 0;
    config.bgcolor.g    = 0;
    config.bgcolor.b    = 0;
    config.bgcolor.a    = 0;

    return 0;
}


//==============================================================================
// Returns a boolean indicating whether the two strings supplied are identical.
//==============================================================================

bool streq(char *a, char *b) {
    return strcmp(a, b) ? false : true;
}


//==============================================================================
// Returns a boolean indicating whether the two strings supplied are identical
// up to the first n bytes. If either string terminates before the nth
// character, the result is false.
//==============================================================================

bool streqn(char *a, char *b, int n) {
    int i;

    for(i = 0; i < n; i++) {
        if(a[i] == '\0' || b[i] == '\0' || a[i] != b[i])
            return false;
    }
    return true;
}


//==============================================================================
//==============================================================================

int load_definitions(char *filename) {

}



//==============================================================================
//==============================================================================


/*

@IMAGE: h, w, bgcolor, bgimage
@DIRECTIONS: N, E, S, W     // which translates to 1, 2, 3, 4 (zero is reserved)
@TILE: [...]
@VERTEX: order, x, y, N:n1;E:n2;W:n3;S:n4, tiletype:orientation



*/
