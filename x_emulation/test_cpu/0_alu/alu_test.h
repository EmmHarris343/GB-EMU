#include "stdint.h"
#include "../cpu_test.h"
#include "../common/utile.h"
#include "../../src/cpu_instructions.h"


void build_add8(instruction_T inst);
void build_adc8(instruction_T inst);
void build_sub8(instruction_T inst);
void build_sbc(instruction_T inst);



// Putting this here, because it should be isolated from the other tests.

typedef struct {
    instruction_T instr;
    char *sub_tname;
    uint8_t initial_A;    
    uint8_t expected_A;
    uint8_t double_A_A;    
    uint8_t from_val;
    uint8_t expected_flags;
} add8_test_case;


typedef struct {
    instruction_T instr;
    char *sub_tname;
    uint8_t initial_A;
    uint8_t expected_A;
    uint8_t double_A_A;
    uint8_t from_val;
    uint8_t carry_state;
    uint8_t expected_flags;
} adc8_test_case;



typedef struct {
    instruction_T instr;
    char *sub_tname;
    uint8_t initial_A;
    uint8_t expected_A;
    uint8_t double_A_A;
    uint8_t from_val;
    uint8_t carry_state;
    uint8_t expected_flags;
} sub8_test_case;


typedef struct {
    instruction_T instr;
    char *sub_tname;
    uint8_t initial_A;
    uint8_t expected_A;
    uint8_t double_A_A;
    uint8_t from_val;
    uint8_t carry_state;
    uint8_t expected_flags;
} sbc8_test_case;