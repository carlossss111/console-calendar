compile:
	gcc -c src/sendhttps.c
	gcc src/main.c sendhttps.o -lcurl
	rm sendhttps.o

compile_debug:
	gcc -Wall -ansi -pedantic-errors -DDEBUG -c src/sendhttps.c
	gcc -Wall -ansi -pedantic-errors src/main.c sendhttps.o -lcurl
	rm sendhttps.o

run:
	gcc -c src/sendhttps.c
	gcc src/main.c sendhttps.o -lcurl
	rm sendhttps.o
	./a.out

run_debug:
	gcc -Wall -ansi -pedantic-errors -DDEBUG -c src/sendhttps.c
	gcc -Wall -ansi -pedantic-errors src/main.c sendhttps.o -lcurl
	rm sendhttps.o
	./a.out

mem:
	gcc -c src/sendhttps.c -g
	gcc src/main.c sendhttps.o -lcurl -g
	rm sendhttps.o
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --suppressions=./src/lib.supp ./a.out

mem_debug:
	gcc -Wall -ansi -pedantic-errors -DDEBUG -c src/sendhttps.c -g
	gcc -Wall -ansi -pedantic-errors src/main.c sendhttps.o -lcurl -g
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --suppressions=./src/lib.supp ./a.out
