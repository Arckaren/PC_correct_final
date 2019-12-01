CFLAGS+=-Wall -Wextra -fsanitize=address -g

all: jeu client server

jeu: jeu.o
	gcc $(CFLAGS) -o jeu jeu.o

client: client.o 
	gcc $(CFLAGS) -o client client.o

server: server.o
	gcc $(CFLAGS) -o server server.o

jeu.o: jeu.c
	gcc $(CFLAGS) -o jeu.o -c jeu.c

client.o: client.c
	gcc $(CFLAGS) -o client.o -c client.c 

server.o: server.c 
	gcc $(CFLAGS) -o server.o -c server.c 

clean:
	rm -rf *.o

mrproper: clean
	rm -rf client
	rm -rf server
	rm -rf jeu
