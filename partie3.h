#ifndef PARTIE3_H
#define PARTIE3_H

#include "structures.h"

// Prototypes des fonctions de la Partie 3
t_matrix createMatrix(int n);
t_matrix graphToMatrix(ListeAdjacence g);
void freeMatrix(t_matrix m);
void printMatrix(t_matrix m);
void copyMatrix(t_matrix dest, t_matrix src);
t_matrix multiplyMatrices(t_matrix A, t_matrix B);
float diffMatrix(t_matrix A, t_matrix B);

#endif