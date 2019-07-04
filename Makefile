

all: main.c
	gcc -Wall -g -o tcpip main.c

clean:
	rm -rf tcpip
