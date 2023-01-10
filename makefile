main: main.c
	gcc -o ncurses_test main.c -lncurses

debug: main
	./ncurses_test