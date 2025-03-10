#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>



#define HEADER_OFFSET       0x0100      // Where the header starts in All ROM files.
#define HEADER_SIZE         0x50        // Header END point is: 0x014f (50 Bytes Total)


#define ENTRY_POINT_START   0x0100      // Same as the beginning of the header.
#define ENTRY_POINT_END     0x0103      // Where first Code executions start.


void main() {
    printf("e_cpu startup..");


}