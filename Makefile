compile:
	gcc src/sendhttps.c -lcurl

compile_debug:
	gcc -Wall -ansi -pedantic-errors -DDEBUG src/sendhttps.c -lcurl

run:
	gcc src/sendhttps.c -lcurl
	./a.out

run_debug:
	gcc -Wall -ansi -pedantic-errors -DDEBUG src/sendhttps.c -lcurl
	./a.out

mem:
	gcc src/sendhttps.c -lcurl -g
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --suppressions=./src/lib.supp ./a.out

mem_debug:
	gcc -Wall -ansi -pedantic-errors -DDEBUG src/sendhttps.c -lcurl -g
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --suppressions=./src/lib.supp ./a.out
