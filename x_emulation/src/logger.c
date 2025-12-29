#include "logger.h"
#include "trace_ids.h"
#include <stdarg.h>


FILE *debug_dump_file = NULL;
FILE *debug_sml = NULL;


FILE *cpu_trace_file = NULL;
FILE *trace_log_file = NULL;

static const char *bus_tag_name[BUS_MAX] = {
    "ROM",    // cart 0000-7FFF
    "ECRAM",   // cart Ram A000-BFFF
    "VRAM",
    "WRAM",
    "HRAM",
    "ECHO",
    "OAM",
    "IO",
    "IE",
    "UNMAPPED"
};

int logging_init(const char *filename) {
    debug_dump_file = fopen(filename, "w");
    if (!debug_dump_file) {
        printf("[ERROR] Failed to open debug log file!\n");
        return -1;
    }
    return 0;
}

int cpu_trace_init(const char *filename) {
    cpu_trace_file = fopen(filename, "w");
    if (!cpu_trace_file) {
        printf("[ERROR] Failed to open CPU Trace log file!\n");
        return -1;
    }
    return 0;
}

int trace_log_init(const char *filename) {
    trace_log_file = fopen(filename, "w");
    if (!trace_log_file) {
        printf("[ERROR] Failed to open Trace log file!\n");
        return -1;
    }
    return 0;
}

// Combine these two if possible?
void logging_close() {
    if (debug_dump_file) {
        fclose(debug_dump_file);
        debug_dump_file = NULL;
    }
}
void cpu_trace_close() {
    if (cpu_trace_file) {
        fclose(cpu_trace_file);
        cpu_trace_file = NULL;
    }
}
void trace_log_close() {
    if (trace_log_file) {
        fclose(trace_log_file);
        trace_log_file = NULL;
    }
}

// Manually Enter to log / trace file:
void logging_log(const char *format, ...) {
    if (!debug_dump_file) return;

    va_list args;
    va_start(args, format);
    vfprintf(debug_dump_file, format, args);
    va_end(args);

    fflush(debug_dump_file);
}

void logging_cpu_trace(const char *format, ...) {
    if (!cpu_trace_file) return;
    va_list args;
    va_start(args, format);
    vfprintf(cpu_trace_file, format, args);
    va_end(args);

    fflush(cpu_trace_file);
}

void logging_trace_log(const char *format, ...) {
    if (!trace_log_file) return;
    va_list args;
    va_start(args, format);
    vfprintf(trace_log_file, format, args);
    va_end(args);

    fflush(trace_log_file);
}

// NOTE DO:
// C 10 0x3405 | 0x78 | 0x76 0x50 0x2C 0x21 0x00 0xD8 0x4D 0x4D 0xFFFC 0x00 0x0000 0x0000
// M R 0x0000 0x00 00 00 // MMU Read Adderess ReadValue MapIndex, Bus_tag
// X R 0x0000 0 14 // Cart/Mapper, READ, BUS, VAL


// Function trace calls:
void trace_cpu() {
    // C 10 0x3405......
}

void trace_cart() {
    // X R 0x0000 0 14 ......

}

void trace_mmu_read(uint16_t addr, uint8_t val, int map_index, uint8_t src_tag) {
    
    // M R 0x0000 0x00 00 00 // MMU Read Adderess ReadValue MapIndex, Bus_tag
    fprintf(trace_log_file, "M R %04X %02X %d %s\n", addr, val, map_index, bus_tag_name[src_tag]);
    fflush(trace_log_file);

}

void trace_mmu_write(uint16_t addr, uint8_t val, int map_index, uint8_t src_tag) {
    // M R 0x0000 0x00 00 00 // MMU Read Adderess ReadValue MapIndex, Bus_tag
    fprintf(trace_log_file, "M W %04X %02X %d %s\n", addr, val, map_index, bus_tag_name[src_tag]);
    fflush(trace_log_file);

}