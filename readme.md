An interpreter for everyone's favorite esoteric programming language: https://esolangs.org/wiki/Brainfuck  
This is written in C, so it may only work with certain operating systems. It has only been tested with the default C compiler for Ubuntu 18.04.  

Compile the interpreter:
```bash
make run
```

Run sample programs:
```bash
./run samples/hello_world.bf # prints "Hello world!"
./run samples/addition.bf # does basic addition of two integers and prints the result
./run samples/cat.bf < file_name # outputs the contents of a file (alternatively, leave the file out and it will echo user input)
```

The interpreter was tested with CUnit. To install in Ubuntu:
```bash
apt-get install libcunit1 libcunit1-doc libcunit1-dev
```

To run tests (outputs to the command line):
```
make tests
./interpreter_tests
```

You will also find, in `source/stack.c`, a not-quite-textbook implementation of an integer stack.  
