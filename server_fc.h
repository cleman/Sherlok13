#ifndef SERVER_FC
#define SERVER_FC

typedef struct {
    char *ip_addr;
    int numport;
    char *nom;
}client;

void traitement_C(char *buffer, client (*tab)[4], int *nb_joueur);

#endif
