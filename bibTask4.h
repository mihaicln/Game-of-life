#ifndef BIBTASK4_H
#define BIBTASK4_H

#include <stdio.h> // Pentru FILE*
#include <stdlib.h> // Pentru malloc, free etc.

// Structura pentru a reprezenta o poziție (coordonate rând, coloană)
typedef struct {
    int row;
    int col;
} Position;

// Structura pentru un nod într-o listă simplu înlănțuită de puncte
typedef struct POINT {
    int valX;
    int valY;
    struct POINT *next;
} POINT;

// Structura pentru un nod într-o listă de adiacență sortată (pentru DFS Hamiltonian)
typedef struct AdjListNode {
    int local_idx; // Indexul local al vecinului în componenta
    struct AdjListNode *next;
} AdjListNode;

// Structura pentru lista de adiacență a unui nod în componentă
typedef struct {
    AdjListNode *head;
    int degree; // Gradul nodului
} AdjList;

// Structura pentru o componentă conexă
typedef struct {
    Position *nodes; // Array de poziții ale nodurilor din componentă
    int count;       // Numărul de noduri din componentă
    AdjList *adj_lists; // Listele de adiacență pentru nodurile acestei componente
} ConnectedComponent;

// Structura pentru un nod din arborele de generare a matricelor
typedef struct TreeNODE {
    POINT *head; // Capul listei de puncte (celule modificate/adaugate)
    struct TreeNODE *left;
    struct TreeNODE *right;
} TreeNODE;

// --- Prototipuri Funcții Globale ---

// Funcții pentru manipularea listelor de puncte (folosite în BuildTree)
void AddAtBeginning(POINT **head, int x, int y);
void AddAtEnd(POINT **head, int x, int y);
void DeleteList(POINT **head);
void GetCoordinates(char **matrix, int n, int m, POINT **head);

// Funcții pentru aplicarea regulilor Game of Life
void MatrixRule1(char **matrix, int n, int m, POINT **head);
void MatrixRule2(char **matrix, int n, int m, POINT **head);

// Funcții pentru gestionarea arborelui de generare
void BuildTree(TreeNODE *node, char **matrix, int n, int m, int k, int level, FILE *fisier_output);
void DeleteTree(TreeNODE **root);

// Funcții pentru găsirea componentelor conexe
void dfs_component(char **matrix, int n, int m, int r, int c, int **visited_global, Position *component_nodes, int *component_count);
ConnectedComponent *findConnectedComponents(char **matrix, int n, int m, int *num_components);

// Funcții pentru găsirea drumului Hamiltonian
int isBetter(Position *a, Position *b, int len);
void findBestHamiltonianChain(char **matrix, int n, int m, FILE *fisier_output);

// Funcția principală pentru Task 4
void implementTask4(char **matrix, int n, int m, FILE *fisier_output, int gen);

#endif // BIBTASK4_H