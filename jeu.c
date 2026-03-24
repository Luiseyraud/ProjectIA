#include "jeu.h"

// --- Initialisation des variables ---
int joueur_x = 200;
int joueur_y = 200;
int vitesse = 3; 
int move_haut = 0;
int move_bas = 0;
int move_gauche = 0;
int move_droite = 0;

int bot_x = 100;
int bot_y = 100;
int bot_vx = 0;
int bot_vy = 0;

// --- Logique du jeu ---

// renvoie 1 s'il y a un mur, 0 sinon
int est_en_collision(int x, int y) {
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
    if (carte[case_haut][case_gauche] == 1) return 1;
    if (carte[case_haut][case_droite] == 1) return 1;
    if (carte[case_bas][case_gauche] == 1) return 1;
    if (carte[case_bas][case_droite] == 1) return 1;
    
    return 0; // si il ne touche pas de 1 alors c'est bon
}