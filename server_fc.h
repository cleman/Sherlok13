#ifndef SERVER_FC
#define SERVER_FC

typedef struct {
    char *ip_addr;
    int numport;
    char *nom;
}client;

void traitement_C(char *buffer, client (*tab)[4], int *nb_joueur);  // Traitement message type C
void send_list(client (*tab)[4], int nb_joueur);                    // Envoi liste joueur
int send_message(char *ip_addr, int portno, char *message);         // Envoi message personnel
void print_card();                                                  // Affichage des cartes
void shuffle_card();                                                // Melange des cartes
void print_playerValue();                                           // Affichage de la grille des figures
void bc(client(*tab)[4], char *message);                            // Broadcast
void fc_act_1(client (*tab)[4], char *buffer, int joueur_actif);    // Traitement action 1
void fc_act_2(client (*tab)[4], char *buffer);                      // Traitement action 2
void fc_act_3(client (*tab)[4], char *buffer);                      // Traitement action 3

#endif