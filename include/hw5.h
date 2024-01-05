#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_LINE 200
#define MAX_SEARCH_LEN 30

#define MISSING_ARGUMENT 1
#define INPUT_FILE_MISSING 2
#define OUTPUT_FILE_UNWRITABLE 3
#define S_ARGUMENT_MISSING 4
#define R_ARGUMENT_MISSING 5
#define L_ARGUMENT_INVALID 6
#define WILDCARD_INVALID 7
#define DUPLICATE_ARGUMENT 8
