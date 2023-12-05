#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "client_fc.h"

char *carteNom[] = {"Sebastian Moran", 
                    "Irene Adler",
                    "inspector Lestrade",
                    "inspector Gregson",
                    "inspector Baynes",
                    "inspector Bradstreet",
                    "inspector Hopkins",
                    "Sherlok Holmes",
                    "John Watson",
                    "Mycroft Holmes",
                    "Mrs. Hudson",
                    "Mary Morstan",
                    "Jame Moriatry"};

int my_deck[3] = {-1,-1,-1};

int cardsValue[13][8] = {
    {0,0,1,0,0,0,0,1},
    {0,1,0,0,0,1,0,1},
    {0,0,0,1,1,0,1,0},
    {0,0,1,1,1,0,0,0},
    {0,1,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,0},
    {1,0,0,1,0,0,1,0},
    {1,1,1,0,0,0,0,0},
    {1,0,1,0,0,0,1,0},
    {1,1,0,0,1,0,0,0},
    {1,0,0,0,0,1,0,0},
    {0,0,0,0,1,1,0,0},
    {0,1,0,0,0,0,0,1}};

char *listeJoueur[] = {"-","-","-","-"};

char buffer_server[256];
int my_server_numport;
char my_server_adress;
char *server_address;
int nb_carte = 0;
int my_id = -1;
int synchro_L = 0;
int playerValue[4][8];  // Tableau 4*8 des figure des joueurs

void create_table() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 8; j++) {
            playerValue[i][j] = 0;
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 8; j++) {
            playerValue[my_id][j] += cardsValue[my_deck[i]][j];
        }
    }
}

void print_playerValue() {
    for (int i = 0; i < 4; i++) {
        printf("Joueur %d : ", i);
        for (int j = 0; j < 8; j++) {
            printf("0%d ", playerValue[i][j]);
        }
        printf("\n");
    }
}

int send_message(char *ip_addr, int portno, char *message) {
    //printf("adresse ip : %s\n", ip_addr);
    //printf("Port : %d\n", portno);
    //printf("message : %s\n", message);

    int sockfd;
    struct sockaddr_in serv_addr;
	struct hostent *server;

    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
	{
		fprintf(stderr,"Impossible d'ouvrir la socket. On arrête tout.\n");
		return 1;
	}
    
	server = gethostbyname(ip_addr);
    if (server == NULL) 
	{
        fprintf(stderr,"Impossible de récupérer les infos sur le serveur\n");
		return 1;
    }

	// On donne toutes les infos sur le serveur
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

	// On se connecte. L'OS local nous trouve un numéro de port, grâce auquel le serveur
	// peut nous renvoyer des réponses

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	{
		fprintf(stderr,"Impossible de faire l'appel system connect().\n");
		return 1;
	}

	sprintf(buffer,"%s\n",message);
    write(sockfd,buffer,strlen(buffer));

	// On ferme la socket

    close(sockfd);

    printf("Message envoyé\n");

    return 0;
}

void *server_tcp() {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;

	// 1) on crée la socket, SOCK_STREAM signifie TCP
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
	{
        fprintf(stderr,"Impossible d'ouvrir la socket.\n");
		exit(0);
	}

	// 2) on réclame au noyau l'utilisation du port passé en paramètre 
	// INADDR_ANY dit que la socket va être affectée à toutes les interfaces locales

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = my_server_numport;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
	{
        fprintf(stderr,"Impossible de faire l'appel système bind().\n");
		exit(0);
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
            exit(0);
    	}

        bzero(buffer_server,256);
        n = read(newsockfd,buffer_server,255);
        if (n < 0) 
        {
            fprintf(stderr,"Impossible de faire l'appel système read().\n");
            exit(0);
        }

        //printf("Received packet from %s:%d\nData: [%s]\n\n",
        //inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer_server);

        //send_message("127.0.0.1",32000, buffer_server);
        if (buffer_server[0] == 'V') {
            int x;
            sscanf(buffer_server,"V %d",&x);
            my_deck[nb_carte] = x;
            nb_carte++;
        }
        if (buffer_server[0] == 'I') {
            int x;
            sscanf(buffer_server, "I %d", &x);
            my_id = x;
        }
        if (buffer_server[0] == 'L') {
            char name[4][32];
            sscanf(buffer_server, "L %s %s %s %s", name[0], name[1], name[2], name[3]);
            listeJoueur[0] = name[0];
            listeJoueur[1] = name[1];
            listeJoueur[2] = name[2];
            listeJoueur[3] = name[3];

            synchro_L = 1;
            while (synchro_L);
        }
        if (buffer_server[0] == 'D') {
            printf("La partie commence\n");
        }

		close(newsockfd);
	}

	close(sockfd);
    pthread_exit(NULL);
}