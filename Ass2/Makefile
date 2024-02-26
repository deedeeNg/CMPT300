build: 
	gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L s-talk.c receiver.c send.c write.c list.c read.c -lpthread -o s-talk

run: build

clean:
	rm -f s-talk