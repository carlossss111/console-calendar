get_jsmn:
	test -f src/jsmn.h || curl -Ls https://raw.githubusercontent.com/zserge/jsmn/master/jsmn.h > src/jsmn.h

compile: get_jsmn
	gcc -c src/sendhttps.c
	gcc src/main.c sendhttps.o -lcurl
	rm sendhttps.o

compile_debug: get_jsmn
	gcc -Wall -ansi -pedantic-errors -DDEBUG -c src/sendhttps.c
	gcc -Wall -ansi -pedantic-errors src/main.c sendhttps.o -lcurl
	rm sendhttps.o

run: get_jsmn
	gcc -c src/sendhttps.c
	gcc src/main.c sendhttps.o -lcurl
	rm sendhttps.o
	./a.out

run_debug: get_jsmn
	gcc -Wall -ansi -pedantic-errors -DDEBUG -c src/sendhttps.c
	gcc -Wall -ansi -pedantic-errors src/main.c sendhttps.o -lcurl
	rm sendhttps.o
	./a.out

mem: get_jsmn
	gcc -c src/sendhttps.c -g
	gcc src/main.c sendhttps.o -lcurl -g
	rm sendhttps.o
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --suppressions=./src/lib.supp ./a.out

mem_debug: get_jsmn
	gcc -Wall -ansi -pedantic-errors -DDEBUG -c src/sendhttps.c -g
	gcc -Wall -ansi -pedantic-errors src/main.c sendhttps.o -lcurl -g
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --suppressions=./src/lib.supp ./a.out
