#ifndef PARTIE1_H
#define PARTIE1_H

#include "structures.h"

liste creerListeVide();
cellule* create_cellule(int value, float proba);
void add_cellule(liste* L, int value, float proba);
void afficherListe(liste L);
ListeAdjacence creerListeAdjacenceVide(int taille);
ListeAdjacence readGraph(const char* filename);
void afficherListeAdjacence(ListeAdjacence g);
int verifierGrapheMarkov(ListeAdjacence g);
char* getId(int num);
void genererFichierMermaid(ListeAdjacence g, const char* nom);

#endif