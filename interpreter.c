#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "interpreter.h"

const int NUM_MEMORY_CELLS = 30000; // initialize with 30 kb of memory

/* Initialize system memory with the initial pointer position at 0
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

void free_mem(SystemMemory *mem) {
    free(mem->tape);
    free(mem);
}

int move_memory_pointer_left(SystemMemory *mem) {
    if (mem->curr_index > 0) {
        mem->curr_index -= 1;
    }
    return mem->curr_index;
}

int move_memory_pointer_right(SystemMemory *mem) {
    if (mem->curr_index < mem->tape_size - 1) {
        mem->curr_index += 1;
    }
    return mem->curr_index;
}

int increment_memory_cell_value(SystemMemory *mem) {
    char current_cell_value = mem->tape[mem->curr_index];
    if (current_cell_value < 127) {
        mem->tape[mem->curr_index] += 1;
    }
    return mem->tape[mem->curr_index];
}

int decrement_memory_cell_value(SystemMemory *mem) {
    char current_cell_value = mem->tape[mem->curr_index];
    if (current_cell_value > 0) {
        mem->tape[mem->curr_index] -= 1;
    }
    return mem->tape[mem->curr_index];
}

int output_current_cell_value(SystemMemory *mem) {
    char current_cell_value = mem->tape[mem->curr_index];
    printf("%c", current_cell_value);
    return current_cell_value;
}

int store_input_char_in_current_cell(SystemMemory *mem) {
    char input_char = getchar();
    mem->tape[mem->curr_index] = input_char;
    return input_char;
}

void conditional_break(SystemMemory *mem) {

}

void conditional_continue(SystemMemory *mem) {

}

void execute_instruction(char c, SystemMemory *mem) {
    switch(c) {
        case '<':
            move_memory_pointer_left(mem);
        case '>':
            move_memory_pointer_right(mem);
        case '+':
            increment_memory_cell_value(mem);
        case '-':
            decrement_memory_cell_value(mem);
        case '.':
            output_current_cell_value(mem);
        case ',':
            store_input_char_in_current_cell(mem);
        case '[':
            conditional_break(mem);
        case ']':
            conditional_continue(mem);
        // BF ignores all other characters
    }
}

void execute_code(char *instructions, SystemMemory *mem) {
    int i;
    int num_instructions = strlen(instructions);
    for (i = 0; i < num_instructions; i++) {
        execute_instruction(instructions[i], mem);
    }
}
