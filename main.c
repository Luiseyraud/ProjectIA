#include "mongoose.h"
#include "jeu.h"
#include "carte.h"
#include "reseau.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h> // Ajouté pour le printf de démarrage

int main(void)
{
    // --- 1. Initialisation du Serveur et du Jeu ---
    struct mg_mgr gestionnaire;
    mg_mgr_init(&gestionnaire);
    // On utilise la fonction_evenement qui se trouve dans reseau.c
    mg_http_listen(&gestionnaire, "ws://localhost:8000", fonction_evenement, NULL);

    srand(time(NULL));

    // On crée les murs (fonction qui se trouve dans carte.c)
    init_carte();

    uint64_t temps_precedent = mg_millis();
    uint64_t temps_precedent_bot = mg_millis();
    
    printf("Serveur demarre sur ws://localhost:8000\n");
    printf("En attente de joueurs...\n");

    // --- 2. La Boucle Principale ---
    for (;;)
    {
        mg_mgr_poll(&gestionnaire, 2);
        uint64_t temps_actuel = mg_millis();

        // LA BOUCLE DE JEU (60 FPS)
        if (temps_actuel - temps_precedent >= 16)
        {
            temps_precedent = temps_actuel;

            // --- A. LE CERVEAU DU BOT (Toutes les secondes) ---
            if (temps_actuel - temps_precedent_bot >= 1000)
            {
                temps_precedent_bot = temps_actuel;
                bot_vx = (rand() % 5) - 2;
                bot_vy = (rand() % 5) - 2;
            }

            // --- B. LES JAMBES DU BOT (Déplacement avec Rebond) ---
            int bot_futur_x = bot_x + bot_vx;
            int bot_futur_y = bot_y + bot_vy;

            // On teste d'abord l'axe X (est_en_collision est dans jeu.c)
            if (est_en_collision(bot_futur_x, bot_y) == 1) {
                bot_vx = -bot_vx; 
            } else {
                bot_x = bot_futur_x; 
            }

            // On teste ensuite l'axe Y
            if (est_en_collision(bot_x, bot_futur_y) == 1) {
                bot_vy = -bot_vy; 
            } else {
                bot_y = bot_futur_y;
            }

            // --- C. MOUVEMENT DU JOUEUR (avec Glissement) ---
            int futur_x = joueur_x;
            int futur_y = joueur_y;

            // Ces variables (move_haut, etc.) sont modifiées par reseau.c quand un message Websocket arrive
            if (move_haut)    futur_y -= vitesse;
            if (move_bas)     futur_y += vitesse;
            if (move_gauche)  futur_x -= vitesse;
            if (move_droite)  futur_x += vitesse;

            // Test X (Glissement)
            if (est_en_collision(futur_x, joueur_y) == 0) {
                joueur_x = futur_x; 
            }

            // Test Y (Glissement)
            if (est_en_collision(joueur_x, futur_y) == 0) {
                joueur_y = futur_y; 
            }

            // --- D. DIFFUSION CONTINUE ---
            for (struct mg_connection *c = gestionnaire.conns; c != NULL; c = c->next)
            {
                if (c->is_websocket) {
                    // Fonction dans reseau.c
                    envoyer_etat_jeu(c);
                }
            }
        } 
    } 
    
    // Libération de la mémoire (bonne pratique même si la boucle est infinie)
    mg_mgr_free(&gestionnaire);
    return 0;
}