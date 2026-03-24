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

// --- Prototypes des fonctions liées au jeu ---
int est_en_collision(int x, int y);

#endif // JEU_H