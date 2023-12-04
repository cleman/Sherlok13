#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>

#include "client_fc.h"

extern char buffer_server[256];
extern int my_server_numport;
extern char my_server_adress;
extern char *server_address;

int main(int argc, char *argv[])
{
	if (argc<6)
	{
		fprintf(stderr,"./client <server_address> <server_numport> <your_server_address> <your_server_numport> <nom\n");
		fprintf(stderr,"ex: ./client localhost 32000 joe localhost 32001\n");
		return 1;
	}
	//my_server_adress = *argv[4];
	my_server_numport = atoi(argv[4]);

	pthread_t tid_server;

	pthread_create(&tid_server, NULL, server_tcp, NULL);
	char *msg = malloc(256);
	sprintf(msg, "C %s %d %s", argv[3], atoi(argv[4]), argv[5]);
	//rintf("Message : %s\n", msg);

	server_address = argv[1];

	send_message(server_address, atoi(argv[2]), msg);
	//send_message(argv[1], atoi(argv[2]), "un autre message");

	while (1) {
		//sleep(5);
		//send_message(argv[1], atoi(argv[2]), "Message récurrent");
	}

    return 0;
}
