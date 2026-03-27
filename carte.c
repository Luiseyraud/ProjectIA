#include "carte.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// --- Initialisation des variables ---
// C'est ici qu'elles sont réellement créées en mémoire
int grille_largeur = 600;
int grille_hauteur = 600;

// 0 = vide, 1 = obstacle, 2 = safe place
int carte[MAP_H][MAP_W] = {0};

// --- Logique de la carte ---

// initialiser la carte aléatoire (les obstacles)
void init_carte(void) {

    int x, y; 

    for ( y = 0; y < MAP_H; y++) {
        for ( x = 0; x < MAP_W; x++) {
            // "mur" au milieu de la map
            if (y == 10 && x >= 5 && x <= 15) {
                carte[y][x] = 1;
            }
        }
    }

    //Ajouter un "safe place" pour les poissons pour se cacher
    for (y = 0; y < MAP_H; y++) {
        for (x = 0; x < MAP_W; x++) {
            // "safe place" au bord de la map
            if (x <= 2 && y > 17 && y < 20 && x >=1) {
                carte[y][x] = 2;
            }
        }
    }
}
