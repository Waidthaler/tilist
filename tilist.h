#ifndef TILIST_H
#define TILIST_H

#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynarray.h"
#include "lodepng/lodepng.h"
#include "cJSON.h"


typedef struct {    // Generic RGBA pixel type
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Pixel;

struct Config {                   // global config structure
    Dynarray  dir;                  // direction array
    Dynarray  vert;                 // vertex array
    Dynarray  tile;                 // tile master array
    int       image_width;
    int       image_height;
    Pixel     bgcolor;
    char     *bg_image_filename;
    char     *output_png_name;
};

typedef struct {         // Definition of lattice vertices
    int order;               // order in which vertices are visited
    int *eligible;           // array of eligible tiles or NULL for all
    int *neighbor;           // array of neighbors by direction, offset matches config.dir.ary
    int tile;                // index of tile master
    int orientation;         // index of orientation
    int x_offset;            // x coord in rendered graphic
    int y_offset;            // y coord in rendered graphic
} Vertex;

typedef struct {             // Definition of mating surface/side
    bool      direction;         // offset into config.dir.ary
    bool      match_any;         // if true, matches any tile
    bool      endcap;            // if true, can match no tile
    bool      match_labels;      // if true, matches any label in labels
    uint32_t  label;             // label for this surface
    uint32_t *labels;            // if not NULL, a null-terminated array of matching labels
    uint32_t  any_of;            // if match_any_of is true, will match any 1 bit in this bitmask
    uint32_t  all_of;            // if match_all_of is true, must match all 1 bits in this bitmask
    uint32_t  none_of;           // if match_none_of is true, must not match any 1 bit in this bitmask
} Surface;

typedef struct {         // Definition of tile masters
    char *name;              // human-readable tile name
    Surface *side;           // pointer to array of sides, offsets matching config.dir.ary
    char *filename;          // path to sprite sheet
    int width;               // width of sprite sheet
    int height;              // height of sprite sheet
    // TODO: bitmap
    int x_offset;            // x coord of tile center
    int y_offset;            // y coord of tile center
} Tile;


// Prototypes ==================================================================

bool  init(char *fname);
char *load_file(char *fname)
bool  partial_line(char *line);
bool  streq(char *a, char *b);
bool  streqn(char *a, char *b, int n);

#endif // TILIST_H
