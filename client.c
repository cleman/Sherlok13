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
extern int cardsValue[13][8];
extern int synchro_L;
//extern int my_turn;

#include "client_thread.h"
int isWantConnect;
extern char sendBuffer[256];
extern int isWantSend;
extern int isWantCopytable;
extern int quit_graph;

#include "global_var.h"
int myDeck[3] = {-1,-1,-1};
int myId = -1;
char playerName[4][256] = {"-","-","-","-"};
int myTurn = 0;
int tableCartes[4][8];
int quit = 0;
char msgDisplay[128] = "Attente de connexion";
int coupable = -1;

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

	pthread_t tid_server, tid_graph;

	pthread_create(&tid_server, NULL, server_tcp, NULL);
	pthread_create(&tid_graph, NULL, thread_graphique_fn, NULL);
	char *msg = malloc(256);
	sprintf(msg, "C %s %d %s", argv[3], atoi(argv[4]), argv[5]);
	//printf("Message : %s\n", msg);

	server_address = argv[1];

	while (isWantConnect == 0);
	isWantConnect = 0;
	send_message(server_address, atoi(argv[2]), msg);

	while (!quit) {
		//sleep(5);
		//send_message(argv[1], atoi(argv[2]), "Message récurrent");
		if (isWantSend) {
			isWantSend = 0;
			send_message(server_address, atoi(argv[2]), sendBuffer);
		}

		if (isCarteWrite == 0 && nb_carte == 3) {
			isCarteWrite = 1;
			printf("Mes cartes sont : %d %d %d\n", myDeck[0], myDeck[1], myDeck[2]);
			for (int i = 0; i < 4; i++) {
				playerName[i][255] = '\0';
			}
			create_table();
			print_playerValue();
		}
		if (isMyIdWrite == 0 && myId != -1) {
			isMyIdWrite = 1;
			printf("Je suis connecté\n");
			printf("Mon identifiant est : %d\n", myId);
		}
		if (synchro_L) {
			synchro_L = 0;
			printf("Liste des joueurs : ");
			for (int i = 0; i < 4; i++) {
				printf("%s ", playerName[i]);
			}
			printf("\n");
		}		
	}

	while (!quit_graph);

    return 0;
}
