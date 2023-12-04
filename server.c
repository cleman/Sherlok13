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


int main(int argc, char *argv[])
{
	client tab[4];	// Données des joueurs
	int nb_joueur = 0;	// Nombre de joueurs
	for (int i = 0; i < 4; i++) {
		tab[i].ip_addr = "my ip addr";
		tab[i].nom = "my name";
		tab[i].numport = 12345;
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

    listen(sockfd,5);
	while (1)
	{    
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
		{
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

		printf("Received packet from %s:%d\nData: [%s]\n\n",
		inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);
		printf("Premier caractère : %c\n", buffer[0]);
		if (buffer[0] == 'C') {		// Première connexion
			traitement_C(buffer, &tab, &nb_joueur);

			printf("nb_joueur : %d\n", nb_joueur);
			printf("Addresse du joueur %d : %s\n", nb_joueur, tab[nb_joueur-1].ip_addr);
			printf("Numéro de port de J%d : %d\n", nb_joueur, tab[nb_joueur-1].numport);
			printf("Nom de J%d : %s\n", nb_joueur, tab[nb_joueur-1].nom);
		}
		else if (buffer[0] == 'O') {		// Action 1 (all)
			
		}
		else if (buffer[0] == 'S') {		// Action 2 (joueur x)
			
		}
		else if (buffer[0] == 'G') {		// Action 3 (accusation)
			
		}
		else if (buffer[0] == 'M') {		// Message
			
		}
		else if (buffer[0] == 'F') {		// Fin
			
		}

		close(newsockfd);
	}

	close(sockfd);
	return 0; 
}
