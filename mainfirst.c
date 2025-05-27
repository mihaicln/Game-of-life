#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functiiStack.h"
#include "treeFunctions.h"
#include "bibTask4.h"

int main(const int argc, char const *argv[])
{

    int n, m, i, j;
    int task, gen;

    char **matrix;
    FILE *fisier_input;
    FILE *fisier_output;

    if ((fisier_input = fopen(argv[1], "rt")) == NULL)
    {
        printf("Fisierul de intrare nu poate fi deschis \n");
        exit(1);
    }

    if ((fisier_output = fopen(argv[2], "w")) == NULL)
    {
        printf("Fisierul de iesire nu poate fi deschis \n");
        exit(1);
    }

    fscanf(fisier_input, "%d %d %d %d", &task, &n, &m, &gen);

    matrix = (char **)malloc(n * sizeof(char *));
    for (i = 0; i < n; i++)
    {
        matrix[i] = (char *)malloc(m * sizeof(char));
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            fscanf(fisier_input, " %c", &matrix[i][j]);
        }
    }

    if (task == 1)
    {
        afisareMatrix(matrix, n, m, fisier_output);

        while (gen != 0)
        {
            populareMatrix(matrix, n, m);
            afisareMatrix(matrix, n, m, fisier_output);
            gen--;
        }
    }

    if (task == 2)
        implementTask2(matrix, n, m, fisier_output, gen);

    if (task == 3)
        implementTask3(matrix, n, m, fisier_output, gen);

    /*if (task == 4)
        implementTask4(matrix, n, m, fisier_output, gen);
    */

    freeMatrix(matrix, n, m);
    return 0;
}