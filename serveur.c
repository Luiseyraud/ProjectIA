#include "mongoose.h"
#include <stdio.h>
#include <string.h>

int joueur_x = 300, joueur_y = 300;
int grille_largeur = 600, grille_hauteur = 600;
int vitesse = 4; // Vitesse stable
int move_haut = 0, move_bas = 0, move_gauche = 0, move_droite = 0;

void envoyer_etat_jeu(struct mg_connection *c)
{
    char reponse[256];
    snprintf(reponse, sizeof(reponse), "{\"x\": %d, \"y\": %d, \"w\": %d, \"h\": %d}",
             joueur_x, joueur_y, grille_largeur, grille_hauteur);
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

        else if (strcmp(commande, "INIT") == 0)
            envoyer_etat_jeu(c);
    }
}

int main(void)
{
    struct mg_mgr gestionnaire;
    mg_mgr_init(&gestionnaire);
    mg_http_listen(&gestionnaire, "ws://localhost:8000", fonction_evenement, NULL);

    // NOUVEAU : On enregistre le temps au démarrage
    uint64_t temps_precedent = mg_millis();

    for (;;)
    {
        // Le serveur écoute le réseau avec une micro-pause max de 2ms
        mg_mgr_poll(&gestionnaire, 2);

        // NOUVEAU : On regarde quelle heure il est maintenant
        uint64_t temps_actuel = mg_millis();

        // Si (et SEULEMENT si) 16 millisecondes se sont écoulées (1000ms / 60 FPS = 16.6ms)
        if (temps_actuel - temps_precedent >= 16)
        {

            // On met à jour le chronomètre pour le prochain tour
            temps_precedent = temps_actuel;

            int a_bouge = 0;
            if (move_haut)
            {
                joueur_y -= vitesse;
                a_bouge = 1;
            }
            if (move_bas)
            {
                joueur_y += vitesse;
                a_bouge = 1;
            }
            if (move_gauche)
            {
                joueur_x -= vitesse;
                a_bouge = 1;
            }
            if (move_droite)
            {
                joueur_x += vitesse;
                a_bouge = 1;
            }

            if (a_bouge)
            {
                // Collisions
                if (joueur_x < 0)
                    joueur_x = 0;
                if (joueur_y < 0)
                    joueur_y = 0;
                if (joueur_x > grille_largeur)
                    joueur_x = grille_largeur;
                if (joueur_y > grille_hauteur)
                    joueur_y = grille_hauteur;

                // On envoie le message maximum 60 fois par seconde !
                for (struct mg_connection *c = gestionnaire.conns; c != NULL; c = c->next)
                {
                    if (c->is_websocket)
                        envoyer_etat_jeu(c);
                }
            }
        }
    }

    mg_mgr_free(&gestionnaire);
    return 0;
}