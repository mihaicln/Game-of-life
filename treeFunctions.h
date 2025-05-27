#ifndef TREE_FUNCTIONS_H
#define TREE_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>

struct nigg
{
    int valX;
    int valY;
    struct nigg *next;
};

typedef struct nigg Point;

struct nod
{
    Point *head;
    struct nod *left, *right;
};

typedef struct nod TreeNode;

void AfisareMatrix(char **matrix, int n, int m, FILE *fisier_output);
void addAtBeginning(Point **head, int x, int y);
void addAtEnd(Point **head, int x, int y);
void matrixRule2(char **matrix, int n, int m, Point **head);
void matrixRule1(char **matrix, int n, int m, Point **head);
void getCoordinates(char **matrix, int n, int m, Point **head);
void buildTree(TreeNode *node, char **matrix, int n, int m, int k, int level, FILE *fisier_output);
void deleteList(Point **head);
void deleteTree(TreeNode **root);
void implementTask3(char **matrix, int n, int m,FILE *fisier_output, int gen);




#endif