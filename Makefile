run: run.c
	gcc -o run interpreter.c run.c -I.

tests: interpreter.c stack.c interpreter_tests.c
	gcc -o interpreter_tests interpreter_tests.c interpreter.c stack.c -lcunit -I.

clean:
	rm interpreter interpreter_tests
	