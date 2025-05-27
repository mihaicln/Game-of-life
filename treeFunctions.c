#include "treeFunctions.h"

void AfisareMatrix(char **matrix, int n, int m, FILE *fisier_output)
{
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            fprintf(fisier_output, "%c", matrix[i][j]);
        }
        fprintf(fisier_output, "\n");
    }
    fprintf(fisier_output, "\n");
}

void addAtBeginning(Point **head, int x, int y)
{
    Point *newNode = (Point *)malloc(sizeof(Point));
    newNode->valX = x;
    newNode->valY = y;
    newNode->next = *head;
    *head = newNode;
}

void addAtEnd(Point **head, int x, int y)
{
    // daca lista este vida, adaug la inceput
    if (*head == NULL)
        addAtBeginning(head, x, y);
    else
    {
        Point *newNode = (Point *)malloc(sizeof(Point));
        newNode->valX = x; // se stocheaza informatia nodului nou
        newNode->valY = y;
        Point *aux = *head;
        // cat timp nu s-a ajuns la final, se parcurge
        while (aux->next != NULL)
            aux = aux->next;
        // se adauga noul element in lista
        aux->next = newNode;
        newNode->next = NULL; 
    }
}

void matrixRule2(char **matrix, int n, int m, Point **head)
{
    int i, j;
    char **copie_matrix;
    copie_matrix = (char **)malloc(n * sizeof(char *));
    for (i = 0; i < n; i++)
    {
        copie_matrix[i] = (char *)malloc(m * sizeof(char));
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
            copie_matrix[i][j] = matrix[i][j];
    }

    char **matrixBordata = (char **)malloc((n + 2) * sizeof(char *));
    for (i = 0; i < n + 2; i++)
    {
        matrixBordata[i] = (char *)malloc((m + 2) * sizeof(char));
    }

    for (i = 0; i < n + 2; i++)
    {
        for (j = 0; j < m + 2; j++)
            matrixBordata[i][j] = '-';
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            matrixBordata[i + 1][j + 1] = matrix[i][j];
        }
    }

    // verific vecinii pt elementele din interiorul matricei bordate
    for (i = 1; i < n + 1; i++)
    {
        for (j = 1; j < m + 1; j++)
        {
            int nr_vecini = 0;
            if (matrixBordata[i][j + 1] == 'X')
                nr_vecini++;
            if (matrixBordata[i][j - 1] == 'X')
                nr_vecini++;
            if (matrixBordata[i - 1][j] == 'X')
                nr_vecini++;
            if (matrixBordata[i + 1][j] == 'X')
                nr_vecini++;
            if (matrixBordata[i - 1][j - 1] == 'X')
                nr_vecini++;
            if (matrixBordata[i - 1][j + 1] == 'X')
                nr_vecini++;
            if (matrixBordata[i + 1][j - 1] == 'X')
                nr_vecini++;
            if (matrixBordata[i + 1][j + 1] == 'X')
                nr_vecini++;

            if (nr_vecini == 2)
            {
                addAtEnd(head, i - 1, j - 1);
                copie_matrix[i - 1][j - 1] = 'X';
            }
        }
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
            matrix[i][j] = copie_matrix[i][j];
    }
    for (i = 0; i < n; i++)
        free(copie_matrix[i]);
    free(copie_matrix);

    for (i = 0; i < n + 2; i++)
        free(matrixBordata[i]);
    free(matrixBordata);
}

void matrixRule1(char **matrix, int n, int m, Point **head)
{
    int i, j;
    char **copie_matrix;
    copie_matrix = (char **)malloc(n * sizeof(char *));
    for (i = 0; i < n; i++)
    {
        copie_matrix[i] = (char *)malloc(m * sizeof(char));
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
            copie_matrix[i][j] = matrix[i][j];
    }

    char **matrixBordata = (char **)malloc((n + 2) * sizeof(char *));
    for (i = 0; i < n + 2; i++)
    {
        matrixBordata[i] = (char *)malloc((m + 2) * sizeof(char));
    }

    for (i = 0; i < n + 2; i++)
    {
        for (j = 0; j < m + 2; j++)
            matrixBordata[i][j] = '-';
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            matrixBordata[i + 1][j + 1] = matrix[i][j];
        }
    }

    // verific vecinii pt elementele din interiorul matricei bordate
    for (i = 1; i < n + 1; i++)
    {
        for (j = 1; j < m + 1; j++)
        {
            int nr_vecini = 0;
            if (matrixBordata[i][j + 1] == 'X')
                nr_vecini++;
            if (matrixBordata[i][j - 1] == 'X')
                nr_vecini++;
            if (matrixBordata[i - 1][j] == 'X')
                nr_vecini++;
            if (matrixBordata[i + 1][j] == 'X')
                nr_vecini++;
            if (matrixBordata[i - 1][j - 1] == 'X')
                nr_vecini++;
            if (matrixBordata[i - 1][j + 1] == 'X')
                nr_vecini++;
            if (matrixBordata[i + 1][j - 1] == 'X')
                nr_vecini++;
            if (matrixBordata[i + 1][j + 1] == 'X')
                nr_vecini++;

            if (matrixBordata[i][j] == 'X')
            {
                if (nr_vecini < 2)
                {
                    addAtEnd(head, i - 1, j - 1);
                    copie_matrix[i - 1][j - 1] = '+';
                }

                if (nr_vecini == 2 || nr_vecini == 3)
                {
                    copie_matrix[i - 1][j - 1] = 'X';
                }
                if (nr_vecini > 3)
                {
                    addAtEnd(head, i - 1, j - 1);
                    copie_matrix[i - 1][j - 1] = '+';
                }
            }
            if (matrixBordata[i][j] == '+')
                if (nr_vecini == 3)
                {
                    addAtEnd(head, i - 1, j - 1);
                    copie_matrix[i - 1][j - 1] = 'X';
                }
        }
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
            matrix[i][j] = copie_matrix[i][j];
    }
    for (i = 0; i < n; i++)
        free(copie_matrix[i]);
    free(copie_matrix);

    for (i = 0; i < n + 2; i++)
        free(matrixBordata[i]);
    free(matrixBordata);
}

void getCoordinates(char **matrix, int n, int m, Point **head)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if (matrix[i][j] == 'X')
                addAtEnd(head, i, j);
        }
    }
}

void buildTree(TreeNode *node, char **matrix, int n, int m, int k, int level, FILE *fisier_output)
{
    if (level == k)
        return;

    // matrice clonata pentru fiecare ramură
    char **matrixLeft = (char **)malloc(n * sizeof(char *));
    char **matrixRight = (char **)malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++)
    {
        matrixLeft[i] = (char *)malloc(m * sizeof(char));
        matrixRight[i] = (char *)malloc(m * sizeof(char));
        for (int j = 0; j < m; j++)
        {
            matrixLeft[i][j] = matrix[i][j];
            matrixRight[i][j] = matrix[i][j];
        }
    }

    // regula pentru copilul din stanga
    node->left = (TreeNode *)malloc(sizeof(TreeNode));
    node->left->head = NULL;
    node->left->left = NULL;
    node->left->right = NULL;
    matrixRule2(matrixLeft, n, m, &(node->left->head)); // actualizează matrixLeft + salvează doar celule noi
    AfisareMatrix(matrixLeft, n, m, fisier_output);

    buildTree(node->left, matrixLeft, n, m, k, level + 1, fisier_output);

    //regula pentru copilul din dreapta
    node->right = (TreeNode *)malloc(sizeof(TreeNode));
    node->right->head = NULL;
    node->right->left = NULL;
    node->right->right = NULL;
    matrixRule1(matrixRight, n, m, &(node->right->head)); // actualizează matrixRight + salvează doar celule modificate
    AfisareMatrix(matrixRight, n, m, fisier_output);

    buildTree(node->right, matrixRight, n, m, k, level + 1, fisier_output);

    // Eliberăm matricile
    for (int i = 0; i < n; i++)
    {
        free(matrixLeft[i]);
        free(matrixRight[i]);
    }
    free(matrixLeft);
    free(matrixRight);
}

void deleteList(Point **head)
{
    Point *current = *head;
    
    while (current != NULL)
    {
        Point *temp = current;
        current = current->next;
        free(temp);
    }
    *head = NULL;
}

void deleteTree(TreeNode **root)
{
    if (*root == NULL)
        return;

    deleteTree(&(*root)->left);
    deleteTree(&(*root)->right);
    deleteList(&(*root)->head);
    free(*root);
    *root = NULL;
}

void implementTask3(char **matrix, int n, int m,FILE *fisier_output, int gen)
{
    Point *headRoot = NULL;
    getCoordinates(matrix, n, m, &headRoot);
    TreeNode *root = NULL;
    root = (TreeNode *)malloc(sizeof(TreeNode));
    root->head = headRoot;
    root->left = NULL;
    root->right = NULL;
    AfisareMatrix(matrix, n, m, fisier_output);
    buildTree(root, matrix, n, m, gen, 0, fisier_output);
    deleteTree(&root);
}