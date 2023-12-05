#ifndef SERVER_FC
#define SERVER_FC

typedef struct {
    char *ip_addr;
    int numport;
    char *nom;
}client;

void traitement_C(char *buffer, client (*tab)[4], int *nb_joueur);
void send_list(client (*tab)[4], int nb_joueur);
int send_message(char *ip_addr, int portno, char *message);
void print_card();
void shuffle_card();
void print_playerValue();
void bc(client(*tab)[4], char *message); // Broadcast

#endif
