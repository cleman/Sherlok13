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

char buffer_server[256];
int my_server_numport;
char my_server_adress;
char *server_address;

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

        printf("Received packet from %s:%d\nData: [%s]\n\n",
        inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer_server);

        send_message("127.0.0.1",32000, buffer_server);

		close(newsockfd);
	}

	close(sockfd);
    pthread_exit(NULL);
}