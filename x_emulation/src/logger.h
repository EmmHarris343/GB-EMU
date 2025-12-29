#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <stdint.h>


extern FILE *log_dump_file;
extern FILE *log_sml;

extern FILE *cpu_trace_file;
extern FILE *trace_log_file;

int logging_init(const char *filename);
void logging_close();
void logging_log(const char *format, ...);


int cpu_trace_init(const char *filename);
void cpu_trace_close();
void logging_cpu_trace(const char *format, ...);


int trace_log_init(const char *filename);
void trace_log_close();
void logging_trace_log(const char *format, ...);

void trace_mmu_read(uint16_t addr, uint8_t val, int map_index, uint8_t src_tag);
void trace_mmu_write(uint16_t addr, uint8_t val, int map_index, uint8_t src_tag);


#endif