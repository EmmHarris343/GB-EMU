#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

extern FILE *debug_dump_file;
extern FILE *debug_sml;


int debug_init(const char *filename);
void debug_close();
void debug_log(const char *format, ...);



#endif