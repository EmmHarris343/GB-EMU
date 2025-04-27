#include "debug.h"
#include <stdarg.h>

FILE *debug_dump_file = NULL;
FILE *debug_sml = NULL;


int debug_init(const char *filename) {
    debug_dump_file = fopen(filename, "w");
    if (!debug_dump_file) {
        printf("[ERROR] Failed to open debug log file!\n");
        return -1;
    }
    return 0;
}


void debug_close() {
    if (debug_dump_file) {
        fclose(debug_dump_file);
        debug_dump_file = NULL;
    }
}

void debug_log(const char *format, ...) {
    if (!debug_dump_file) return;

    va_list args;
    va_start(args, format);
    vfprintf(debug_dump_file, format, args);
    va_end(args);

    fflush(debug_dump_file);
}