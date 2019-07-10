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

    bres = init(argv[1]);
    if(!bres)
        return 1;


    return 0;
}


//==============================================================================
// Initial setup of data structures.
//==============================================================================

int init(char *fname) {
    bool result;

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

    result = parse_config(fname);

    return 0;
}

/*


{
    background: {
        height: 800,
        width:  600,
        bgcolor: "#FFFFFF",
        image:   "/foo/bar/somefile.png"      // optional
    },
    directions: [ "N", "E", "S", "W" ],       // assumed to be clockwise
    tiles: {
        blinky: {
            sides: [
                {
                    direction:   "N",
                    label:       1231,
                    matchAny:    false,  // false is the default for all of these; can be omitted
                    endcap:      false,
                    matchLabels: [ 2131, 13221, 12 ],
                    matchAnyOf:  981269,
                    matchAllOf:  false,
                    matchNoneOf: false,
                },
                ...
            ],
            spritesheet: "/foo/bar/sprites1.png",
            tileHeight: 20,
            tileWidth:  20,
            centerX:    10,
            centerY:    10,
        },
        pinky: { },
        inky:  { },
        clyde: { },
        ...
    },
    vertices: {
        foo: {
            order: 1,
            eligibleTiles: [ "blinky", "pinky", "inky", "clyde" ],  // or null for all
            neighbors: {
                N: "bar",
                E: "baz",
                S: "quux",
                W: "murgatroyd"
            },
            centerX: 100,
            centerY: 227
        },
        ...
        },
}



*/

//==============================================================================
// Loads the specified file and attempts to parse the JSON contained therein.
// Returns boolean success/true or failure/false. Automatically deallocates the
// buffer returned by load_file.
//==============================================================================

bool parse_config(char *fname) {
    char *buf;
    cJSON *json;
    cJSON *cur;
    cJSON *sub;
    const char *error_ptr;
    int i, j, cnt;
    char *newstr;

    buf = load_file(fname);
    if(!buf)
        return false;

    json = cJSON_parse(buf);

    if(json == NULL) {
        error_ptr = cJSON_GetErrorPtr();
        if(error_ptr != NULL) {
            fprintf(stderr, "Error in config file before: %s\n", error_ptr);
        }
        return false;
    }
    free(buf);

    // Parse and validate background ===========================================

    cur = cJSON_GetObjectItemCaseSensitive(json, "background");
    if(cur == NULL) {
        fprintf(stderr, "Missing background entry in config file.\n");
        return false;
    }

    // background.width --------------------------------------------------------

    sub = cJSON_GetObjectItemCaseSensitive(cur, "width");
    if(sub == NULL) {
        fprintf(stderr, "Missing background.width entry in config file.\n");
        return false;
    }
    if(!cJSON_isNumber(sub) || sub.valueint < 1) {
        fprintf(stderr, "Malformed background.width in config file, must be a positive integer.\n");
        return false;
    }

    config.image_width = sub.valueint;

    // background.height -------------------------------------------------------

    sub = cJSON_GetObjectItemCaseSensitive(cur, "height");
    if(sub == NULL) {
        fprintf(stderr, "Missing background.height entry in config file.\n");
        return false;
    }
    if(!cJSON_isNumber(sub) || sub.valueint < 1) {
        fprintf(stderr, "Malformed background.height in config file, must be a positive integer.\n");
        return false;
    }

    config.image_height = sub.valueint;

    // background.bgcolor ------------------------------------------------------

    sub = cJSON_GetObjectItemCaseSensitive(cur, "bgcolor");
    if(sub == NULL) {
        fprintf(stderr, "Missing background.bgcolor entry in config file.\n");
        return false;
    }
    if(strlen(sub.valuestring) == 7 && sub.valuestring[0] == '#') {
        for(i = 1; i < 8; i++) {
            if(!isxdigit(sub.valuestring[i])) {
                fprintf(stderr, "Malformed background.bgcolor entry, must be in the form '#xxxxxx', where 'x' represents a hexadecimal digit.\n");
                return false;
            }
        }
    } else {
        fprintf(stderr, "Malformed background.bgcolor entry, must be in the form '#xxxxxx'.\n");
        return false;
    }

    config.image.bgcolor.a = 0xFF;
    parse_hex_triplet(sub.valuestring);

    // background.image --------------------------------------------------------

    sub = cJSON_GetObjectItemCaseSensitive(cur, "image");
    if(sub != NULL) {
        config.image.bg_image_filename = malloc(strlen(sub.valuestring) + 1);
        strcpy(config.image.bg_image_filename, sub.valuestring);
    }

    // Parse and validate directions ===========================================

    cur = cJSON_GetObjectItemCaseSensitive(json, "background");
    if(cur == NULL) {
        fprintf(stderr, "Missing background entry in config file.\n");
        return false;
    }
    if(!cJSON_IsArray(cur)) {
        fprintf(stderr, "The directions element in the config file must be an array.\n");
        return false;
    }

    cnt = cJSON_GetArraySize(cur);
    if(!cnt || cnt > 32) {
        fprintf(stderr, "The directions array must have 1-32 entries.\n");
        return false;
    }

    // Load directions into Config struct --------------------------------------

    dynarray_push(Config.dir, NULL);  // empty placeholder value for 0

    for(i = 0; i < cnt; i++) {
        sub = cJSON_GetArrayItem(cur, i);
        newstr = malloc(strlen(sub.valuestring + 1));
        strcpy(newstr, sub.valuestring);
        dynarray_push(Config.dir, newstr);
    }

    // Check for duplicates ----------------------------------------------------

    for(i = 1; i < cnt; i++) {
        for(j = i + 1; j <= cnt; i++) {
            if(streqn(Config.dir.ary[i], Config.dir.ary[j])) {
                fprintf(stderr, "Duplicate directions '%s' and '%s' in config file.\n",
                    Config.dir.ary[i], Config.dir.ary[j]);
                return false;
            }
        }
    }

    // Parse and validate tiles ================================================

    // Parse and validate vertices =============================================

}


//==============================================================================
// Given a direction name string, returns its offset or 0 if not found.
//==============================================================================

int get_dir_offset(char *name) {
    int i;

    for(i = 1; i <= Config.dir.used; i++) {
        if(streqn(config.dir.ary[i], name))
            return i;
    }
    return 0;
}


//==============================================================================
// Given a string of the form "#xxxxxx", sets the corresponding RGB values in
// the Pixel.
//==============================================================================

void parse_hex_triplet(char *triplet, Pixel *p) {
    char buf[3] = { ' ', ' ', '\0');

    buf[0] = triplet[1];
    buf[1] = triplet[2];
    p->r = (uint8_t)strtol(buf, NULL, 16);

    buf[0] = triplet[3];
    buf[1] = triplet[4];
    p->g = (uint8_t)strtol(buf, NULL, 16);

    buf[0] = triplet[5];
    buf[1] = triplet[6];
    p->b = (uint8_t)strtol(buf, NULL, 16);

    return;
}


//==============================================================================
// Attempts to open the supplied filename and read its contents into a buffer.
// Returns NULL on error. On success, returns a pointer to the buffer, which is
// dynamically allocated and must be freed.
//==============================================================================

char *load_file(char *fname) {
    char  *buf;
    FILE  *fp;
    size_t bytes_read;
    size_t bufsize = 0xFFFF;

    buf = malloc(bufsize)
    if(!buf)
        return NULL;

    fp = fopen(fname, "r");
    if(!fp)
        return NULL;

    bytes_read = fread(buf, sizeof(char), bufsize, fp);
    while(feof(fp)) {
        bufsize *= 2;
        buf = realloc(buf, bufsize);
        bytes_read += fread(buf + bytes_read, sizeof(char), bufsize / 2, fp);
    }
    fclose(fp);

    return buf;
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






