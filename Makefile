EXEC = client server
CFLAGS = -Wall
CC = gcc

all: $(EXEC)

server: server.c server_fc.c
	$(CC) $^ $(CFLAGS) -o $@

client: client.c client_fc.c client_thread.c
	$(CC) $^ $(CFLAGS) -o $@ -I/usr/include/SDL2 -lpthread -lSDL2_image -lSDL2_ttf -lSDL2

clean:
	rm -f *.o *~ $(EXEC)

vclean:
	rm -f *.o *~ core $(EXEC)