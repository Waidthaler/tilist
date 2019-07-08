#ifndef PARSING_H
#define PARSING_H

#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "tilist.h"
#include "cJSON.h"

extern struct Config config;

// Prototypes ==================================================================

char *load_file(char *fname)
bool  partial_line(char *line);
bool  streq(char *a, char *b);
bool  streqn(char *a, char *b, int n);

#endif // PARSING_H
