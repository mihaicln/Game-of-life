#ifndef TASK4_H_
#define TASK4_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Structuri (rămân aici, așa cum sunt)
typedef struct {
    int r;
    int c;
    int id_org;
} VarfT4;

typedef struct POINT {
    int valX, valY;
    struct POINT *next;
} POINT;

typedef struct TreeNODE {
    POINT *head;
    struct TreeNODE *left, *right;
} TreeNODE;

// NOU: Structură pentru informații despre vecini sortați
typedef struct {
    VarfT4 v_info;
    int idx_local;
} SortedNeighborInfoT4;

// NOU: Funcție de comparare pentru qsort, pentru SortedNeighborInfoT4
int comparaVeciniSortabiliT4(const void *a, const void *b);

// NOU: Structură pentru a ține listele de adiacență pre-sortate ale unei componente
typedef struct {
    int num_neighbors;
    SortedNeighborInfoT4* sorted_neighbors_info;
} ComponentAdjListT4;

// MODIFICAT: DateDFS acum folosește listele de adiacență pre-sortate
typedef struct {
    VarfT4 *noduri;
    ComponentAdjListT4 *pre_sorted_adj_lists;
    int nr_noduri;
    int *cale_crt;
    int *cale_opt;
    int *vizitat;
    int lung_opt;
} DateDFS;


// --- PROTOTIPURILE FUNCȚIILOR ---

// Funcții ajutătoare pentru DFS și drum Hamiltonian
int comparaCaiLexicograficT4(const VarfT4 *cale1, const VarfT4 *cale2, int lung);
void dfsPentruComponentaT4(DateDFS *date, int nod_crt_idx_local, int adanc);

// MODIFICAT: Adăugat argumente pentru global_cale_crt, global_cale_opt, global_vizitat
int gasesteCeaMaiBunaCaleInComponentaT4(VarfT4 *noduri_comp, int nr_noduri_comp,
                                         ComponentAdjListT4 *pre_sorted_adj_lists, VarfT4 *out_cale,
                                         int *global_cale_crt, int *global_cale_opt, int *global_vizitat);
void normalizeazaCaleFinalaT4(VarfT4 *path, int lung);

// Funcții pentru Game of Life și arbore
void AddAtBeginning(POINT **head, int x, int y);
void AddAtEnd(POINT **head, int x, int y);

// MODIFICAT: Adăugat argumente pentru copiile matricilor temporare
void MatrixRule2(char **matrix, int n, int m, POINT **head, char **copie_matrix, char **matrixBordata);
void MatrixRule1(char **matrix, int n, int m, POINT **head, char **copie_matrix, char **matrixBordata);
void GetCoordinates(char **matrix, int n, int m, POINT **head);

// ATENȚIE: Prototipul pentru DeleteTree și DeleteList înainte de BuildTree
void DeleteList(POINT **head);
void DeleteTree(TreeNODE **root);

// MODIFICAT: Adăugat argumente pentru matricile temporare și array-urile globale de DFS
void BuildTree(TreeNODE *node, char **matrix, int n, int m, int k, int level, FILE *fisier_output,
               char **global_copie_matrix, char **global_matrixBordata,
               VarfT4 *global_celule_vii, int **global_map_id,
               int *global_cale_crt, int *global_cale_opt, int *global_vizitat);

// MODIFICAT: Adăugat argumente pentru VarfT4 *celule_vii_global, int **map_id_global,
// int *global_cale_crt, int *global_cale_opt, int *global_vizitat
void hamiltonian(char **mat, int linii, int coloane, FILE *fisier_output,
                 VarfT4 *celule_vii_global, int **map_id_global,
                 int *global_cale_crt, int *global_cale_opt, int *global_vizitat);

// Funcția de implementare a Task 4 (care include Game of Life și Hamiltonian)
void implementTask4(char **matrix, int n, int m, FILE *fisier_output, int gen_K);

#endif // TASK4_H_