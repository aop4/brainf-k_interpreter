run: source/run.c source/stack.c source/interpreter.c
	gcc -o run source/interpreter.c source/stack.c source/run.c -I.

tests: source/interpreter.c source/stack.c source/interpreter_tests.c
	gcc -o interpreter_tests source/interpreter_tests.c source/interpreter.c source/stack.c -lcunit -I.

clean:
	rm run interpreter_tests
	
