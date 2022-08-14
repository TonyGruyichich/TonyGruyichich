assembler: first_pass.o second_pass.o code.o preprocessor.o assembler.o globals.h
	gcc -g -ansi -Wall first_pass.o second_pass.o code.o preprocessor.o assembler.o -o assembler

assembler.o: assembler.c first_pass.h second_pass.h globals.h preprocessor.h
	gcc -c -g -ansi -Wall -pedantic assembler.c -o assembler.o

first_pass.o: first_pass.c globals.h code.h first_pass.h
	gcc -c -g -ansi -Wall -pedantic first_pass.c -o first_pass.o
	
code.o: code.c code.h globals.h
	gcc -c -g -ansi -Wall -pedantic code.c -o code.o
	
second_pass.o: second_pass.c globals.h code.h second_pass.h
	gcc -c -g -ansi -Wall -pedantic second_pass.c -o second_pass.o

preprocessor.o: preprocessor.c globals.h code.h preprocessor.h
	gcc -c -g -ansi -Wall -pedantic preprocessor.c -o preprocessor.o
