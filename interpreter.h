#ifndef INTERPRETER_HEADER
#define INTERPRETER_HEADER

typedef struct {
    char *tape;
    int tape_size;
    int curr_index;
} SystemMemory;

SystemMemory *initialize_memory();

void free_mem(SystemMemory *mem);

int move_memory_pointer_left(SystemMemory *mem);

int move_memory_pointer_right(SystemMemory *mem);

int increment_memory_cell_value(SystemMemory *mem);

int decrement_memory_cell_value(SystemMemory *mem);

int output_current_cell_value(SystemMemory *mem);

int store_input_char_in_current_cell(SystemMemory *mem);

void conditional_break(SystemMemory *mem);

void conditional_continue(SystemMemory *mem);

void execute_code(char *instructions, SystemMemory *mem);

void execute_instruction(char c, SystemMemory *mem);

#endif