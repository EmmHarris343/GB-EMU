#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

extern FILE *log_dump_file;
extern FILE *log_sml;


int logging_init(const char *filename);
void logging_close();
void logging_log(const char *format, ...);



#endif