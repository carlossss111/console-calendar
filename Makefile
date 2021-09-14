get_jsmn:
	test -f src/jsmn.h || curl -Ls https://raw.githubusercontent.com/zserge/jsmn/master/jsmn.h > src/jsmn.h

compile: get_jsmn
	@echo Compiling...
	gcc -c src/sendhttps.c
	gcc src/main.c sendhttps.o -lcurl
	rm sendhttps.o

compile_debug: get_jsmn
	gcc -Wall -ansi -pedantic-errors -DDEBUG -c src/sendhttps.c -g
	gcc -Wall -ansi -pedantic-errors -DDEBUG src/main.c sendhttps.o -lcurl -g
	rm sendhttps.o

run: compile
	./a.out

run_debug: compile_debug
	./a.out

mem: compile
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --suppressions=./src/lib.supp ./a.out

mem_debug: compile_debug
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --suppressions=./src/lib.supp ./a.out

link:
	@echo Checking to see if already installed. If so, removing old version...
	test -f /usr/local/bin/consolecal && sudo rm /usr/local/bin/consolecal || continue
	test -f /usr/local/bin/consolecal_data/config.txt && sudo rm /usr/local/bin/consolecal_data/* || continue
	test -d /usr/local/bin/consolecal_data && sudo rmdir /usr/local/bin/consolecal_data || continue
	@echo Linking binary file...
	sudo ln $(shell pwd)/a.out /usr/local/bin/consolecal
	@echo Linking text files...
	sudo mkdir /usr/local/bin/consolecal_data
	sudo ln $(shell pwd)/*.txt /usr/local/bin/consolecal_data/
	sudo ln $(shell pwd)/README.md /usr/local/bin/consolecal_data/
	@echo Linking complete, if there were no errors then the file should be able to be run globally as \'consolecal\'.
	@echo This directory and it\'s contents can now be safely deleted.

compile_and_link: compile link