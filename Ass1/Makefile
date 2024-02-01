# Define what compiler to use and the flags.
CC=cc
CXX=CC
CCFLAGS= -g -Wall -Werror

all: test_list

# Compile all .c files into .o files
# % matches all (like * in a command)
# $< is the source file (.c file)
%.o : %.c
	$(CC) -c $(CCFLAGS) $<


test_list: test_list.o
	$(CC) -o test_list test_list.o

clean:
	rm -f core *.o test_list

