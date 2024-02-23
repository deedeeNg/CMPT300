build: 
	gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L main.c receiver.c send.c write.c list.c read.c -lpthread -o main

run: build

clean:
	rm -f main