#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int row, col;
} Position;

int dirR[] = {-1, -1, -1, 0, 0, 1, 1, 1};
int dirC[] = {-1, 0, 1, -1, 1, -1, 0, 1};

int n, m;

int countNodes(char **matrix)
{
    int cnt = 0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            if (matrix[i][j] == 'X')
                cnt++;
    return cnt;
}

// Verifică dacă două lanțuri sunt lexicografic ordonate conform regulilor
int isBetter(Position *a, Position *b, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (a[i].row < b[i].row)
            return 1;
        if (a[i].row > b[i].row)
            return 0;
        if (a[i].col < b[i].col)
            return 1;
        if (a[i].col > b[i].col)
            return 0;
    }
    return 0; // sunt identice
}

void backtrack(char **matrix, int **visited,
               int r, int c,
               Position *path,
               int length,
               int totalNodes,
               int *maxLength,
               Position *bestPath)
{

    visited[r][c] = 1;
    path[length] = (Position){r, c};
    length++;

    if (length == totalNodes)
    {
        if (length > *maxLength || (length == *maxLength && isBetter(path, bestPath, length)))
        {
            *maxLength = length;
            memcpy(bestPath, path, length * sizeof(Position));
        }
    }
    else
    {
        for (int d = 0; d < 8; d++)
        {
            int nr = r + dirR[d];
            int nc = c + dirC[d];
            if (nr >= 0 && nr < n && nc >= 0 && nc < m)
            {
                if (matrix[nr][nc] == 'X' && !visited[nr][nc])
                {
                    backtrack(matrix, visited, nr, nc, path, length, totalNodes, maxLength, bestPath);
                }
            }
        }
    }

    visited[r][c] = 0;
}

// Funcție principală pentru lanțuri Hamiltoniene
void findBestHamiltonianChain(char **matrix)
{
    int total = countNodes(matrix);
    if (total == 0)
    {
        printf("-1\n");
        return;
    }

    int **visited = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
        visited[i] = calloc(m, sizeof(int));

    Position *path = malloc(total * sizeof(Position));
    Position *bestPath = malloc(total * sizeof(Position));
    int maxLength = -1;

    // Încearcă să pornești din fiecare nod 'X'
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if (matrix[i][j] == 'X')
            {
                backtrack(matrix, visited, i, j, path, 0, total, &maxLength, bestPath);
            }
        }
    }

    if (maxLength == total)
    {
        printf("%d\n", maxLength - 1);
        for (int i = 0; i < maxLength; i++)
        {
            printf("(%d,%d) ", bestPath[i].row, bestPath[i].col);
        }
        printf("\n");
    }
    else
    {
        printf("-1\n");
    }

    for (int i = 0; i < n; i++)
        free(visited[i]);
    free(visited);
    free(path);
    free(bestPath);
}

int main()
{
    n = 25;
    m = 25;
    char *rows[] = {
        "+++++++",
        "+++++++",
        "++X+X++",
        "+++XX++",
        "+++X+++",
        "+++++++"};

    char **matrix = malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++)
    {
        matrix[i] = malloc(m * sizeof(char));
        for (int j = 0; j < m; j++)
        {
            matrix[i][j] = rows[i][j];
        }
    }

    findBestHamiltonianChain(matrix);

    for (int i = 0; i < n; i++)
        free(matrix[i]);
    free(matrix);

    return 0;
}

4
25 25
4
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
++++++++++XXX++++++++++++
+++++++++XXX+++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++
+++++++++++++++++++++++++