#include <stdio.h>
#include <stdlib.h>

#include "utility.h"

void HandlePthreadError(char* message, int code) {
    fprintf(stderr, "Error! %s: %i\n", message, code);
    exit(EXIT_FAILURE);
}