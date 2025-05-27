#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct Elem
{
    int valX;
    int valY;
    struct Elem *next;
};

typedef struct Elem Node;

void populareMatrix(char **matrix, int n, int m);
void afisareMatrix(char **matrix, int n, int m, FILE *fisier_output);
int isEmpty(Node *top);
void push(Node **top, int x, int y);
Node* pop(Node **top);
void deleteStack(Node **top);
void printStack(Node* top, FILE *fisier_out);
void Task2(char **matrix, int n, int m, Node **stacktop);
void implementTask2(char **matrix, int n, int m, FILE *fisier_output, int gen);
void freeMatrix(char **matrix, int n, int m);



