#include "mongoose.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int joueur_x = 300, joueur_y = 300;
int grille_largeur = 600, grille_hauteur = 600;
int vitesse = 3; // Vitesse stable
int move_haut = 0, move_bas = 0, move_gauche = 0, move_droite = 0;
int bot_x = 100, bot_y = 100;
int bot_vx = 0, bot_vy = 0;

void envoyer_etat_jeu(struct mg_connection *c)
{
    char reponse[256];
    // On ajoute "bx" et "by" au format JSON
    snprintf(reponse, sizeof(reponse),
             "{\"x\": %d, \"y\": %d, \"bx\": %d, \"by\": %d, \"w\": %d, \"h\": %d}",
             joueur_x, joueur_y, bot_x, bot_y, grille_largeur, grille_hauteur);

    mg_ws_send(c, reponse, strlen(reponse), WEBSOCKET_OP_TEXT);
}

static void fonction_evenement(struct mg_connection *c, int ev, void *ev_data)
{
    if (ev == MG_EV_HTTP_MSG)
    {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        if (mg_match(hm->uri, mg_str("/"), NULL))
        {
            mg_ws_upgrade(c, hm, NULL);
        }
    }
    else if (ev == MG_EV_WS_MSG)
    {
        struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
        char commande[64];
        snprintf(commande, sizeof(commande), "%.*s", (int)wm->data.len, wm->data.buf);

        // Mise à jour de l'état des touches
        if (strcmp(commande, "+HAUT") == 0)
            move_haut = 1;
        else if (strcmp(commande, "-HAUT") == 0)
            move_haut = 0;

        else if (strcmp(commande, "+BAS") == 0)
            move_bas = 1;
        else if (strcmp(commande, "-BAS") == 0)
            move_bas = 0;

        else if (strcmp(commande, "+GAUCHE") == 0)
            move_gauche = 1;
        else if (strcmp(commande, "-GAUCHE") == 0)
            move_gauche = 0;

        else if (strcmp(commande, "+DROITE") == 0)
            move_droite = 1;
        else if (strcmp(commande, "-DROITE") == 0)
            move_droite = 0;
        else if (strncmp(commande, "RESIZE", 6) == 0)
        {
            sscanf(commande, "RESIZE %d %d", &grille_largeur, &grille_hauteur);
            envoyer_etat_jeu(c);
        }

        else if (strcmp(commande, "INIT") == 0)
            envoyer_etat_jeu(c);
    }
}

int main(void)
{
    struct mg_mgr gestionnaire;
    mg_mgr_init(&gestionnaire);
    mg_http_listen(&gestionnaire, "ws://localhost:8000", fonction_evenement, NULL);

    srand(time(NULL));

    uint64_t temps_precedent = mg_millis();
    uint64_t temps_precedent_bot = mg_millis();

    for (;;)
    {
        mg_mgr_poll(&gestionnaire, 2);
        uint64_t temps_actuel = mg_millis();

        // LA BOUCLE DE JEU (60 FPS)
        if (temps_actuel - temps_precedent >= 16)
        {
            temps_precedent = temps_actuel;

            // --- 1. LE CERVEAU DU BOT (Toutes les 1000 millisecondes = 1 seconde) ---
            if (temps_actuel - temps_precedent_bot >= 1000)
            {
                temps_precedent_bot = temps_actuel;

                // On donne une vitesse aléatoire entre -2 et +2
                bot_vx = (rand() % 5) - 2;
                bot_vy = (rand() % 5) - 2;
            }

            // --- 2. LES JAMBES DU BOT (Déplacement 60 fois par seconde) ---
            bot_x += bot_vx;
            bot_y += bot_vy;

            // Rebondir contre les murs
            if (bot_x <= 0 || bot_x >= grille_largeur)
                bot_vx = -bot_vx;
            if (bot_y <= 0 || bot_y >= grille_hauteur)
                bot_vy = -bot_vy;

            // --- 3. MOUVEMENT DU JOUEUR ---
            if (move_haut)
                joueur_y -= vitesse;
            if (move_bas)
                joueur_y += vitesse;
            if (move_gauche)
                joueur_x -= vitesse;
            if (move_droite)
                joueur_x += vitesse;

            // Collisions du joueur
            if (joueur_x < 0)
                joueur_x = 0;
            if (joueur_y < 0)
                joueur_y = 0;
            if (joueur_x > grille_largeur)
                joueur_x = grille_largeur;
            if (joueur_y > grille_hauteur)
                joueur_y = grille_hauteur;

            // --- 4. DIFFUSION CONTINUE ---
            // On envoie le message maximum 60 fois par seconde à tout le monde !
            for (struct mg_connection *c = gestionnaire.conns; c != NULL; c = c->next)
            {
                if (c->is_websocket)
                    envoyer_etat_jeu(c);
            }
        } // Fin du "if (temps_actuel - temps_precedent >= 16)"
    } // Fin du "for (;;)"
}