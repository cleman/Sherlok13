#ifndef GLOBAL_VAR_H
#define GLOBAL_VAR_H

extern int myDeck[3];           // Cartes personnelles
extern int myId;                // ID
extern char playerName[4][256]; // Noms des joueurs
extern int myTurn;              // 1 : mon tour, 0 : pas mon tour
extern int tableCartes[4][8];   // Grille des figures
extern int quit;                // 1 : Fin des boucles while
extern char msgDisplay[128];    // Message à affiché sur l'IU
extern int joueur_actif;        // ID joueur actif
extern int coupable;            // ID personnage coupable
extern int gagnant;             // ID gagnant

#endif