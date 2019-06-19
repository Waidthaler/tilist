#ifndef TILIST_H

#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#include "dynarray.h"

typedef struct Vertex {  // Definition of lattice vertices
    int order;               // order in which vertices are visited
    int *eligible;           // array of eligible tiles or NULL for all
    int *neighbor;           // array of neighbors by direction
    int tile;                // index of tile master
    int orientation;         // index of orientation
    int x_offset;            // x coord in rendered graphic
    int y_offset;            // y coord in rendered graphic
} Vertex;

typedef struct {             // Definition of mating surface/side
    bool      orientation;       // offset into config.dir.ary
    bool      match_any;         // if true, matches any tile
    bool      endcap;            // if true, can match no tile
    bool      match_label;       // if true, matches only label
    bool      match_labels;      // if true, matches any label in labels
    bool      match_any_of;      // if true, matches any bit in any_of
    bool      match_all_of;      // if true, matches all bits in all_of
    bool      match_none_of;     // if true, cannot match anyt bit in none_of
    uint32_t  label;
    uint32_t *labels;
    uint32_t  any_of;
    uint32_t  all_of;
    uint32_t  none_of;
} Surface;

typedef struct {         // Definition of tile masters
    char *name;              // human-readable tile name
    Surface *side;           // pointer to array of sides
} Tile;


int pre_init();


#endif // TILIST_H
