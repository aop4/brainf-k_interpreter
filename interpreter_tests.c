#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "CUnit/Basic.h"
#include "interpreter.h"
#include "stack.h"

int init_suite(void) {
   return 0;
}

int clean_suite(void) {
   return 0;
}

SystemMemory *create_test_memory(int num_tape_cells, int start_index) {
    SystemMemory *mem = malloc(sizeof(SystemMemory));
    char *tape = malloc(sizeof(char) * num_tape_cells);
    mem->tape_size = num_tape_cells;
    mem->curr_index = start_index;
    mem->tape = tape;
    return mem;
}

static void test_initialize_memory(void) {
    SystemMemory *mem = initialize_memory();
    CU_ASSERT_EQUAL(0, mem->curr_index);
    CU_ASSERT_EQUAL(30000, mem->tape_size);
    CU_ASSERT_PTR_NOT_NULL(mem->tape);
    free_mem(mem);
}

static void test_initialize_memory_has_blank_tape(void) {
    SystemMemory *mem = initialize_memory();
    int i;
    for (i = 0; i < 30000; i++) {
        if (mem->tape[i] != 0) {
            free_mem(mem);
            CU_FAIL();
            return;
        }
    }
    free_mem(mem);
}

static void test_move_memory_pointer_left(void) {
    SystemMemory *mem = create_test_memory(100, 1);
    int res = move_memory_pointer_left(mem);
    CU_ASSERT_EQUAL(0, mem->curr_index);
    CU_ASSERT_EQUAL(0, res);
    free_mem(mem);
}

static void test_move_memory_pointer_left_at_left_end_of_tape(void) {
    SystemMemory *mem = create_test_memory(100, 0);
    int res = move_memory_pointer_left(mem);
    CU_ASSERT_EQUAL(0, mem->curr_index);
    CU_ASSERT_EQUAL(0, res);
    free_mem(mem);
}

static void test_move_memory_pointer_right(void) {
    SystemMemory *mem = create_test_memory(100, 98);
    int res = move_memory_pointer_right(mem);
    CU_ASSERT_EQUAL(99, mem->curr_index);
    CU_ASSERT_EQUAL(99, res);
    free_mem(mem);
}

static void test_move_memory_pointer_right_at_right_end_of_tape(void) {
    SystemMemory *mem = create_test_memory(100, 99);
    int res = move_memory_pointer_right(mem);
    CU_ASSERT_EQUAL(99, mem->curr_index);
    CU_ASSERT_EQUAL(99, res);
    free_mem(mem);
}

static void test_increment_memory_cell_value(void) {
    SystemMemory *mem = create_test_memory(100, 0);
    mem->tape[mem->curr_index] = 126;
    int res = increment_memory_cell_value(mem);
    CU_ASSERT_EQUAL(127, mem->tape[mem->curr_index]);
    CU_ASSERT_EQUAL(127, res);
    free_mem(mem);
}

static void test_increment_memory_cell_value_does_not_overflow(void) {
    SystemMemory *mem = create_test_memory(100, 0);
    mem->tape[mem->curr_index] = 127;
    int res = increment_memory_cell_value(mem);
    CU_ASSERT_EQUAL(127, mem->tape[mem->curr_index]);
    CU_ASSERT_EQUAL(127, res);
    free_mem(mem);
}

static void test_decrement_memory_cell_value(void) {
    SystemMemory *mem = create_test_memory(100, 0);
    mem->tape[mem->curr_index] = 1;
    int res = decrement_memory_cell_value(mem);
    CU_ASSERT_EQUAL(0, mem->tape[mem->curr_index]);
    CU_ASSERT_EQUAL(0, res);
    free_mem(mem);
}

static void test_decrement_memory_cell_value_stays_above_zero(void) {
    SystemMemory *mem = create_test_memory(100, 0);
    mem->tape[mem->curr_index] = 0;
    int res = decrement_memory_cell_value(mem);
    CU_ASSERT_EQUAL(0, mem->tape[mem->curr_index]);
    CU_ASSERT_EQUAL(0, res);
    free_mem(mem);
}

static void test_output_current_cell_value(void) {
    SystemMemory *mem = create_test_memory(100, 0);
    mem->tape[mem->curr_index] = 65;
    // simply verify the correct value is returned; do not verify console output
    CU_ASSERT_EQUAL(65, output_current_cell_value(mem));
    free_mem(mem);
}



int main() {
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    CU_initialize_registry();

    /* add a suite to the registry */
    interpreter_suite = CU_add_suite("Interpreter Suite", init_suite, clean_suite);
    stack_suite = CU_add_suite("Stack Suite", init_suite, clean_suite);

    /* add the tests to the suite */
    CU_add_test(interpreter_suite, "test_initialize_memory", test_initialize_memory);
    CU_add_test(interpreter_suite, "test_initialize_memory_has_blank_tape", test_initialize_memory_has_blank_tape);
    CU_add_test(interpreter_suite, "test_move_memory_pointer_left", test_move_memory_pointer_left);
    CU_add_test(interpreter_suite, "test_move_memory_pointer_right", test_move_memory_pointer_right);
    CU_add_test(interpreter_suite, "test_move_memory_pointer_left_at_left_end_of_tape", test_move_memory_pointer_left_at_left_end_of_tape);
    CU_add_test(interpreter_suite, "test_move_memory_pointer_right_at_right_end_of_tape", test_move_memory_pointer_right_at_right_end_of_tape);
    CU_add_test(interpreter_suite, "test_increment_memory_cell_value", test_increment_memory_cell_value);
    CU_add_test(interpreter_suite, "test_increment_memory_cell_value_does_not_overflow", test_increment_memory_cell_value_does_not_overflow);
    CU_add_test(interpreter_suite, "test_decrement_memory_cell_value", test_decrement_memory_cell_value);
    CU_add_test(interpreter_suite, "test_decrement_memory_cell_value_stays_above_zero", test_decrement_memory_cell_value_stays_above_zero);
    CU_add_test(interpreter_suite, "test_output_current_cell_value", test_output_current_cell_value);
    //CU_add_test(pSuite, "", );

    CU_add_test(stack_suite, "", );

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
