CFLAGS+=-Wall -Wextra -lpthread -g 

all: client server 

client: client.c
	gcc $(CFLAGS) -o client client.c structure.c

server: server.c 
	gcc $(CFLAGS) -o server server.c structure.c


clean:
	rm -rf *.o
	rm -rf client
	rm -rf server
