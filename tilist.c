#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include "dynarray.h"
#include "lodepng/lodepng.h"
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
// Main loop. The sole CLI argument is the filename of the config file.
//==============================================================================

int main(int argc, char **argv) {
    bool bres;

    if(argc != 2) {
        printf("FATAL ERROR: The config file must be the only command-line argument.\n");
        return 1;
    }

    pre_init();

    bres = load_definitions(argv[1]);
    if(!bres)
        return 1;

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
// Attempts to open and load the contents of the config file. Returns true on
// success and false on failure.
//==============================================================================

#define LBUFSIZE 1024

bool load_definitions(char *filename) {
    char *prefix[] = { "@VERTEX:", "@TILE:", "@IMAGE:", "@DIRECTIONS:" };
    char lbuf[LBUFSIZE];
    FILE *fp;
    int line_number = 0;

    fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("FATAL ERROR: Unable to open config file '%s'.\n", filename);
        return false;
    }

    do {
        lbuf = fgets(lbuf, LBUFSIZE, fp);
        line_number++;
        if(lbuf == NULL)
            break;
        if(partial_line(lbuf)) {
            printf("FATAL ERROR: Line %d exceeds limit of %d characters.", line_number, LBUFSIZE);
            return false;
        }
        if(!((streqn(prefix[0], lbuf, strlen(prefix[0]) && parse_vertex_line(lbuf)))
           || (streqn(prefix[1], lbuf, strlen(prefix[1]) && parse_tile_line(lbuf)))
           || (streqn(prefix[2], lbuf, strlen(prefix[2]) && parse_image_line(lbuf)))
           || (streqn(prefix[3], lbuf, strlen(prefix[3]) && parse_directions_line(lbuf)))) ) {
            printf("FATAL ERROR: Syntax error in line %d.\n", line_number);
            return false;
        }

    } while(1);

    if(lbuf == NULL) {
        fclose(fp);
        // we are done reading; perform final checks
    }

    return true;
}

#undef LBUFSIZE

/*

@IMAGE: h, w, bgcolor, bgimage
@DIRECTIONS: N, E, S, W     // which translates to 1, 2, 3, 4 (zero is reserved)
@TILE: [...]
@VERTEX: order, x, y, N:n1;E:n2;W:n3;S:n4, tiletype:orientation

*/


bool parse_image_line(char *line) {
    return true;
}

bool parse_directions_line(char *line) {
    return true;
}

bool parse_tile_line(char *line) {
    return true;
}

bool parse_vertex_line(char *line) {
    return true;
}


//==============================================================================
// Returns a boolean indicating whether all of the line was read into the
// buffer or not, i.e., it ends in a newline.
//==============================================================================

bool partial_line(char *line) {
    char *p;
    for(p = line; *p; p++);
    return p[-1] == '\n' ? false : true;
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


