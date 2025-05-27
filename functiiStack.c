#include "functiiStack.h"

void afisareMatrix(char **matrix, int n, int m, FILE *fisier_output)
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

void populareMatrix(char **matrix, int n, int m)
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
                    copie_matrix[i - 1][j - 1] = '+';
                if (nr_vecini == 2 || nr_vecini == 3)
                    copie_matrix[i - 1][j - 1] = 'X';
                if (nr_vecini > 3)
                    copie_matrix[i - 1][j - 1] = '+';
            }
            if (matrixBordata[i][j] == '+')
                if (nr_vecini == 3)
                    copie_matrix[i - 1][j - 1] = 'X';
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

void push(Node **top, int x, int y)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->valX = x;
    newNode->valY = y;
    newNode->next = *top;
    *top = newNode;
}

// Elimină și returnează elementul din vârful stivei
Node *pop(Node **top)
{
    Node *aux = (Node *)malloc(sizeof(Node));
    // returnează INT_MIN dacă stiva este goală
    // stochează adresa vârfului în temp
    Node *temp = (*top);

    // stochează valoarea din vârf în aux
    aux->valX = temp->valX;
    aux->valY = temp->valY;

    // șterge elementul din vârf
    *top = (*top)->next;
    free(temp);

    return aux;
}

void printStack(Node *top, FILE *fisier_out)
{
    while (top != NULL)
    {
        fprintf(fisier_out, " %d %d", top->valX, top->valY);
        top = top->next;
        // printf("\n")
    }
}

void deleteStack(Node **top)
{
    while ((*top) != NULL)
    { // echivalent cu !isEmpty(*top)
        Node *temp;
        temp = *top;
        *top = (*top)->next;
        free(temp);
    }
}

void Task2(char **matrix, int n, int m, Node **stacktop)
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
                    push(stacktop, i - 1, j - 1);
                    copie_matrix[i - 1][j - 1] = '+';
                }

                if (nr_vecini == 2 || nr_vecini == 3)
                {
                    // push(stacktop, i - 1, j - 1);
                    copie_matrix[i - 1][j - 1] = 'X';
                }
                if (nr_vecini > 3)
                {
                    push(stacktop, i - 1, j - 1);
                    copie_matrix[i - 1][j - 1] = '+';
                }
            }
            if (matrixBordata[i][j] == '+')
                if (nr_vecini == 3)
                {
                    push(stacktop, i - 1, j - 1);
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

void implementTask2(char **matrix, int n, int m, FILE *fisier_output, int gen)
{
    int cnt = 1;
    while (gen != 0)
    {
        Node *stacktop = NULL;
        Node *stacktop2 = NULL;
        Node *aux = NULL;
        Task2(matrix, n, m, &stacktop);

        while (stacktop != NULL)
        {
            aux = pop(&stacktop);
            push(&stacktop2, aux->valX, aux->valY);
            // printf("(%d %d) ", aux->valX, aux->valY);
            free(aux);
        }

        fprintf(fisier_output, "%d", cnt);
        printStack(stacktop2, fisier_output);
        fprintf(fisier_output, "\n");
        deleteStack(&stacktop2);
        cnt++;
        gen--;
    }
}

void freeMatrix(char **matrix, int n, int m)
{
    for (int i = 0; i < n; i++)
        free(matrix[i]);
    free(matrix);
}