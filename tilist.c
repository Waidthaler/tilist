#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

#include "dynarray.h"
#include "lodepng/lodepng.h"
#include "tilist.h"
#include "parsing.h"


struct Config config;


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
        fgets(lbuf, LBUFSIZE, fp);
        line_number++;
        if(lbuf == NULL)
            break;
        if(partial_line(lbuf)) {
            printf("FATAL ERROR: Line %d exceeds limit of %d characters.", line_number, LBUFSIZE);
            return false;
        }

        clean_line(lbuf);   // skip blank lines & comments
        if(!strlen(lbuf))
            continue;

        if(!((streqn(prefix[0], lbuf, strlen(prefix[0]) && parse_vertex_line(lbuf, line_number)))
           || (streqn(prefix[1], lbuf, strlen(prefix[1]) && parse_tile_line(lbuf, line_number)))
           || (streqn(prefix[2], lbuf, strlen(prefix[2]) && parse_image_line(lbuf, line_number)))
           || (streqn(prefix[3], lbuf, strlen(prefix[3]) && parse_directions_line(lbuf, line_number)))) ) {
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




