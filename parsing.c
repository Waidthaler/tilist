#include "parsing.h"

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
// Given the contents of the config file returned by load_file, attempts to
// parse the JSON contained therein. On success, returns a pointer to a cJSON
// object; on error, returns NULL. Automatically deallocates the buffer returned
// by load_file.
//==============================================================================

cJSON *parse_config(char *buf) {
    cJSON *json;
    cJSON *cur;
    cJSON *sub;
    const char *error_ptr;
    int i;

    json = cJSON_parse(buf);

    if(json == NULL) {
        error_ptr = cJSON_GetErrorPtr();
        if(error_ptr != NULL) {
            fprintf(stderr, "Error in config file before: %s\n", error_ptr);
        }
        return NULL;
    }
    free(buf);

    // Parse and validate background ===========================================

    cur = cJSON_GetObjectItemCaseSensitive(json, "background");
    if(cur == NULL) {
        fprintf(stderr, "Missing background entry in config file.\n");
        return NULL;
    }

    // background.width --------------------------------------------------------

    sub = cJSON_GetObjectItemCaseSensitive(cur, "width");
    if(sub == NULL) {
        fprintf(stderr, "Missing background.width entry in config file.\n");
        return NULL;
    }
    if(!cJSON_isNumber(sub) || sub.valueint < 1) {
        fprintf(stderr, "Malformed background.width in config file, must be a positive integer.\n");
        return NULL;
    }

    config.image_width = sub.valueint;

    // background.height -------------------------------------------------------

    sub = cJSON_GetObjectItemCaseSensitive(cur, "height");
    if(sub == NULL) {
        fprintf(stderr, "Missing background.height entry in config file.\n");
        return NULL;
    }
    if(!cJSON_isNumber(sub) || sub.valueint < 1) {
        fprintf(stderr, "Malformed background.height in config file, must be a positive integer.\n");
        return NULL;
    }

    config.image_height = sub.valueint;

    // background.bgcolor ------------------------------------------------------

    sub = cJSON_GetObjectItemCaseSensitive(cur, "bgcolor");
    if(sub == NULL) {
        fprintf(stderr, "Missing background.bgcolor entry in config file.\n");
        return NULL;
    }
    if(strlen(sub.valuestring) == 7 && sub.valuestring[0] == '#') {
        for(i = 1; i < 8; i++) {
            if(!isxdigit(sub.valuestring[i])) {
                fprintf(stderr, "Malformed background.bgcolor entry, must be in the form '#xxxxxx', where 'x' represents a hexadecimal digit.\n");
                return NULL;
            }
        }
    } else {
        fprintf(stderr, "Malformed background.bgcolor entry, must be in the form '#xxxxxx'.\n");
        return NULL;
    }

    config.image.bgcolor.a = 0xFF;
    parseHexTriplet(sub.valuestring);

    // background.image --------------------------------------------------------

    sub = cJSON_GetObjectItemCaseSensitive(cur, "image");
    if(sub != NULL) {
        config.image.bg_image_filename = malloc(strlen(sub.valuestring) + 1);
        strcpy(config.image.bg_image_filename, sub.valuestring);
    }

    // Parse and validate directions -------------------------------------------

    // Parse and validate tiles ------------------------------------------------

    // Parse and validate vertices ---------------------------------------------

}


//==============================================================================
// Given a string of the form "#xxxxxx", sets the corresponding RGB values in
// the Pixel.
//==============================================================================

void parseHexTriplet(char *triplet, Pixel *p) {
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

