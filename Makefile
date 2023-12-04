EXEC = client server
CFLAGS = -Wall
CC = gcc

all: $(EXEC)

server: server.c server_fc.c
	$(CC) $^ $(CFLAGS) -o $@

client: client.c client_fc.c
	$(CC) $^ $(CFLAGS) -o $@ -pthread

clean:
	rm -f *.o *~ $(EXEC)

vclean:
	rm -f *.o *~ core $(EXEC)