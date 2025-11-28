#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// --- PARTIE 1 ---
typedef struct cellule {
    int c;
    float proba;
    struct cellule* next;
} cellule;

typedef struct liste {
    cellule* head;
} liste;

typedef struct ListeAdjacence {
    int taille;
    liste* tableau;
} ListeAdjacence;

// --- PARTIE 2 ---
typedef struct {
    int id;
    int num;
    int accessible;
    int dans_pile;
    int id_classe;
} t_tarjan_vertex;

typedef struct {
    int* array;
    int top;
    int capacity;
} Pile;

typedef struct classe {
    char name[10];
    int id_classe;
    int* sommets;
    int nb_sommets;
    int capacity;
} t_classe;

typedef struct {
    t_classe* classes;  
    int nb_classes;    
} Partition;

typedef struct lien {
    int depart;
    int arrivee;
} Lien;

typedef struct {
    Lien* array;
    int count;
    int capacity;
} ListeLiens;

// --- PARTIE 3 ---
typedef struct {
    int size;
    float** data;
} t_matrix;

#endif