#include "server_fc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

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

int deck[13] = {0,1,2,3,4,5,6,7,8,9,10,11,12};

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

int playerValue[4][8];
int quit = 0;

// Fonction lié à l'action 1
void fc_act_1(client (*tab)[4], char *buffer, int joueur_actif) {
    int k = atoi(&buffer[4]);
    printf("Action 1 pour l'objet %d\n",k);
    for (int i = 0; i < 4; i++) {
        if (i != joueur_actif) {
            char msg_R[64];
            int c;
            if (playerValue[i][k] == 0) c = 0;
            else c = 100;
            sprintf(msg_R, "R %d %d %d", i, k, c);
            bc(tab, msg_R);
        }
    }
}

// Fonction lié à l'action 2
void fc_act_2(client (*tab)[4], char *buffer) {
    int joueur = atoi(&buffer[4]);
    int obj = atoi(&buffer[6]);
    printf("Action 2 pour le joueur %d sur l'objet %d\n", joueur, obj);
    char msg_R[64];
    sprintf(msg_R, "R %d %d %d", joueur, obj, playerValue[joueur][obj]);
    bc(tab, msg_R);
}

// Fonction lié à l'action 3
void fc_act_3(client (*tab)[4], char *buffer) {
    int k = atoi(&buffer[4]);
    printf("Action 3 sur %s\n", carteNom[k]);
    int id_G = atoi(&buffer[2]); // Joueur qui a guilt
    char msg_F[64];
    if (k != deck[12]) {
        printf("Mauvaise proposition\n");
        sprintf(msg_F,"F %d 0 %d", id_G, k);   // Mauvaise réponse
    }
    else {
        printf("Bonne proposition\n");
        sprintf(msg_F,"F %d 1 %d", id_G, k);  // Bonne réponse, partie finie
        quit = 1;

        char msg_T[128];
        sprintf(msg_T, "T %s a trouve le coupable : %s ", (*tab)[id_G].nom, carteNom[k]);
        bc(tab, msg_T);
    }
    bc(tab, msg_F);
    sleep(1);
}

void print_card() {
    for (int i = 0; i < 13; i++) {
        printf("Carte %d : %s\n", deck[i], carteNom[deck[i]]);
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

void shuffle_card() {
    srand(time(NULL));
    int index1, index2, temp;
    for (int i = 0; i < 1000; i++) {
        index1 = rand()%13;
        index2 = rand()%13;

        temp = deck[index1];
        deck[index1] = deck[index2];
        deck[index2] = temp;
    }
    // Initialisation de playerValue
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            playerValue[i][j] = 0;
        }
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            
            for (int k = 0; k < 8; k++) {
                playerValue[i][k] += cardsValue[deck[i*3+j]][k];
            }
        }
    }
}

void traitement_C(char *buffer, client (*tab)[4], int *nb_joueur) {
    //printf("Le message reçu \"%s\"\n", buffer);
    char data[3][64];
    sscanf(buffer, "C %s %s %s", data[0], data[1], data[2]);
    // Attrivuer au tableau
    //printf("nb in fc  : %d\n", *nb_joueur);
    (*tab)[*nb_joueur].ip_addr = strdup(data[0]);
    (*tab)[*nb_joueur].numport = atoi(data[1]);
    (*tab)[*nb_joueur].nom = strdup(data[2]);
    
    (*nb_joueur)++;
}

void send_list(client (*tab)[4], int nb_joueur) {
    char msg[128];
    sprintf(msg, "L %s %s %s %s", (*tab)[0].nom, (*tab)[1].nom, (*tab)[2].nom, (*tab)[3].nom);
    for (int i = 0; i < nb_joueur; i++) {
        send_message((*tab)[i].ip_addr, (*tab)[i].numport, msg);
    }
}


void bc(client(*tab)[4], char *message) {
    for (int i = 0; i < 4; i++) {
        send_message((*tab)[i].ip_addr, (*tab)[i].numport, message);
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

    //printf("Message envoyé\n");

    return 0;
}