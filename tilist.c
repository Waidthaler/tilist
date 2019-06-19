#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include "dynarray.h"
#include "tilist.h"

struct {               // global config structure
    Dynarray dir;          // direction array
    Dynarray vert;         // vertex array
    Dynarray tile;         // tile master array
} config;



int main(int argc, char **argv) {
    pre_init();
    return 0;
}


int pre_init() {
    config.dir.nmemb = 32;
    if(!dynarray_create(&config.dir)) {
        printf("Unable to initialize config.dir.\n");
        abort();
    }
}


