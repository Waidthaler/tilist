#ifndef PARSING_H
#define PARSING_H

#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "tilist.h"

extern struct Config config;

// Prototypes ==================================================================

char *beginning_of_data(char *line);
void clean_line(char *line);
bool parse_image_line(char *line, int line_number);
bool parse_directions_line(char *line, int line_number);
bool parse_rgb_hex(char *rgb, Pixel *pixel);
bool parse_tile_line(char *line, int line_number);
bool parse_vertex_line(char *line, int line_number);
bool partial_line(char *line);
bool streq(char *a, char *b);
bool streqn(char *a, char *b, int n);

#endif // PARSING_H
