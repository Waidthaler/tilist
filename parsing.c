#include "parsing.h"

/*


{
    background: {
        height: 800,
        width:  600,
        bgcolor: "#FFFFFF",
        image:   "/foo/bar/somefile.png"
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
    const char *error_ptr;

    json = cJSON_parse(buf);

    if(json == NULL) {
        error_ptr = cJSON_GetErrorPtr();
        if(error_ptr != NULL) {
            fprintf(stderr, "Error in config file before: %s\n", error_ptr);
        }
        return NULL;
    }

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

