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
extern int nb_carte;
extern char *carteNom[13];
extern int my_deck[3];
extern int cardsValue[13][8];
extern int my_id;
extern char *listeJoueur[4];
extern int synchro_L;
extern int playerValue[4][8];

int main(int argc, char *argv[])
{
	int isCarteWrite = 0, isMyIdWrite = 0;

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
	//printf("Message : %s\n", msg);

	server_address = argv[1];

	send_message(server_address, atoi(argv[2]), msg);
	//send_message(argv[1], atoi(argv[2]), "un autre message");

	while (1) {
		//sleep(5);
		//send_message(argv[1], atoi(argv[2]), "Message récurrent");
		if (isCarteWrite == 0 && nb_carte == 3) {
			isCarteWrite = 1;
			printf("Mes cartes sont : %d %d %d\n", my_deck[0], my_deck[1], my_deck[2]);
			create_table();
			print_playerValue();
		}
		if (isMyIdWrite == 0 && my_id != -1) {
			isMyIdWrite = 1;
			printf("Mon identifiant est : %d\n", my_id);
		}
		if (synchro_L) {
			synchro_L = 0;
			printf("Liste des joueurs : ");
			for (int i = 0; i < 4; i++) {
				printf("%s ", listeJoueur[i]);
			}
			printf("\n");
		}
	}

    return 0;
}
