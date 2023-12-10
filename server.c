#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <arpa/inet.h>

#include "server_fc.h"

extern char *carteNom[13];
extern int deck[13];
extern int cardsValue[13][8];
extern int playerValue[4][8];
extern int quit;

int main(int argc, char *argv[])
{
	client tab[4];	// Données des joueurs
	int nb_joueur = 0;	// Nombre de joueurs
	int isCardGive = 0, isStart = 0, nextStep = 0;
	int joueur_actif = 0;
	for (int i = 0; i < 4; i++) {
		tab[i].ip_addr = "localhost";
		tab[i].nom = "-";
		tab[i].numport = -1;
	}

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    if (argc < 2) 
	{
        fprintf(stderr,"./server <numport>\n");
        fprintf(stderr,"ex: ./server 32000\n");
		return 1;
    }

	// 1) on crée la socket, SOCK_STREAM signifie TCP
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
	{
        fprintf(stderr,"Impossible d'ouvrir la socket.\n");
		return 1;
	}

	// 2) on réclame au noyau l'utilisation du port passé en paramètre 
	// INADDR_ANY dit que la socket va être affectée à toutes les interfaces locales

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
	{
        fprintf(stderr,"Impossible de faire l'appel système bind().\n");
		return 1;
	}


	// On commence à écouter sur la socket. Le 5 est le nombre max
	// de connexions pendantes

	shuffle_card();
	printf("Carte mélangées\n");
	printf("Le coupable est  : %s\n", carteNom[deck[12]]);

    listen(sockfd,5);
	while (!quit)
	{
		if (nb_joueur == 4 && isStart == 0) {
			isStart = 1;
			joueur_actif = -1;
			nextStep = 1;
			printf("La partie commence\n");
			char msg_G = 'D';
			bc(&tab, &msg_G);
		}
		if (nb_joueur == 4 && isCardGive == 0) {
			isCardGive = 1;
			char msg_v[16];
			for (int i = 0; i < 4; i++){
				for (int j = 0; j < 3; j++) {
					sprintf(msg_v, "V %d", deck[i*3+j]);
					send_message(tab[i].ip_addr, tab[i].numport, msg_v);
				}
			}
		} 

		// Joueur suivant
		if (nextStep) {
			joueur_actif++;
			joueur_actif %= 4;

			nextStep = 0;
			printf("Joueur %d joue\n", joueur_actif);

			char msg_M[8];
			sprintf(msg_M, "M %d", joueur_actif);
			bc(&tab, msg_M);

			char msg_T[128];
			sprintf(msg_T, "T %s joue", tab[joueur_actif].nom);
			bc(&tab, msg_T);
		}

		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
		{
			perror("Error accepting connection");
			fprintf(stderr,"Impossible de faire l'appel système accept().\n");
			return 1;
		}

		bzero(buffer,256);
		n = read(newsockfd,buffer,255);
		if (n < 0) 
		{
			fprintf(stderr,"Impossible de faire l'appel système read().\n");
			return 1;
		}

		//printf("Received packet from %s:%d\nData: [%s]\n\n",
		//inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);
		//printf("Premier caractère : %c\n", buffer[0]);
		if (buffer[0] == 'C') {		// Première connexion
			traitement_C(buffer, &tab, &nb_joueur);

			printf("Nombre de joueur connecté : %d\n", nb_joueur);

			printf("Liste des joueurs : %s %s %s %s\n", tab[0].nom, tab[1].nom, tab[2].nom, tab[3].nom);

			char msg_I[8];
			sprintf(msg_I,"I %d", nb_joueur-1);
			send_message(tab[nb_joueur-1].ip_addr, tab[nb_joueur-1].numport, msg_I);
			send_list(&tab, nb_joueur);

			char msg_T[128];
			sprintf(msg_T, "T En attente de joueur. Nombre de joueur : %d", nb_joueur);
			for (int i = 0; i < nb_joueur; i++) {
				send_message(tab[i].ip_addr, tab[i].numport, msg_T);
			}			
		}
		else if (buffer[0] == 'O') {		// Action 1 (all)
			if (atoi(&buffer[2]) == joueur_actif) {
				fc_act_1(&tab, buffer, joueur_actif);
				nextStep = 1;
			}
		}
		else if (buffer[0] == 'S') {		// Action 2 (private)
			if (atoi(&buffer[2]) == joueur_actif) {
				fc_act_2(&tab, buffer);
				nextStep = 1;
			}
		}
		else if (buffer[0] == 'G') {		// Action 3 (accusation)
			if (atoi(&buffer[2]) == joueur_actif) {
				fc_act_3(&tab, buffer);
				nextStep = 1;
			}
		}
		else if (buffer[0] == 'F') {		// Fin
			char msg_F[16];
			sprintf(msg_F, "F");
			bc(&tab, msg_F);
			exit(1);
		}

		close(newsockfd);
	}

	close(sockfd);
	return 0; 
}
