#ifndef JEU_H
#define JEU_H

// On inclut la carte car la fonction de collision a besoin 
// de connaître la grille, TILE_SIZE, etc.
#include "carte.h" 

// --- Variables du Joueur ---
extern int joueur_x;
extern int joueur_y;
extern int vitesse;
extern int move_haut;
extern int move_bas;
extern int move_gauche;
extern int move_droite;

// --- Variables du Bot ---
extern int bot_x;
extern int bot_y;
extern int bot_vx;
extern int bot_vy;
extern int vitesse_bot;
extern int bot_panique;



// --- Prototypes des fonctions liées au jeu ---
int est_en_collision(char * user, int x, int y);
int calculer_distance(int x1, int y1, int x2, int y2);
void trouver_safe_place(int bot_grille_x, int bot_grille_y, int *cible_x, int *cible_y);
void algorithme_a_etoile(int start_x, int start_y, int dest_x, int dest_y);


#endif // JEU_H
