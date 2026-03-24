#include "carte.h"

// --- Initialisation des variables ---
// C'est ici qu'elles sont réellement créées en mémoire
int grille_largeur = 600;
int grille_hauteur = 600;

// 0 = vide, 1 = obstacle
int carte[MAP_H][MAP_W] = {0};

// --- Logique de la carte ---

// initialiser la carte (les obstacles)
void init_carte(void) {
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            // "mur" au milieu de la map
            if (x == 10 && y >= 5 && y <= 15) {
                carte[y][x] = 1;
            }
        }
    }
}