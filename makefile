all: client.c

	gcc client.c -Wall -o client

all-GDB: client.c

	gcc -g client.c -Wall -o client