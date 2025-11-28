#ifndef PARTIE2_H
#define PARTIE2_H

#include "structures.h"

// Prototypes des fonctions de la Partie 2

// Pile
void initPile(Pile* p, int capacity);
int isPileEmpty(Pile* p);
void push(Pile* p, int v);
int pop(Pile* p);

// Tarjan
void parcours(int u, ListeAdjacence g, t_tarjan_vertex* sommets_info, Pile* pile, Partition* partition);
Partition executeTarjan(ListeAdjacence g);
void afficherPartition(Partition p);

// Hasse & Propriétés
ListeLiens creerLiensHasse(ListeAdjacence g, Partition p);
void analyserProprietes(Partition p, ListeLiens liens);
void genererHasseMermaid(Partition p, ListeLiens liens, const char* nom);

#endif