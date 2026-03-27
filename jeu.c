#include "jeu.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MAX_NODES 1200 // Taille max de la grille (60x60)

typedef struct Node {
    int x, y;
    int g; // Coût du départ à ce nœud
    int h; // Estimation du coût de ce nœud à l'arrivée
    int f; // Score total (f = g + h)
    int parent_idx; // On stocke l'index dans un tableau plutôt qu'un pointeur pour optimiser l'espace
} Node;

Node nodePool[MAX_NODES];
int poolSize = 0;


// --- Initialisation des variables ---
int joueur_x = 100;
int joueur_y = 100;
int vitesse = 3; 
int move_haut = 0;
int move_bas = 0;
int move_gauche = 0;
int move_droite = 0;

int bot_x = 500;
int bot_y = 100;
int bot_vx = 0;
int bot_vy = 0;
int vitesse_bot = 2.7;
int bot_panique;

// --- Logique du jeu ---

// renvoie 1 s'il y a un mur, 0 sinon
int est_en_collision(char * user, int x, int y) {
    // le cercle est en fait un carré de 10px sur 10px (pour faciliter les calculs)
    int rayon = 10;
    
    // coordonnées en pixels des 4 coins du carré
    int gauche = x - rayon;
    int droite = x + rayon;
    int haut = y - rayon;
    int bas = y + rayon;
    
    // Sécurité : ne pas dépasser les limites de la map
    if (gauche < 0 || droite >= grille_largeur || haut < 0 || bas >= grille_hauteur) {
        return 1; // 1 = "Attention, mur détecté !"
    }
    
    // Division Entière pour trouver la case reliée à la matrice
    int case_gauche = gauche / TILE_SIZE;
    int case_droite = droite / TILE_SIZE;
    int case_haut = haut / TILE_SIZE;
    int case_bas = bas / TILE_SIZE;
    
     // l'un des 4 coins touche un '1' dans la matrice carte[y][x] ?
    if (strcmp(user,"bot") == 0){ //si les 2 chaines sont identiques...
        if (carte[case_haut][case_gauche] == 1 ) return 1;
        if (carte[case_haut][case_droite] == 1 ) return 1;
        if (carte[case_bas][case_gauche] == 1 ) return 1;
        if (carte[case_bas][case_droite] == 1 ) return 1; 
    }
    else{
        //rendre la safe place innatteignable au joueur
        if (carte[case_haut][case_gauche] == 1 || carte[case_haut][case_gauche] == 2) return 1;
        if (carte[case_haut][case_droite] == 1 || carte[case_haut][case_droite] == 2) return 1;
        if (carte[case_bas][case_gauche] == 1 || carte[case_bas][case_gauche] == 2) return 1;
        if (carte[case_bas][case_droite] == 1 || carte[case_bas][case_droite] == 2) return 1;
    }

    return 0; // si il ne touche pas de 1 alors c'est bon
}


// --- Principe de la "safe place" ---

// Fonction calcdule de distance simple
int calculer_distance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}


// Trouver la "Safe Place" la plus proche du bot
void trouver_safe_place(int bot_grille_x, int bot_grille_y, int *cible_x, int *cible_y) {
    float distance_min = 999999.0;
    int y, x, trouve = 0;
    float d;

    for (y = 0; y < MAP_H; y++) {
        for (x = 0; x < MAP_W; x++) {
            if (carte[y][x] == 2) {
                // Calcul de la distance entre le bot et cette case précise
                d = sqrt((x - bot_grille_x) * (x - bot_grille_x) + (y - bot_grille_y) * (y - bot_grille_y));
                
                if (d < distance_min) {
                    distance_min = d;
                    *cible_x = x;
                    *cible_y = y;
                    trouve = 1;
                }
            }
        }
    }
    
    // Si aucune zone 2 n'existe, on reste sur place
    if (!trouve) {
        *cible_x = bot_grille_x;
        *cible_y = bot_grille_y;
    }
}

//Recherche du chemin le plus court
void algorithme_a_etoile(int start_x, int start_y, int dest_x, int dest_y) {
    int i;
    poolSize = 0;
    int openList[MAX_NODES];
    int openCount = 0;
    int closedList[MAP_W][MAP_H] = {0}; // Pour ne pas repasser sur la même case

    // Ajouter le départ
    nodePool[poolSize] = (Node){start_x, start_y, 0, calculer_distance(start_x, start_y, dest_x, dest_y), 0, -1};
    nodePool[poolSize].f = nodePool[poolSize].h;
    openList[openCount++] = poolSize++;

    while (openCount > 0) {
        // Trouver le meilleur nœud (F le plus bas)
        int bestIdxInOpen = 0;
        for (i = 1; i < openCount; i++) {
            if (nodePool[openList[i]].f < nodePool[openList[bestIdxInOpen]].f) bestIdxInOpen = i;
        }

        int actuelIdx = openList[bestIdxInOpen];
        Node actuel = nodePool[actuelIdx];

        // Si destination atteinte
        if (actuel.x == dest_x && actuel.y == dest_y) {
            // Remonter jusqu'au premier mouvement
            int tempIdx = actuelIdx;
            while (nodePool[tempIdx].parent_idx != -1 && nodePool[nodePool[tempIdx].parent_idx].parent_idx != -1) {
                tempIdx = nodePool[tempIdx].parent_idx;
            }
            // Appliquer la direction
            bot_vx = (nodePool[tempIdx].x - start_x) * vitesse_bot;
            bot_vy = (nodePool[tempIdx].y - start_y) * vitesse_bot;
            return;
        }

        // Retirer de openList et marquer comme fermé
        openList[bestIdxInOpen] = openList[--openCount];
        closedList[actuel.x][actuel.y] = 1;

        // LES 8 DIRECTIONS (Horizontales + Diagonales)
        int dx[] = {0, 0, 1, -1, 1, 1, -1, -1};
        int dy[] = {1, -1, 0, 0, 1, -1, 1, -1};

        for (i = 0; i < 8; i++) {
            int nx = actuel.x + dx[i];
            int ny = actuel.y + dy[i];

            // Vérification classique (murs et limites)
            if (nx < 0 || nx >= MAP_W || ny < 0 || ny >= MAP_H || carte[ny][nx] == 1 || closedList[nx][ny]) continue;

            //Coût : 10 pour droit, 14 pour diagonale)
            int coutMouvement = (dx[i] != 0 && dy[i] != 0) ? 14 : 10;

            // AJOUT DE LA PÉNALITÉ "JOUEUR": eviter que le bot fonce sur le joueur
            // On convertit la position du joueur en coordonnées grille
            int j_grid_x = joueur_x / TILE_SIZE;
            int j_grid_y = joueur_y / TILE_SIZE;

            // On calcule la distance entre le voisin testé (nx, ny) et le joueur
            int dist_au_joueur = abs(nx - j_grid_x) + abs(ny - j_grid_y);

            if (dist_au_joueur < 4) { // Si la case testée est à moins de 4 cases du joueur
                // Plus on est proche, plus c'est cher (Ex: 100 / 1 = 100 de malus)
                coutMouvement += (100 / (dist_au_joueur + 1)); 
            }

            int gScore = actuel.g + coutMouvement;

            // Ajouter au pool
            if (poolSize < MAX_NODES) {
                nodePool[poolSize] = (Node){nx, ny, gScore, calculer_distance(nx, ny, dest_x, dest_y) * 10, 0, actuelIdx};
                nodePool[poolSize].f = nodePool[poolSize].g + nodePool[poolSize].h;
                openList[openCount++] = poolSize++;
            }
        }
    }
}
