# set up our compiler settings
# we will be compiling using gcc
CC     = gcc
# setting some useful flags:
# std=c99 is the standard which you should be using
# -Wall means 'report all warnings' which is useful to help you write better code
# -Werror means 'make all warnings into errors' which means your code doesn't compile with warnings
# this is a good idea when code quality is important
# -g enables the use of GDB
CFLAGS = -std=c99 -Wall -g -Werror=unused-variable
# this is your list of executables which you want to compile with all
EXE    = pgmEcho pgmComp pgmb2a pgma2b

# we put 'all' as the first command as this will be run if you just enter 'make'
all: ${EXE}
	gcc -g pgmComp.c -o pgmComp -Werror=unused-variable
	gcc -g pgmb2a.c -o pgmb2a -Werror=unused-variable
	gcc -g pgma2b.c -o pgma2b -Werror=unused-variable

# clean removes all object files - DO NOT UNDER ANY CIRCUMSTANCES ADD .c OR .h FILES
# rm is NOT REVERSIBLE.
clean: 
	rm -rf *.o ${EXE}

# this is a rule to define how .o files will be compiled
# it means we do not have to write a rule for each .o file
# pgmEcho.o will be compiled by this rule as:
# pgmEcho.o: pgmEcho.c
#gcc -c -std=c99 -g pgmEcho.c -o pgmEcho.o
# gcc -g pgmEcho.c -o pgmEcho 
	
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# for each executable, you need to tell the makefile the 'recipe' for your file
# at the moment, this is very simple as we are only using one .c file
# but as you refactor and add more .c and .h files
# these recipes will become more complex.

pgmEcho: pgmEcho.o
	$(CC) $(CCFLAGS) $^ -o $@
	


