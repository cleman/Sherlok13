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

char buffer_server[256];
int my_server_numport;
char my_server_adress;
char *server_address;
int nb_carte = 0;
int synchro_L = 0;
int isWantCopytable = 0;

#include "global_var.h"

void create_table() {
    for (int j = 0; j < 8; j++) {
        tableCartes[myId][j] = 0;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 8; j++) {
            tableCartes[myId][j] += cardsValue[myDeck[i]][j];
        }
    }
}

void print_playerValue() {
    for (int i = 0; i < 4; i++) {
        printf("Joueur %d : ", i);
        for (int j = 0; j < 8; j++) {
            if (tableCartes[i][j] != -1 && tableCartes[i][j] != 100) printf("0%d ", tableCartes[i][j]);
            else if (tableCartes[i][j] == 100) printf("** ");
            else printf("%d ", tableCartes[i][j]);
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
	while (!quit)
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

        // Reception carte
        if (buffer_server[0] == 'V') {
            int x;
            sscanf(buffer_server,"V %d",&x);
            myDeck[nb_carte] = x;
            nb_carte++;
            printf("Ma carte %d est %d\n", nb_carte,myDeck[nb_carte-1]);
        }
        // Recepetion id
        if (buffer_server[0] == 'I') {
            int x;
            sscanf(buffer_server, "I %d", &x);
            myId = x;
        }
        // Recepetion liste joueur
        if (buffer_server[0] == 'L') {
            char name[4][32];
            sscanf(buffer_server, "L %s %s %s %s", name[0], name[1], name[2], name[3]);
            strcpy(playerName[0], name[0]);
            strcpy(playerName[1], name[1]);
            strcpy(playerName[2], name[2]);
            strcpy(playerName[3], name[3]);

            synchro_L = 1;
            while (synchro_L);
        }
        // Reception debut de partie
        if (buffer_server[0] == 'D') {
            printf("D : La partie commence\n");
        }
        // Reception joueur actif
        if (buffer_server[0] == 'M') {
            printf("M : Joueur %c joue\n", buffer_server[2]);
            if (atoi(&buffer_server[2]) == myId) {
                myTurn = 1;
                //printf("C'est à mon tour de jouer\n");
            }
            else myTurn = 0;
        }

        // Retour action 1
        if (buffer_server[0] == 'R') {
            int obj, id_j, data;
            sscanf(buffer_server, "R %d %d %d", &id_j, &obj, &data);
            printf("R : Donnée reçu sur le joueur %d : %d\n", id_j, data);
            if (tableCartes[id_j][obj] == -1 || tableCartes[id_j][obj] == 100) {
                tableCartes[id_j][obj] = data;
            }
            print_playerValue();
        }

        // Proposition coupable
        if (buffer_server[0] == 'F') {
            if (atoi(&buffer_server[4]) == 1) {
                printf("F : Partie gagné par Joueur %d. Le coupable est %s\n", atoi(&buffer_server[2]), carteNom[atoi(&buffer_server[6])]);
                quit = 1;
                myTurn = 0;
            }
            else printf("F : Le joueur %d s'est trompé en denoncant %s\n", atoi(&buffer_server[2]), carteNom[atoi(&buffer_server[6])]);
        }
        
        // Message to display
        if (buffer_server[0] == 'T') {
            //sscanf(msgDisplay, "T %s");
            strcpy(msgDisplay, buffer_server + 2);
        }

		close(newsockfd);
	}

	close(sockfd);
    pthread_exit(NULL);
}