#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "interpreter.h"

char *read_file_as_str(const char *file_name) {
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        printf("Error: File \"%s\" not found.\n", file_name);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    char *instructions = malloc(file_size + 1);
    fread(instructions, 1, file_size, file);
    instructions[file_size] = 0; // terminate string
    fclose(file);
    return instructions;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Error: Provide one command-line argument to specify the input file.\n");
        exit(EXIT_FAILURE);
    }

    const char *file_name = argv[1];
    SystemMemory *mem;
    char *instructions = read_file_as_str(file_name);
    mem = initialize_memory();
    execute_code(instructions, mem);

    free(instructions);
    free_mem(mem);
    puts("\n");
    return 0;
}
