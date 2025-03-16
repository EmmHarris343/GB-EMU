#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


// Bank one 0b00100

// Bank two 0b10

// 68 = 0x44


// Area being Read: 0b0111 0010 1010 0111

// 0b1   0                      // 0b10 Value Bank 2 Register
// 001                          // 0b00100 Value bank 1 Register
// 0011 0010 1010 0111          // 0x1132A7 ((11 0010 1010 0111))

// Actual Rom Address