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

static void test_conditional_loop_entry_enter_loop() {
    SystemMemory *mem = create_test_memory(100, 0);
    mem->tape[mem->curr_index] = 27; // non-zero memory value
    char *instruction_snippet = "[>-[++-";
    int left_bracket_index = 3; // instruction pointer starts at second bracket
    Stack *left_bracket_stack = new_stack();
    stack_push(left_bracket_stack, 0); // give stack one value
    int next_instr_index = conditional_loop_entry(mem, instruction_snippet,
                                        left_bracket_index, left_bracket_stack);
    // validate returned instruction index is right after the left bracket
    // (we enter the loop)
    CU_ASSERT_EQUAL(4, next_instr_index);
    // validate stack contents
    CU_ASSERT_EQUAL(2, stack_size(left_bracket_stack));
    CU_ASSERT_EQUAL(3, stack_peek(left_bracket_stack)); // index of left bracket
    free_mem(mem);
    stack_free(left_bracket_stack);
}

static void test_conditional_loop_entry_skip_loop_no_matching_right_bracket() {
    SystemMemory *mem = create_test_memory(100, 0);
    mem->tape[mem->curr_index] = 0; // zero memory value
    char *instruction_snippet = "[>-[++-";
    int left_bracket_index = 3; // instruction pointer starts at second bracket
    Stack *left_bracket_stack = new_stack();
    stack_push(left_bracket_stack, 0); // give stack one value
    int next_instr_index = conditional_loop_entry(mem, instruction_snippet,
                                        left_bracket_index, left_bracket_stack);
    // validate returned instruction index is -1: no matching right bracket
    CU_ASSERT_EQUAL(-1, next_instr_index);
    // validate stack was not changed
    CU_ASSERT_EQUAL(1, stack_size(left_bracket_stack));
    free_mem(mem);
    stack_free(left_bracket_stack);
}

static void test_conditional_loop_entry_skip_loop_has_matching_right_bracket() {
    SystemMemory *mem = create_test_memory(100, 0);
    mem->tape[mem->curr_index] = 0; // zero memory value
    char *instruction_snippet = "[>-[++]-";
    int left_bracket_index = 3; // instruction pointer starts at second left bracket
    Stack *left_bracket_stack = new_stack();
    stack_push(left_bracket_stack, 0); // give stack one value
    int next_instr_index = conditional_loop_entry(mem, instruction_snippet,
                                        left_bracket_index, left_bracket_stack);
    // validate returned instruction index follows right bracket
    // (we skip over the loop)
    CU_ASSERT_EQUAL(7, next_instr_index);
    // validate stack was not changed
    CU_ASSERT_EQUAL(1, stack_size(left_bracket_stack));
    free_mem(mem);
    stack_free(left_bracket_stack);
}

static void test_conditional_loop_entry_nested_loop_skip_no_matching_right_bracket() {
    SystemMemory *mem = create_test_memory(100, 0);
    mem->tape[mem->curr_index] = 0; // zero memory value
    char *instruction_snippet = "[>-[+]-"; // the second left bracket nullifies the right bracket
    int left_bracket_index = 0; // instruction pointer at first left bracket
    Stack *left_bracket_stack = new_stack();
    int next_instr_index = conditional_loop_entry(mem, instruction_snippet,
                                        left_bracket_index, left_bracket_stack);
    // validate returned instruction index is -1: no matching right bracket
    CU_ASSERT_EQUAL(-1, next_instr_index);
    free_mem(mem);
    stack_free(left_bracket_stack);
}

static void test_conditional_loop_entry_nested_loop_skip_has_matching_right_bracket() {
    SystemMemory *mem = create_test_memory(100, 0);
    mem->tape[mem->curr_index] = 0; // zero memory value
    char *instruction_snippet = "[>-[++]]-";
    int left_bracket_index = 0; // instruction pointer starts at first left bracket
    Stack *left_bracket_stack = new_stack();
    int next_instr_index = conditional_loop_entry(mem, instruction_snippet,
                                        left_bracket_index, left_bracket_stack);
    // validate returned instruction index follows second right bracket
    // (we skip over the loop and its internal nested loop)
    CU_ASSERT_EQUAL(8, next_instr_index);
    free_mem(mem);
    stack_free(left_bracket_stack);
}

static void test_conditional_continue_restart_loop_no_matching_left_bracket() {
    SystemMemory *mem = create_test_memory(100, 0);
    mem->tape[mem->curr_index] = 1; // non-zero memory value
    Stack *left_bracket_stack = new_stack(); // empty stack
    int next_instr_index = conditional_continue(mem, 0 ,left_bracket_stack);
    CU_ASSERT_EQUAL(-1, next_instr_index);
    free_mem(mem);
    stack_free(left_bracket_stack);
}

static void test_conditional_continue_restart_loop_has_matching_left_bracket() {
    SystemMemory *mem = create_test_memory(100, 0);
    mem->tape[mem->curr_index] = 1; // non-zero memory value
    Stack *left_bracket_stack = new_stack();
    stack_push(left_bracket_stack, 3); // left bracket at instruction 3
    int next_instr_index = conditional_continue(mem, 17, left_bracket_stack);
    CU_ASSERT_EQUAL(4, next_instr_index); // next instruction index is after left bracket
    CU_ASSERT_EQUAL(1, stack_size(left_bracket_stack)); // stack should be unchanged
    free_mem(mem);
    stack_free(left_bracket_stack);
}

static void test_conditional_continue_end_loop_has_matching_left_bracket() {
    SystemMemory *mem = create_test_memory(100, 0);
    mem->tape[mem->curr_index] = 0; // zero memory value--end the loop
    Stack *left_bracket_stack = new_stack();
    stack_push(left_bracket_stack, 3); // left bracket at instruction 3
    int next_instr_index = conditional_continue(mem, 17, left_bracket_stack);
    CU_ASSERT_EQUAL(18, next_instr_index); // next instruction index is after current
    CU_ASSERT_EQUAL(0, stack_size(left_bracket_stack)); // left bracket removed from stack
    free_mem(mem);
    stack_free(left_bracket_stack);
}

static void test_conditional_continue_end_loop_no_matching_left_bracket() {
    SystemMemory *mem = create_test_memory(100, 0);
    mem->tape[mem->curr_index] = 0; // zero memory value--end the loop
    Stack *left_bracket_stack = new_stack(); // empty stack
    int next_instr_index = conditional_continue(mem, 17, left_bracket_stack);
    CU_ASSERT_EQUAL(-1, next_instr_index); // no matching left bracket found--syntax error
    free_mem(mem);
    stack_free(left_bracket_stack);
}

static void test_stack_size(void) {
    Stack *stack = new_stack();
    stack->size = 12;
    CU_ASSERT_EQUAL(12, stack_size(stack));
    stack_free(stack);
}

static void test_stack_peek(void) {
    Stack *stack = new_stack();
    Node *tail = malloc(sizeof(Node));
    stack->tail = tail;
    tail->val = 14;
    CU_ASSERT_EQUAL(14, stack_peek(stack));
    stack_free(stack);
}

static void test_stack_push_with_empty_stack(void) {
    Stack *stack = new_stack();
    stack_push(stack, 99);

    CU_ASSERT_EQUAL(1, stack->size);
    CU_ASSERT_EQUAL(99, stack->tail->val);
    // test that pointers of linked list are properly set
    CU_ASSERT_PTR_NOT_NULL(stack->head);
    CU_ASSERT_PTR_NOT_NULL(stack->tail);
    CU_ASSERT_PTR_EQUAL(stack->head, stack->tail);
    CU_ASSERT_PTR_NULL(stack->head->next);
    CU_ASSERT_PTR_NULL(stack->head->prev);
    stack_free(stack);
}

static void test_stack_push_two_items(void) {
    Stack *stack = new_stack();
    stack_push(stack, 99);
    stack_push(stack, 100);

    CU_ASSERT_EQUAL(2, stack->size);
    CU_ASSERT_EQUAL(99, stack->head->val);
    CU_ASSERT_EQUAL(100, stack->tail->val);
    // test that pointers of linked list are properly set
    CU_ASSERT_PTR_NULL(stack->head->prev);
    CU_ASSERT_PTR_NULL(stack->tail->next);
    CU_ASSERT_PTR_EQUAL(stack->tail, stack->head->next);
    CU_ASSERT_PTR_EQUAL(stack->head, stack->tail->prev);
    stack_free(stack);
}

static void test_stack_push_three_items(void) {
    Stack *stack = new_stack();
    stack_push(stack, 99);
    stack_push(stack, 100);
    stack_push(stack, 101);

    CU_ASSERT_EQUAL(3, stack->size);
    CU_ASSERT_EQUAL(99, stack->head->val);
    CU_ASSERT_EQUAL(100, stack->head->next->val);
    CU_ASSERT_EQUAL(101, stack->tail->val);
    // test that pointers of linked list are properly set
    CU_ASSERT_PTR_NULL(stack->head->prev);
    CU_ASSERT_PTR_NULL(stack->tail->next);
    CU_ASSERT_PTR_EQUAL(stack->tail, stack->head->next->next);
    CU_ASSERT_PTR_EQUAL(stack->head, stack->tail->prev->prev);
    stack_free(stack);
}

static void test_stack_pop_one_item(void) {
    Stack *stack = new_stack();
    stack_push(stack, 99);
    int val = stack_pop(stack);
    
    CU_ASSERT_EQUAL(99, val);
    CU_ASSERT_EQUAL(0, stack->size);
    CU_ASSERT_PTR_NULL(stack->head);
    CU_ASSERT_PTR_NULL(stack->tail);
    stack_free(stack);
}

static void test_stack_pop_two_items(void) {
    Stack *stack = new_stack();
    stack_push(stack, 99);
    stack_push(stack, 100);
    int val = stack_pop(stack);

    CU_ASSERT_EQUAL(100, val);
    CU_ASSERT_EQUAL(1, stack->size);
    CU_ASSERT_EQUAL(99, stack->head->val);
    CU_ASSERT_PTR_EQUAL(stack->head, stack->tail);
    CU_ASSERT_PTR_NULL(stack->head->next);
    stack_free(stack);
}

static void test_stack_pop_three_items(void) {
    Stack *stack = new_stack();
    stack_push(stack, 99);
    stack_push(stack, 100);
    stack_push(stack, 101);
    int val = stack_pop(stack);

    CU_ASSERT_EQUAL(101, val);
    CU_ASSERT_EQUAL(2, stack->size);
    CU_ASSERT_EQUAL(99, stack->head->val);
    CU_ASSERT_EQUAL(100, stack->head->next->val);
    CU_ASSERT_PTR_EQUAL(stack->tail, stack->head->next);
    CU_ASSERT_PTR_NULL(stack->tail->next);
    stack_free(stack);
}

int main() {
    CU_pSuite interpreter_suite;
    CU_pSuite stack_suite;

    /* initialize the CUnit test registry */
    CU_initialize_registry();

    /* add a suite to the registry */
    interpreter_suite = CU_add_suite("Interpreter Suite", init_suite, clean_suite);
    stack_suite = CU_add_suite("Stack Suite", init_suite, clean_suite);

    /* add tests to the interpreter suite */
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
    CU_add_test(interpreter_suite, "test_conditional_loop_entry_enter_loop", test_conditional_loop_entry_enter_loop);
    CU_add_test(interpreter_suite, "test_conditional_loop_entry_skip_loop_no_matching_right_bracket", test_conditional_loop_entry_skip_loop_no_matching_right_bracket);
    CU_add_test(interpreter_suite, "test_conditional_loop_entry_skip_loop_has_matching_right_bracket", test_conditional_loop_entry_skip_loop_has_matching_right_bracket);
    CU_add_test(interpreter_suite, "test_conditional_loop_entry_nested_loop_skip_no_matching_right_bracket", test_conditional_loop_entry_nested_loop_skip_no_matching_right_bracket);
    CU_add_test(interpreter_suite, "test_conditional_loop_entry_nested_loop_skip_has_matching_right_bracket", test_conditional_loop_entry_nested_loop_skip_has_matching_right_bracket);
    CU_add_test(interpreter_suite, "test_conditional_continue_restart_loop_no_matching_left_bracket", test_conditional_continue_restart_loop_no_matching_left_bracket);
    CU_add_test(interpreter_suite, "test_conditional_continue_restart_loop_has_matching_left_bracket", test_conditional_continue_restart_loop_has_matching_left_bracket);
    CU_add_test(interpreter_suite, "test_conditional_continue_end_loop_has_matching_left_bracket", test_conditional_continue_end_loop_has_matching_left_bracket);
    CU_add_test(interpreter_suite, "test_conditional_continue_end_loop_no_matching_left_bracket", test_conditional_continue_end_loop_no_matching_left_bracket);
    
    /* add tests to the stack suite */
    CU_add_test(stack_suite, "test_stack_size", test_stack_size);
    CU_add_test(stack_suite, "test_stack_peek", test_stack_peek);
    CU_add_test(stack_suite, "test_stack_push_with_empty_stack", test_stack_push_with_empty_stack);
    CU_add_test(stack_suite, "test_stack_push_two_items", test_stack_push_two_items);
    CU_add_test(stack_suite, "test_stack_push_three_items", test_stack_push_three_items);
    CU_add_test(stack_suite, "test_stack_pop_one_item", test_stack_pop_one_item);
    CU_add_test(stack_suite, "test_stack_pop_two_items", test_stack_pop_two_items);
    CU_add_test(stack_suite, "test_stack_pop_three_items", test_stack_pop_three_items);

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
