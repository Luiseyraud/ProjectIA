#include "reseau.h"
#include "jeu.h"
#include "carte.h"
#include <stdio.h>
#include <string.h>

// --- Fonctions d'envoi de données ---

// On envoie les données nécessaires à afficher la map en JSON
void envoyer_carte(struct mg_connection *c) {
    char buffer[2048];
    int offset = snprintf(buffer, sizeof(buffer), "{\"type\":\"map\",\"w\":%d,\"h\":%d,\"size\":%d,\"data\":[", MAP_W, MAP_H, TILE_SIZE);
    
    // on traduit la matrice pour le JSON (une grande liste)
    for(int y = 0; y < MAP_H; y++) {
        for(int x = 0; x < MAP_W; x++) {
            offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%d", carte[y][x]);
            // Ajouter une virgule si ce n'est pas le dernier élément
            if (y != MAP_H - 1 || x != MAP_W - 1) {
                offset += snprintf(buffer + offset, sizeof(buffer) - offset, ",");
            }
        }
    }
    snprintf(buffer + offset, sizeof(buffer) - offset, "]}");
    mg_ws_send(c, buffer, strlen(buffer), WEBSOCKET_OP_TEXT);
}

// On envoie la position des entités
void envoyer_etat_jeu(struct mg_connection *c) {
    char reponse[256];
    // On ajoute "bx" et "by" au format JSON
    snprintf(reponse, sizeof(reponse),
         "{\"type\": \"state\", \"x\": %d, \"y\": %d, \"bx\": %d, \"by\": %d, \"w\": %d, \"h\": %d, \"bp\":%d}",
         joueur_x, joueur_y, bot_x, bot_y, grille_largeur, grille_hauteur, bot_panique);

    mg_ws_send(c, reponse, strlen(reponse), WEBSOCKET_OP_TEXT);
}

// --- Gestionnaire d'événements Mongoose ---

void fonction_evenement(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        if (mg_match(hm->uri, mg_str("/"), NULL)) {
            mg_ws_upgrade(c, hm, NULL);
        }
    }
    else if (ev == MG_EV_WS_MSG) {
        struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
        char commande[64];
        snprintf(commande, sizeof(commande), "%.*s", (int)wm->data.len, wm->data.buf);

        // Mise à jour de l'état des touches (modifie les variables de jeu.h)
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
            
        // Modification des dimensions de la grille (variables de carte.h)
        else if (strncmp(commande, "RESIZE", 6) == 0) {
            sscanf(commande, "RESIZE %d %d", &grille_largeur, &grille_hauteur);
            envoyer_etat_jeu(c);
        }

        else if (strcmp(commande, "INIT") == 0) {
            envoyer_carte(c);
            envoyer_etat_jeu(c);
        }
    }
}
