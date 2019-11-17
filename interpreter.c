#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "interpreter.h"

const int NUM_MEMORY_CELLS = 30000; // initialize with 30 kb of memory

void log_system_error(const char *function_name, const char *message) {
    fprintf(stderr, "interpreter.c: Internal error in %s(): %s\n", function_name,
        message);
}

/* 
 * Initializes system memory with the initial pointer position at 0
 * and a completely blank tape (zeroes in all cells).
 */
SystemMemory *initialize_memory() {
    SystemMemory *mem = malloc(sizeof(SystemMemory));
    mem->tape_size = NUM_MEMORY_CELLS;
    char *tape = malloc(sizeof(char) * NUM_MEMORY_CELLS);
    mem->curr_index = 0;
    mem->tape = tape;
    return mem;
}

/*
 * Free allocated SystemMemory and all internal pointers.
 */
void free_mem(SystemMemory *mem) {
    free(mem->tape);
    free(mem);
}

/*
 * Moves the system memory's pointer one tape-cell to the left. Has no effect at
 * the left end of the tape. Returns the new index of the memory pointer.
 */
int move_memory_pointer_left(SystemMemory *mem) {
    if (mem->curr_index > 0) {
        mem->curr_index -= 1;
    }
    return mem->curr_index;
}

/*
 * Moves the system memory's pointer one tape-cell to the right. Has no effect
 * at the right end of the tape. Returns the new index of the memory pointer.
 */
int move_memory_pointer_right(SystemMemory *mem) {
    if (mem->curr_index < mem->tape_size - 1) {
        mem->curr_index += 1;
    }
    return mem->curr_index;
}

/*
 * Increments the value of the tape-cell under the pointer. Does
 * nothing if this would give a value greater than 127 (the tape only stores
 * valid ASCII values). Returns the cell's new value.
 */
int increment_memory_cell_value(SystemMemory *mem) {
    char current_cell_value = mem->tape[mem->curr_index];
    if (current_cell_value < 127) {
        mem->tape[mem->curr_index] += 1;
    }
    return mem->tape[mem->curr_index];
}

/*
 * Decrements the value of the tape-cell under the pointer. Does
 * nothing if this would give a value less than 0 (the tape only stores valid
 * ASCII values). Returns the cell's new value.
 */
int decrement_memory_cell_value(SystemMemory *mem) {
    char current_cell_value = mem->tape[mem->curr_index];
    if (current_cell_value > 0) {
        mem->tape[mem->curr_index] -= 1;
    }
    return mem->tape[mem->curr_index];
}

/*
 * Outputs to the console the value of the tape-cell under the pointer. Returns
 * the cell's value.
 */
int output_current_cell_value(SystemMemory *mem) {
    char current_cell_value = mem->tape[mem->curr_index];
    printf("%c", current_cell_value);
    return current_cell_value;
}

/*
 * Reads a single character as input and stores it in the tape-cell under the
 * pointer. Returns the stored value.
 */
int store_input_char_in_current_cell(SystemMemory *mem) {
    char input_char = getchar();
    mem->tape[mem->curr_index] = input_char;
    return input_char;
}

/*
 * Executed when a left bracket ("[") is encountered. If the tape-cell under the
 * pointer is 0, attempts to find the index of the matching right bracket. If it
 * can be found, the instruction after the right bracket is set as the current
 * instruction. This is equivalent to skipping a loop.
 * If the tape-cell under the pointer is not 0, the loop will be entered and
 * the left bracket's index will be added to the stack.
 * Returns the index of the current instruction after running this algorithm. If
 * there is no matching right bracket but a loop-skip is called for, returns -1.
 *
 * Note: This could be optimized by taking a pass through the instructions
 * before executing anything. The compiler could iterate through the code
 * and store with each left bracket the index of its corresponding right bracket,
 * making later jumps to the right bracket O(1). In the case of nested loops,
 * where we might repeat the scan for a right bracket when the outer loop is executed
 * more than once and the inner loop is not executed, this pre-work is more efficient.
 * The optimization would also catch unmatched brackets before execution.
 */
int conditional_loop_entry(SystemMemory *mem, char *instructions,
                           int instruction_index, Stack *stack) {
    // skip over the loop
    if (mem->tape[mem->curr_index] == 0) {
        int i;
        int nested_loop_offset = 0;
        int instr_length = strlen(instructions);
        // search ahead of the left bracket for a matching right bracket
        for (i = instruction_index + 1; i < instr_length; i++) {
            // if we encounter another left bracket, ignore its corresponding right bracket
            if (instructions[i] == '[') {
                nested_loop_offset++;
            }
            if (instructions[i] == ']') {
                if (nested_loop_offset-- == 0) {
                    // set the current instruction to the instruction after the loop
                    return i + 1;
                }
            }
        }
        log_system_error("conditional_loop_entry", "Error: no matching right-bracket found for left-bracket");
        return -1;
    }
    // add left bracket index to stack--entering loop
    stack_push(stack, instruction_index);
    return instruction_index + 1;
}

/*
 * Executed when a right bracket ("]") is encountered. If the tape-cell under
 * the pointer is NOT 0, jumps to the instruction after the previous left bracket.
 * If a matching left bracket can be found, the instruction after the left
 * bracket is set as the current instruction. This is equivalent to continuing
 * a loop.
 * If the pointer value is 0, the left bracket that marked the beginning of
 * the loop is removed from the stack and execution may continue beyond the loop.
 * Returns the index of the current instruction after this algorithm is run.
 * Regardless of the pointer value, if no matching left bracket is found,
 * returns -1.
 */
int conditional_continue(SystemMemory *mem, int instruction_index,
                         Stack *stack) {
    if (stack_size(stack) == 0) {
        log_system_error("conditional_continue", "Error: no matching left-bracket found for right-bracket");
        return -1;
    }
    if (mem->tape[mem->curr_index] == 0) {
        // remove the left bracket that started the loop from the stack
        stack_pop(stack);
        return instruction_index + 1;
    } else {
        int left_bracket_index = stack_peek(stack);
        return left_bracket_index + 1;
    }
}

/*
 * Executes the instruction at instruction_index. Returns the index of the next
 * instruction to execute.
 */
int execute_instruction(SystemMemory *mem, char *instructions,
                         int instruction_index, Stack *stack) {
    char instruction = instructions[instruction_index];
    int next_instruction_index = instruction_index + 1;
    switch(instruction) {
        case '<':
            move_memory_pointer_left(mem);
            break;
        case '>':
            move_memory_pointer_right(mem);
            break;
        case '+':
            increment_memory_cell_value(mem);
            break;
        case '-':
            decrement_memory_cell_value(mem);
            break;
        case '.':
            output_current_cell_value(mem);
            break;
        case ',':
            store_input_char_in_current_cell(mem);
            break;
        case '[':
            next_instruction_index = conditional_loop_entry(mem, instructions,
                                                       instruction_index, stack);
            if (next_instruction_index == -1) {
                exit(EXIT_FAILURE);
            }
            break;
        case ']':
            next_instruction_index = conditional_continue(mem, instruction_index, stack);
            if (next_instruction_index == -1) {
                exit(EXIT_FAILURE);
            }
            break;
        // the language ignores all other characters
    }
    return next_instruction_index;
}

/*
 * Executes Brainf**k source code stored in the provided instructions using the
 * provided SystemMemory.
 */
void execute_code(char *instructions, SystemMemory *mem) {
    int curr_instruction_index = 0;
    int num_instructions = strlen(instructions);
    Stack *left_bracket_stack = new_stack(); // stores the indices of left brackets in
                                             // the instructions array to support looping
    while (curr_instruction_index < num_instructions) {
        curr_instruction_index = execute_instruction(mem, instructions, 
                                    curr_instruction_index, left_bracket_stack);
    }
    stack_free(left_bracket_stack);
}
