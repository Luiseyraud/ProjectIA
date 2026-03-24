#ifndef CARTE_H
#define CARTE_H

// --- Constantes de la carte ---
#define TILE_SIZE 30
#define MAP_W 20
#define MAP_H 20

// --- Variables de la carte ---
// On utilise extern pour annoncer aux autres fichiers que ces variables existent
extern int grille_largeur;
extern int grille_hauteur;
extern int carte[MAP_H][MAP_W];

// --- Prototypes ---
void init_carte(void);

#endif // CARTE_H