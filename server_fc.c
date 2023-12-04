#include "server_fc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void traitement_C(char *buffer, client (*tab)[4], int *nb_joueur) {
    printf("Le message reçu \"%s\"\n", buffer);
    char *strToken;
    char *data[3];
    strToken = strtok(buffer, " ");
    for (int i = 0; i < 3; i++) {
        strToken = strtok(NULL, " ");
        data[i] = strToken;
    }
    // Attrivuer au tableau
    printf("nb in fc  : %d\n", *nb_joueur);
    (*tab)[*nb_joueur].ip_addr = strdup(data[0]);
    (*tab)[*nb_joueur].numport = atoi(data[1]);
    (*tab)[*nb_joueur].nom = strdup(data[2]);
    
    (*nb_joueur)++;
}