#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Inclusion des fichiers d'en-tête créés précédemment
#include "structures.h"
#include "partie1.h"
#include "partie2.h"
#include "partie3.h"

/* --------------------------------------------------------------------------------
   VARIABLES GLOBALES (Nécessaires pour Tarjan)
-------------------------------------------------------------------------------- */
int num_compteur;       
int class_compteur;     
int* vertex_to_class_map; 

/* --------------------------------------------------------------------------------
   IMPLEMENTATION PARTIE 1 (Graphe)
-------------------------------------------------------------------------------- */

liste creerListeVide() {
    liste L;
    L.head = NULL;
    return L;
}

cellule* create_cellule(int value, float proba) {
    cellule *c = (cellule*)malloc(sizeof(cellule));
    c->c = value;
    c->proba = proba;
    c->next = NULL;
    return c;
}

void add_cellule(liste* L, int value, float proba) {
    cellule* newc = create_cellule(value, proba);
    newc->next = L->head; 
    L->head = newc;
}

void afficherListe(liste L) {
    cellule* cur = L.head;
    while (cur != NULL) {
        printf(" -> (%d, %.2f)", cur->c, cur->proba);
        cur = cur->next;
    }
    printf("\n");
}

ListeAdjacence creerListeAdjacenceVide(int taille) {
    ListeAdjacence g;
    g.taille = taille;
    g.tableau = (liste*)malloc(sizeof(liste) * taille);
    if (!g.tableau) {
        perror("Erreur malloc tableau de listes");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < taille; i++) {
        g.tableau[i] = creerListeVide();
    }
    return g;
}

ListeAdjacence readGraph(const char* filename) {
    FILE* file = fopen(filename, "rt");
    int nbvert, depart, arrivee;
    float proba;
    ListeAdjacence g;

    if (file == NULL) {
        perror("Impossible d'ouvrir le fichier");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", &nbvert);
    g = creerListeAdjacenceVide(nbvert);

    while (fscanf(file, "%d %d %f", &depart, &arrivee, &proba) == 3) {
        add_cellule(&g.tableau[depart - 1], arrivee, proba);
    }

    fclose(file);
    return g;
}

void afficherListeAdjacence(ListeAdjacence g) {
    printf("\n=== LISTE D'ADJACENCE ===\n");
    for (int i = 0; i < g.taille; i++) {
        printf("Sommet %d :", i + 1);
        afficherListe(g.tableau[i]);
    }
}

int verifierGrapheMarkov(ListeAdjacence g) {
    int ok = 1;
    printf("\n=== VERIFICATION MARKOV ===\n");

    for (int i = 0; i < g.taille; i++) {
        float somme = 0;
        cellule* cur = g.tableau[i].head;
        while (cur != NULL) {
            somme += cur->proba;
            cur = cur->next;
        }
        if (somme < 0.99 || somme > 1.01) {
            ok = 0;
            printf("Sommet %d : Somme = %.2f \n", i + 1, somme);
        } else {
            printf("Sommet %d : Somme = %.2f \n", i + 1, somme);
        }
    }
    if (ok) printf("\n Le graphe est bien un graphe de Markov\n");
    else    printf("\n Le graphe n'est PAS un graphe de Markov\n");
    return ok;
}

char* getId(int num) {
    static char buffer[10];
    char* p = &buffer[9];
    *p = '\0';
    int n = num;
    do {
        p--;
        *p = (n - 1) % 26 + 'A'; 
        n = (n - 1) / 26;        
    } while (n > 0);
    return p;
}

void genererFichierMermaid(ListeAdjacence g, const char* nom) {
    FILE* f = fopen(nom, "wt");
    if (!f) {
        perror("Erreur création fichier Mermaid");
        return;
    }
    fprintf(f, "graph LR\n"); 
    for (int i = 1; i <= g.taille; i++) {
        fprintf(f, "  %s((%d))\n", getId(i), i); 
    }
    for (int i = 0; i < g.taille; i++) {
        cellule* cur = g.tableau[i].head;
        while (cur != NULL) {
            fprintf(f, "  %s", getId(i + 1));        
            fprintf(f, " -->|%.2f| ", cur->proba);   
            fprintf(f, "%s\n", getId(cur->c));       
            cur = cur->next;
        }
    }
    fclose(f);
    printf("\n Fichier Mermaid généré : %s\n", nom);
}

/* --------------------------------------------------------------------------------
   IMPLEMENTATION PARTIE 2 (Tarjan & Hasse)
-------------------------------------------------------------------------------- */

void initPile(Pile* p, int capacity) {
    p->array = (int*)malloc(capacity * sizeof(int));
    p->top = -1; 
    p->capacity = capacity;
}

int isPileEmpty(Pile* p) {
    return p->top == -1;
}

void push(Pile* p, int v) {
    if (p->top < p->capacity - 1) {
        p->array[++(p->top)] = v;
    }
}

int pop(Pile* p) {
    if (!isPileEmpty(p)) {
        return p->array[(p->top)--];
    }
    return -1; 
}

void parcours(int u, ListeAdjacence g, t_tarjan_vertex* sommets_info, Pile* pile, Partition* partition) {
    sommets_info[u].num = num_compteur;
    sommets_info[u].accessible = num_compteur;
    num_compteur++;
    
    push(pile, u);
    sommets_info[u].dans_pile = 1;

    cellule* voisin = g.tableau[u].head;
    while (voisin != NULL) {
        int v = voisin->c - 1; 

        if (sommets_info[v].num == -1) { 
            parcours(v, g, sommets_info, pile, partition);
            if (sommets_info[v].accessible < sommets_info[u].accessible) {
                sommets_info[u].accessible = sommets_info[v].accessible;
            }
        } else if (sommets_info[v].dans_pile) { 
            if (sommets_info[v].num < sommets_info[u].accessible) {
                sommets_info[u].accessible = sommets_info[v].num;
            }
        }
        voisin = voisin->next;
    }

    if (sommets_info[u].accessible == sommets_info[u].num) {
        class_compteur++;
        partition->nb_classes++;
        partition->classes = (t_classe*)realloc(partition->classes, partition->nb_classes * sizeof(t_classe));
        
        int idx_classe = partition->nb_classes - 1;
        sprintf(partition->classes[idx_classe].name, "C%d", class_compteur);
        partition->classes[idx_classe].id_classe = idx_classe;
        partition->classes[idx_classe].nb_sommets = 0;
        partition->classes[idx_classe].sommets = NULL;

        int w;
        do {
            w = pop(pile);
            sommets_info[w].dans_pile = 0; 
            
            t_classe* current_class = &partition->classes[idx_classe];
            current_class->nb_sommets++;
            current_class->sommets = (int*)realloc(current_class->sommets, current_class->nb_sommets * sizeof(int));
            current_class->sommets[current_class->nb_sommets - 1] = sommets_info[w].id;
            
            vertex_to_class_map[w] = idx_classe;

        } while (w != u);
    }
}

Partition executeTarjan(ListeAdjacence g) {
    t_tarjan_vertex* sommets_info = (t_tarjan_vertex*)malloc(g.taille * sizeof(t_tarjan_vertex));
    vertex_to_class_map = (int*)malloc(g.taille * sizeof(int)); 
    
    Pile pile;
    initPile(&pile, g.taille);
    
    Partition partition;
    partition.classes = NULL;
    partition.nb_classes = 0;
    
    num_compteur = 0;
    class_compteur = 0;

    for (int i = 0; i < g.taille; i++) {
        sommets_info[i].id = i + 1;
        sommets_info[i].num = -1; 
        sommets_info[i].accessible = -1;
        sommets_info[i].dans_pile = 0;
        vertex_to_class_map[i] = -1;
    }

    for (int i = 0; i < g.taille; i++) {
        if (sommets_info[i].num == -1) {
            parcours(i, g, sommets_info, &pile, &partition);
        }
    }

    free(sommets_info);
    free(pile.array);
    return partition;
}

void afficherPartition(Partition p) {
    printf("\n=== RESULTATS TARJAN (Composantes Connexes) ===\n");
    for (int i = 0; i < p.nb_classes; i++) {
        printf("Composante %s : {", p.classes[i].name);
        for (int j = 0; j < p.classes[i].nb_sommets; j++) {
            printf("%d", p.classes[i].sommets[j]);
            if (j < p.classes[i].nb_sommets - 1) printf(", ");
        }
        printf("}\n");
    }
}

ListeLiens creerLiensHasse(ListeAdjacence g, Partition p) {
    ListeLiens liens;
    liens.count = 0;
    liens.capacity = 10;
    liens.array = (Lien*)malloc(liens.capacity * sizeof(Lien));

    for (int u = 0; u < g.taille; u++) { 
        int class_u = vertex_to_class_map[u]; 
        
        cellule* curr = g.tableau[u].head;
        while (curr != NULL) {
            int v = curr->c - 1;
            int class_v = vertex_to_class_map[v]; 

            if (class_u != class_v) { 
                int existe = 0;
                for (int k = 0; k < liens.count; k++) {
                    if (liens.array[k].depart == class_u && liens.array[k].arrivee == class_v) {
                        existe = 1;
                        break;
                    }
                }
                if (!existe) {
                    if (liens.count >= liens.capacity) {
                        liens.capacity *= 2;
                        liens.array = (Lien*)realloc(liens.array, liens.capacity * sizeof(Lien));
                    }
                    liens.array[liens.count].depart = class_u;
                    liens.array[liens.count].arrivee = class_v;
                    liens.count++;
                }
            }
            curr = curr->next;
        }
    }
    return liens;
}

void analyserProprietes(Partition p, ListeLiens liens) {
    printf("\n=== PROPRIETES DES CLASSES ===\n");
    
    int* a_des_sortants = (int*)calloc(p.nb_classes, sizeof(int)); 
    for (int i = 0; i < liens.count; i++) {
        a_des_sortants[liens.array[i].depart] = 1; 
    }
    int irreductible = (p.nb_classes == 1); 

    for (int i = 0; i < p.nb_classes; i++) {
        printf("Classe %s : ", p.classes[i].name);
        if (a_des_sortants[i]) {
            printf("TRANSITOIRE (on peut en sortir)\n"); 
        } else {
            printf("PERSISTANTE (on ne peut pas en sortir)"); 
            if (p.classes[i].nb_sommets == 1) {
                printf(" -> Etat ABSORBANT (%d)\n", p.classes[i].sommets[0]); 
            } else {
                printf("\n");
            }
        }
    }
    if (irreductible) printf("\n=> Le graphe est IRREDUCTIBLE.\n");
    else printf("\n=> Le graphe n'est PAS irréductible.\n");

    free(a_des_sortants);
}

void genererHasseMermaid(Partition p, ListeLiens liens, const char* nom) {
    FILE* f = fopen(nom, "wt");
    if (!f) return;

    fprintf(f, "graph TD\n");
    for (int i = 0; i < p.nb_classes; i++) {
        fprintf(f, "  %s[\"{", p.classes[i].name);
        for (int j = 0; j < p.classes[i].nb_sommets; j++) {
            fprintf(f, "%d", p.classes[i].sommets[j]);
            if (j < p.classes[i].nb_sommets - 1) fprintf(f, ",");
        }
        fprintf(f, "}\"]\n");
    }
    for (int i = 0; i < liens.count; i++) {
        fprintf(f, "  %s --> %s\n", 
                p.classes[liens.array[i].depart].name,
                p.classes[liens.array[i].arrivee].name);
    }
    fclose(f);
    printf("\n Fichier Hasse Mermaid généré : %s\n", nom);
}

/* --------------------------------------------------------------------------------
   IMPLEMENTATION PARTIE 3 (Matrices)
-------------------------------------------------------------------------------- */

t_matrix createMatrix(int n) {
    t_matrix m;
    m.size = n;
    m.data = (float**)malloc(n * sizeof(float*));
    for (int i = 0; i < n; i++) {
        m.data[i] = (float*)calloc(n, sizeof(float)); 
    }
    return m;
}

t_matrix graphToMatrix(ListeAdjacence g) {
    t_matrix m = createMatrix(g.taille);
    for (int i = 0; i < g.taille; i++) { 
        cellule* curr = g.tableau[i].head;
        while (curr != NULL) {
            m.data[i][curr->c - 1] = curr->proba; 
            curr = curr->next;
        }
    }
    return m;
}

void freeMatrix(t_matrix m) {
    for (int i = 0; i < m.size; i++) free(m.data[i]);
    free(m.data);
}

void printMatrix(t_matrix m) {
    printf("\nMatrice (%dx%d) :\n", m.size, m.size);
    for (int i = 0; i < m.size; i++) {
        printf("[ ");
        for (int j = 0; j < m.size; j++) {
            printf("%6.3f ", m.data[i][j]);
        }
        printf("]\n");
    }
}

void copyMatrix(t_matrix dest, t_matrix src) {
    for(int i=0; i<src.size; i++) {
        for(int j=0; j<src.size; j++) {
            dest.data[i][j] = src.data[i][j];
        }
    }
}

t_matrix multiplyMatrices(t_matrix A, t_matrix B) {
    if (A.size != B.size) {
        printf("Erreur dimension matrices\n");
        exit(1);
    }
    t_matrix R = createMatrix(A.size);
    for (int i = 0; i < A.size; i++) {
        for (int j = 0; j < A.size; j++) {
            for (int k = 0; k < A.size; k++) {
                R.data[i][j] += A.data[i][k] * B.data[k][j];
            }
        }
    }
    return R;
}

float diffMatrix(t_matrix A, t_matrix B) {
    float diff = 0.0;
    for(int i=0; i<A.size; i++){
        for(int j=0; j<A.size; j++){
            diff += fabsf(A.data[i][j] - B.data[i][j]);
        }
    }
    return diff;
}

/* --------------------------------------------------------------------------------
   FONCTION PRINCIPALE (MAIN)
-------------------------------------------------------------------------------- */

int main() {
    char filename[100];

    printf("Nom du fichier graphe (ex: exemple1.txt) : ");
    if (scanf("%s", filename) != 1) return 1;

    // --- PARTIE 1 ---
    ListeAdjacence g = readGraph(filename);
    afficherListeAdjacence(g);
    verifierGrapheMarkov(g);
    genererFichierMermaid(g, "graphe_initial.mmd");

    // --- PARTIE 2 ---
    Partition part = executeTarjan(g);
    afficherPartition(part); 

    ListeLiens liens = creerLiensHasse(g, part);
    genererHasseMermaid(part, liens, "graphe_hasse.mmd"); 
    analyserProprietes(part, liens); 

    // --- PARTIE 3 ---
    t_matrix M = graphToMatrix(g);
    printf("\nMatrice de transition M :"); 
    printMatrix(M); 

    // Calcul de la distribution stationnaire (M^n)
    t_matrix M_curr = createMatrix(M.size);
    t_matrix M_next = createMatrix(M.size);
    
    copyMatrix(M_curr, M); 
    
    int n = 1;
    float epsilon = 0.01; 
    float diff = 1.0;
    int max_iter = 1000;

    printf("\nCalcul de convergence (M^n) avec epsilon=%.3f...\n", epsilon);
    
    while (diff > epsilon && n < max_iter) {
        t_matrix res = multiplyMatrices(M_curr, M); 
        copyMatrix(M_next, res);
        freeMatrix(res); 

        diff = diffMatrix(M_next, M_curr); 
        copyMatrix(M_curr, M_next); 
        n++;
    }

    if (n >= max_iter) {
        printf("Convergence non atteinte après %d itérations.\n", max_iter);
    } else {
        printf("Convergence atteinte à n=%d (diff=%.5f)\n", n, diff);
        printf("Distribution stationnaire approximée (M^n) :\n");
        printMatrix(M_curr); 
    }

    // --- NETTOYAGE ---
    freeMatrix(M);
    freeMatrix(M_curr);
    freeMatrix(M_next);
    for(int i=0; i<part.nb_classes; i++) free(part.classes[i].sommets);
    free(part.classes);
    free(liens.array);
    free(vertex_to_class_map);

    return 0;
}