#ifndef RESEAU_H
#define RESEAU_H

#include "mongoose.h"

// --- Prototypes des fonctions réseau ---
void envoyer_carte(struct mg_connection *c);
void envoyer_etat_jeu(struct mg_connection *c);
void fonction_evenement(struct mg_connection *c, int ev, void *ev_data);

#endif // RESEAU_H