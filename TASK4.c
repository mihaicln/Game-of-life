#include "TASK4.h" // Includeți propriul header
#include <stdio.h>   // Pentru fprintf, stderr
#include <stdlib.h>  // Pentru malloc, free, qsort, calloc
#include <string.h>  // Pentru memcpy, memset
#include <stdbool.h> // Pentru tipul bool

// Funcție de comparare pentru qsort, pentru SortedNeighborInfoT4
int comparaVeciniSortabiliT4(const void *a, const void *b) {
    SortedNeighborInfoT4 *v1 = (SortedNeighborInfoT4 *)a;
    SortedNeighborInfoT4 *v2 = (SortedNeighborInfoT4 *)b;
    if (v1->v_info.r < v2->v_info.r) return -1;
    if (v1->v_info.r > v2->v_info.r) return 1;
    if (v1->v_info.c < v2->v_info.c) return -1;
    if (v1->v_info.c > v2->v_info.c) return 1;
    return 0;
}

// MODIFICAT: Funcție pentru comparație lexicografică, folosind VarfT4 direct
int comparaCaiLexicograficT4(const VarfT4 *cale1, const VarfT4 *cale2, int lung) {
    for (int i = 0; i < lung; ++i) {
        if (cale1[i].r < cale2[i].r) return 1; // Returnează 1 dacă calea1 e mai mică lexicografic
        if (cale1[i].r > cale2[i].r) return -1;
        if (cale1[i].c < cale2[i].c) return 1;
        if (cale1[i].c > cale2[i].c) return -1;
    }
    return 0;
}

// MODIFICAT: Algoritm DFS pentru a folosi listele de adiacență pre-sortate
void dfsPentruComponentaT4(DateDFS *date, int nod_crt_idx_local, int adanc) {
    date->cale_crt[adanc - 1] = nod_crt_idx_local;
    date->vizitat[nod_crt_idx_local] = 1;

    if (adanc == date->nr_noduri) {
        if (date->lung_opt == 0) {
            date->lung_opt = date->nr_noduri;
            memcpy(date->cale_opt, date->cale_crt, date->nr_noduri * sizeof(int));
        } else {
            // Folosim comparaCaiLexicograficT4 direct pe VarfT4
            // Atenție: Folosirea VLA (Variable Length Array) pe stack poate duce la stack overflow pentru dimensiuni mari.
            // Ar fi mai sigur să se aloce pe heap, dar pentru a păstra compatibilitatea cu antetul, o lăsăm așa.
            VarfT4 *temp_cale_crt_nodes = (VarfT4 *)malloc(date->nr_noduri * sizeof(VarfT4));
            VarfT4 *temp_cale_opt_nodes = (VarfT4 *)malloc(date->nr_noduri * sizeof(VarfT4));

            if (!temp_cale_crt_nodes || !temp_cale_opt_nodes) {
                fprintf(stderr, "Eroare alocare temporara in dfsPentruComponentaT4\n");
                if (temp_cale_crt_nodes) free(temp_cale_crt_nodes);
                if (temp_cale_opt_nodes) free(temp_cale_opt_nodes);
                // Nu putem continua, dar trebuie să ne asigurăm că `vizitat` este resetat la ieșire.
                date->vizitat[nod_crt_idx_local] = 0;
                return;
            }

            for(int i = 0; i < date->nr_noduri; ++i) {
                temp_cale_crt_nodes[i] = date->noduri[date->cale_crt[i]];
                temp_cale_opt_nodes[i] = date->noduri[date->cale_opt[i]];
            }

            if (comparaCaiLexicograficT4(temp_cale_crt_nodes, temp_cale_opt_nodes, date->nr_noduri) > 0) {
                memcpy(date->cale_opt, date->cale_crt, date->nr_noduri * sizeof(int));
            }
            free(temp_cale_crt_nodes);
            free(temp_cale_opt_nodes);
        }
    } else {
        ComponentAdjListT4 current_node_adj = date->pre_sorted_adj_lists[nod_crt_idx_local];
        for (int i = 0; i < current_node_adj.num_neighbors; ++i) {
            int vecin_idx_local_next = current_node_adj.sorted_neighbors_info[i].idx_local;
            if (!date->vizitat[vecin_idx_local_next]) {
                dfsPentruComponentaT4(date, vecin_idx_local_next, adanc + 1);
            }
        }
    }
    date->vizitat[nod_crt_idx_local] = 0;
}

// MODIFICAT: Determină cel mai bun lanț (Hamiltonian dacă există) dintr-o componentă
int gasesteCeaMaiBunaCaleInComponentaT4(VarfT4 *noduri_comp, int nr_noduri_comp,
                                         ComponentAdjListT4 *pre_sorted_adj_lists, VarfT4 *out_cale,
                                         int *global_cale_crt, int *global_cale_opt, int *global_vizitat) {
    if (nr_noduri_comp == 0) return 0;
    if (nr_noduri_comp == 1) {
        out_cale[0] = noduri_comp[0];
        return 1;
    }

    // Heuristica simpla: daca exista nod izolat sau prea multe noduri cu grad 1
    if (nr_noduri_comp > 1) {
        int nodes_with_degree_one = 0;
        bool has_isolated_node_in_multi_node_component = false;
        for (int i = 0; i < nr_noduri_comp; ++i) {
            if (pre_sorted_adj_lists[i].num_neighbors == 0) {
                has_isolated_node_in_multi_node_component = true;
                break;
            }
            if (pre_sorted_adj_lists[i].num_neighbors == 1) {
                nodes_with_degree_one++;
            }
        }
        if (has_isolated_node_in_multi_node_component || (nodes_with_degree_one > 2)) {
            return 0;
        }
    }

    DateDFS date;
    date.noduri = noduri_comp;
    date.pre_sorted_adj_lists = pre_sorted_adj_lists;
    date.nr_noduri = nr_noduri_comp;
    date.cale_crt = global_cale_crt; // Folosim memoria pre-alocată
    date.cale_opt = global_cale_opt; // Folosim memoria pre-alocată
    date.vizitat = global_vizitat;   // Folosim memoria pre-alocată
    date.lung_opt = 0;

    for (int start_idx_in_component = 0; start_idx_in_component < nr_noduri_comp; ++start_idx_in_component) {
        memset(date.vizitat, 0, nr_noduri_comp * sizeof(int)); // Resetăm vizitat pentru fiecare start
        dfsPentruComponentaT4(&date, start_idx_in_component, 1);
    }

    int lung_rez = 0;
    if (date.lung_opt == nr_noduri_comp) {
        for (int i = 0; i < nr_noduri_comp; ++i)
            out_cale[i] = date.noduri[date.cale_opt[i]];
        lung_rez = nr_noduri_comp;
    } else {
        lung_rez = 0;
    }

    return lung_rez;
}

// Normalizarea căii
void normalizeazaCaleFinalaT4(VarfT4 *path, int lung) {
    if (lung <= 1) return;
    VarfT4 *path_inv = (VarfT4 *)malloc(lung * sizeof(VarfT4));
    if (!path_inv) {
        fprintf(stderr, "Eroare malloc in normalizeazaCaleFinalaT4\n");
        return;
    }
    for (int i = 0; i < lung; i++) path_inv[i] = path[lung - 1 - i];

    if (comparaCaiLexicograficT4(path_inv, path, lung) > 0)
        memcpy(path, path_inv, lung * sizeof(VarfT4));

    free(path_inv);
}

// Funcția principală de procesare a unei matrici pentru Task 4
void hamiltonian(char **mat, int linii, int coloane, FILE *fisier_output,
                 VarfT4 *celule_vii_global, int **map_id_global,
                 int *global_cale_crt, int *global_cale_opt, int *global_vizitat) {

    int nr_celule_vii = 0;
    for (int i = 0; i < linii; ++i)
        for (int j = 0; j < coloane; ++j) {
            map_id_global[i][j] = -1; // Resetam map_id pentru fiecare apel
            if (mat[i][j] == 'X') {
                celule_vii_global[nr_celule_vii].r = i;
                celule_vii_global[nr_celule_vii].c = j;
                celule_vii_global[nr_celule_vii].id_org = nr_celule_vii;
                map_id_global[i][j] = nr_celule_vii;
                nr_celule_vii++;
            }
        }

    if (nr_celule_vii == 0) {
        fprintf(fisier_output, "-1\n\n");
        return;
    }

    int **adj_list_global = (int **)malloc(nr_celule_vii * sizeof(int *));
    if (!adj_list_global) {
        fprintf(stderr, "Eroare alocare adj_list_global (rânduri) in hamiltonian\n");
        fprintf(fisier_output, "-1\n\n");
        return;
    }
    for (int i = 0; i < nr_celule_vii; ++i) {
        adj_list_global[i] = (int *)calloc(nr_celule_vii, sizeof(int));
        if (!adj_list_global[i]) {
            fprintf(stderr, "Eroare alocare adj_list_global (coloane) la index %d in hamiltonian\n", i);
            for (int k = 0; k < i; ++k) free(adj_list_global[k]);
            free(adj_list_global);
            fprintf(fisier_output, "-1\n\n");
            return;
        }
    }

    const int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    for (int k = 0; k < nr_celule_vii; ++k) {
        int r1 = celule_vii_global[k].r;
        int c1 = celule_vii_global[k].c;
        for (int move = 0; move < 8; ++move) {
            int r2 = r1 + dr[move];
            int c2 = c1 + dc[move];
            if (r2 >= 0 && r2 < linii && c2 >= 0 && c2 < coloane && mat[r2][c2] == 'X') {
                adj_list_global[k][map_id_global[r2][c2]] = 1;
            }
        }
    }

    int lung_max_cale_global = 0;
    VarfT4 *cale_max_global = NULL;

    // Coada si id_nod_comp_global_indices sunt alocate aici, nu in bucla
    int *coada = (int *)malloc(nr_celule_vii * sizeof(int));
    int *id_nod_comp_global_indices = (int *)malloc(nr_celule_vii * sizeof(int));

    if (!coada || !id_nod_comp_global_indices) {
        fprintf(stderr, "Eroare alocare coada sau id_nod_comp_global_indices in hamiltonian\n");
        if (coada) free(coada);
        if (id_nod_comp_global_indices) free(id_nod_comp_global_indices);
        for (int i = 0; i < nr_celule_vii; ++i) free(adj_list_global[i]);
        free(adj_list_global);
        fprintf(fisier_output, "-1\n\n");
        return;
    }

    memset(global_vizitat, 0, (linii * coloane) * sizeof(int)); // Initializare cu 0

    for (int start_node_global_idx = 0; start_node_global_idx < nr_celule_vii; ++start_node_global_idx) {
        if (!global_vizitat[start_node_global_idx]) {
            int head = 0, tail = 0;
            coada[tail++] = start_node_global_idx;
            global_vizitat[start_node_global_idx] = 1;

            int nr_noduri_comp = 0;

            while (head < tail) {
                int u_global_idx = coada[head++];
                id_nod_comp_global_indices[nr_noduri_comp++] = u_global_idx;
                for (int v_global_idx = 0; v_global_idx < nr_celule_vii; ++v_global_idx) {
                    if (adj_list_global[u_global_idx][v_global_idx] && !global_vizitat[v_global_idx]) {
                        global_vizitat[v_global_idx] = 1;
                        coada[tail++] = v_global_idx;
                    }
                }
            }

            if (nr_noduri_comp > 0) {
                VarfT4 *noduri_comp = (VarfT4 *)malloc(nr_noduri_comp * sizeof(VarfT4));
                ComponentAdjListT4 *component_adj_lists = (ComponentAdjListT4 *)malloc(nr_noduri_comp * sizeof(ComponentAdjListT4));

                if (!noduri_comp || !component_adj_lists) {
                    fprintf(stderr, "Eroare alocare noduri_comp sau component_adj_lists in hamiltonian\n");
                    if (noduri_comp) free(noduri_comp);
                    if (component_adj_lists) free(component_adj_lists);
                    continue;
                }

                // Maparea nodurilor globale la noduri locale in componenta
                for (int k_comp = 0; k_comp < nr_noduri_comp; ++k_comp) {
                    noduri_comp[k_comp] = celule_vii_global[id_nod_comp_global_indices[k_comp]];
                }

                for (int u_local_idx = 0; u_local_idx < nr_noduri_comp; ++u_local_idx) {
                    SortedNeighborInfoT4 temp_neighbors[8]; // Max 8 vecini
                    int current_num_neighbors = 0;
                    int u_global_from_local = id_nod_comp_global_indices[u_local_idx];

                    for (int v_local_idx = 0; v_local_idx < nr_noduri_comp; ++v_local_idx) {
                        if (u_local_idx == v_local_idx) continue;
                        int v_global_from_local = id_nod_comp_global_indices[v_local_idx];
                        if (adj_list_global[u_global_from_local][v_global_from_local]) {
                            temp_neighbors[current_num_neighbors].v_info = noduri_comp[v_local_idx];
                            temp_neighbors[current_num_neighbors].idx_local = v_local_idx;
                            current_num_neighbors++;
                        }
                    }

                    qsort(temp_neighbors, current_num_neighbors, sizeof(SortedNeighborInfoT4), comparaVeciniSortabiliT4);

                    component_adj_lists[u_local_idx].num_neighbors = current_num_neighbors;
                    if (current_num_neighbors > 0) {
                        component_adj_lists[u_local_idx].sorted_neighbors_info = (SortedNeighborInfoT4 *)malloc(current_num_neighbors * sizeof(SortedNeighborInfoT4));
                        if (!component_adj_lists[u_local_idx].sorted_neighbors_info) {
                            fprintf(stderr, "Eroare alocare sorted_neighbors_info pentru nodul %d\n", u_local_idx);
                            component_adj_lists[u_local_idx].num_neighbors = 0;
                            // Continua, dar cu posibila pierdere de date pentru acest nod
                            continue;
                        }
                        memcpy(component_adj_lists[u_local_idx].sorted_neighbors_info, temp_neighbors, current_num_neighbors * sizeof(SortedNeighborInfoT4));
                    } else {
                        component_adj_lists[u_local_idx].sorted_neighbors_info = NULL;
                    }
                }

                VarfT4 *cale_comp = (VarfT4 *)malloc(nr_noduri_comp * sizeof(VarfT4));
                if (!cale_comp) {
                    fprintf(stderr, "Eroare alocare cale_comp in hamiltonian\n");
                    for (int i = 0; i < nr_noduri_comp; ++i) {
                        if (component_adj_lists[i].sorted_neighbors_info) free(component_adj_lists[i].sorted_neighbors_info);
                    }
                    free(component_adj_lists);
                    free(noduri_comp);
                    continue;
                }

                int lung_cale_curenta = gasesteCeaMaiBunaCaleInComponentaT4(noduri_comp, nr_noduri_comp, component_adj_lists, cale_comp, global_cale_crt, global_cale_opt, global_vizitat);

                if (lung_cale_curenta > 0) {
                    normalizeazaCaleFinalaT4(cale_comp, lung_cale_curenta);
                    if (lung_cale_curenta > lung_max_cale_global) {
                        lung_max_cale_global = lung_cale_curenta;
                        if (cale_max_global) free(cale_max_global);
                        cale_max_global = (VarfT4 *)malloc(lung_max_cale_global * sizeof(VarfT4));
                        if (!cale_max_global) {
                            fprintf(stderr, "Eroare re-alocare cale_max_global in hamiltonian\n");
                            lung_max_cale_global = 0;
                        } else {
                            memcpy(cale_max_global, cale_comp, lung_max_cale_global * sizeof(VarfT4));
                        }
                    } else if (lung_cale_curenta == lung_max_cale_global) {
                        if (comparaCaiLexicograficT4(cale_comp, cale_max_global, lung_max_cale_global) > 0) {
                            memcpy(cale_max_global, cale_comp, lung_max_cale_global * sizeof(VarfT4));
                        }
                    }
                }

                for (int i = 0; i < nr_noduri_comp; ++i) {
                    if (component_adj_lists[i].sorted_neighbors_info) {
                        free(component_adj_lists[i].sorted_neighbors_info);
                    }
                }
                free(component_adj_lists);
                free(noduri_comp);
                free(cale_comp);
            }
        }
    }

    if (lung_max_cale_global > 0) {
        fprintf(fisier_output, "%d\n", lung_max_cale_global - 1);
        for (int k = 0; k < lung_max_cale_global; ++k) {
            fprintf(fisier_output, "(%d,%d)%s", cale_max_global[k].r + 1, cale_max_global[k].c + 1, (k == lung_max_cale_global - 1) ? "" : " ");
        }
        fprintf(fisier_output, "\n\n");
    } else {
        fprintf(fisier_output, "-1\n\n");
    }

    if (cale_max_global) free(cale_max_global);
    free(coada);
    free(id_nod_comp_global_indices);

    // Dealocare adj_list_global
    for (int i = 0; i < nr_celule_vii; ++i) free(adj_list_global[i]);
    free(adj_list_global);
}

// Functii ajutatoare pentru Game of Life si arbore
// MODIFICAT: AddAtEnd - implementare clasică
void AddAtEnd(POINT **head, int x, int y) {
    POINT *newNode = (POINT *)malloc(sizeof(POINT));
    if (!newNode) {
        fprintf(stderr, "malloc error AddAtEnd\n");
        return;
    }
    newNode->valX = x;
    newNode->valY = y;
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
    } else {
        POINT *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

// Nu este folosită în cod, dar păstrată conform headerului
void AddAtBeginning(POINT **head, int x, int y) {
    POINT *newNode = (POINT *)malloc(sizeof(POINT));
    if (!newNode) {
        fprintf(stderr, "malloc error AddAtBeginning\n");
        return;
    }
    newNode->valX = x;
    newNode->valY = y;
    newNode->next = *head;
    *head = newNode;
}


// MODIFICAT: DeleteList
void DeleteList(POINT **head) {
    POINT *current = *head;
    POINT *temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    *head = NULL;
}

// MODIFICAT: MatrixRule2 - foloseste PointList și matrici globale
void MatrixRule2(char **matrix, int n, int m, POINT **head, char **copie_matrix, char **matrixBordata) {
    int i, j;

    DeleteList(head); // Resetăm head_list pentru o nouă utilizare

    // Copiem matricea originală în copie_matrix
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            copie_matrix[i][j] = matrix[i][j];
        }
    }

    // Umplem matrixBordata cu '+'
    for (i = 0; i < n + 2; i++) {
        for (j = 0; j < m + 2; j++) {
            matrixBordata[i][j] = '+';
        }
    }

    // Copiem conținutul matricei originale în centrul matrixBordata
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            matrixBordata[i + 1][j + 1] = matrix[i][j];
        }
    }

    for (i = 1; i < n + 1; i++) {
        for (j = 1; j < m + 1; j++) {
            int nr_vecini = 0;
            if (matrixBordata[i][j + 1] == 'X') nr_vecini++;
            if (matrixBordata[i][j - 1] == 'X') nr_vecini++;
            if (matrixBordata[i - 1][j] == 'X') nr_vecini++;
            if (matrixBordata[i + 1][j] == 'X') nr_vecini++;
            if (matrixBordata[i - 1][j - 1] == 'X') nr_vecini++;
            if (matrixBordata[i - 1][j + 1] == 'X') nr_vecini++;
            if (matrixBordata[i + 1][j - 1] == 'X') nr_vecini++;
            if (matrixBordata[i + 1][j + 1] == 'X') nr_vecini++;

            if (matrixBordata[i][j] == '+') {
                if (nr_vecini == 2) {
                    if (copie_matrix[i - 1][j - 1] != 'X') {
                        AddAtEnd(head, i - 1, j - 1);
                    }
                    copie_matrix[i - 1][j - 1] = 'X';
                }
            }
        }
    }

    // Copiem rezultatul în matricea originală
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            matrix[i][j] = copie_matrix[i][j];
        }
    }
}

// MODIFICAT: MatrixRule1 - foloseste PointList și matrici globale
void MatrixRule1(char **matrix, int n, int m, POINT **head, char **copie_matrix, char **matrixBordata) {
    int i, j;

    DeleteList(head); // Resetăm head_list pentru o nouă utilizare

    // Copiem matricea originală în copie_matrix
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            copie_matrix[i][j] = matrix[i][j];
        }
    }

    // Umplem matrixBordata cu '+'
    for (i = 0; i < n + 2; i++) {
        for (j = 0; j < m + 2; j++) {
            matrixBordata[i][j] = '+';
        }
    }

    // Copiem conținutul matricei originale în centrul matrixBordata
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            matrixBordata[i + 1][j + 1] = matrix[i][j];
        }
    }

    for (i = 1; i < n + 1; i++) {
        for (j = 1; j < m + 1; j++) {
            int nr_vecini = 0;
            if (matrixBordata[i][j + 1] == 'X') nr_vecini++;
            if (matrixBordata[i][j - 1] == 'X') nr_vecini++;
            if (matrixBordata[i - 1][j] == 'X') nr_vecini++;
            if (matrixBordata[i + 1][j] == 'X') nr_vecini++;
            if (matrixBordata[i - 1][j - 1] == 'X') nr_vecini++;
            if (matrixBordata[i - 1][j + 1] == 'X') nr_vecini++;
            if (matrixBordata[i + 1][j - 1] == 'X') nr_vecini++;
            if (matrixBordata[i + 1][j + 1] == 'X') nr_vecini++;

            if (matrixBordata[i][j] == 'X') {
                if (nr_vecini < 2 || nr_vecini > 3) {
                    if (copie_matrix[i - 1][j - 1] != '+') AddAtEnd(head, i - 1, j - 1);
                    copie_matrix[i - 1][j - 1] = '+';
                }
            } else {
                if (nr_vecini == 3) {
                    if (copie_matrix[i - 1][j - 1] != 'X') AddAtEnd(head, i - 1, j - 1);
                    copie_matrix[i - 1][j - 1] = 'X';
                }
            }
        }
    }

    // Copiem rezultatul în matricea originală
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            matrix[i][j] = copie_matrix[i][j];
        }
    }
}

// MODIFICAT: GetCoordinates
void GetCoordinates(char **matrix, int n, int m, POINT **head) {
    DeleteList(head); // Resetăm head_list

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (matrix[i][j] == 'X')
                AddAtEnd(head, i, j);
        }
    }
}

// MODIFICAT: DeleteTree - primește TreeNODE** pentru a putea modifica rădăcina la NULL
void DeleteTree(TreeNODE **root) {
    if (*root == NULL) return;
    DeleteTree(&((*root)->left));
    DeleteTree(&((*root)->right));
    DeleteList(&((*root)->head)); // Folosim funcția DeleteList
    free(*root);
    *root = NULL; // Asigură că pointerul este NULL după eliberare
}

// MODIFICAT: BuildTree - primește matricile temporare și array-urile globale pentru DFS
void BuildTree(TreeNODE *node, char **matrix, int n, int m, int k, int level, FILE *fisier_output,
               char **global_copie_matrix, char **global_matrixBordata,
               VarfT4 *global_celule_vii, int **global_map_id,
               int *global_cale_crt, int *global_cale_opt, int *global_vizitat) {
    if (level == k) return;

    char **matrixLeft = (char **)malloc(n * sizeof(char *));
    char **matrixRight = (char **)malloc(n * sizeof(char *));
    if (!matrixLeft || !matrixRight) {
        fprintf(stderr, "malloc error for matrixLeft/Right in BuildTree\n");
        // Rezolvare avertizare misleading-indentation
        if (matrixLeft) {
            for(int i=0; i<n; ++i) {
                if(matrixLeft[i]) free(matrixLeft[i]);
            }
            free(matrixLeft);
        }
        if (matrixRight) {
            for(int i=0; i<n; ++i) {
                if(matrixRight[i]) free(matrixRight[i]);
            }
            free(matrixRight);
        }
        return;
    }

    for (int i = 0; i < n; i++) {
        matrixLeft[i] = (char *)malloc(m * sizeof(char));
        matrixRight[i] = (char *)malloc(m * sizeof(char));
        if (!matrixLeft[i] || !matrixRight[i]) {
            fprintf(stderr, "malloc error for matrixLeft/Right row %d in BuildTree\n", i);
            for (int j = 0; j < i; ++j) {
                if(matrixLeft[j]) free(matrixLeft[j]);
                if(matrixRight[j]) free(matrixRight[j]);
            }
            if(matrixLeft) free(matrixLeft);
            if(matrixRight) free(matrixRight);
            return;
        }
        for (int j = 0; j < m; j++) {
            matrixLeft[i][j] = matrix[i][j];
            matrixRight[i][j] = matrix[i][j];
        }
    }

    node->left = (TreeNODE *)malloc(sizeof(TreeNODE));
    if (!node->left) {
        fprintf(stderr, "malloc error for node->left in BuildTree\n");
        for (int i = 0; i < n; i++) { free(matrixLeft[i]); free(matrixRight[i]); }
        free(matrixLeft); free(matrixRight);
        return;
    }
    node->left->head = NULL; // Initializăm head-ul listei
    node->left->left = NULL;
    node->left->right = NULL;
    MatrixRule2(matrixLeft, n, m, &(node->left->head), global_copie_matrix, global_matrixBordata);
    hamiltonian(matrixLeft, n, m, fisier_output, global_celule_vii, global_map_id,
                global_cale_crt, global_cale_opt, global_vizitat);

    BuildTree(node->left, matrixLeft, n, m, k, level + 1, fisier_output,
              global_copie_matrix, global_matrixBordata,
              global_celule_vii, global_map_id,
              global_cale_crt, global_cale_opt, global_vizitat);

    node->right = (TreeNODE *)malloc(sizeof(TreeNODE));
    if (!node->right) {
        fprintf(stderr, "malloc error for node->right in BuildTree\n");
        DeleteTree(&(node->left)); // Folosim DeleteTree modificată
        for (int i = 0; i < n; i++) { free(matrixLeft[i]); free(matrixRight[i]); }
        free(matrixLeft); free(matrixRight);
        return;
    }
    node->right->head = NULL; // Initializăm head-ul listei
    node->right->left = NULL;
    node->right->right = NULL;
    MatrixRule1(matrixRight, n, m, &(node->right->head), global_copie_matrix, global_matrixBordata);
    hamiltonian(matrixRight, n, m, fisier_output, global_celule_vii, global_map_id,
                global_cale_crt, global_cale_opt, global_vizitat);

    BuildTree(node->right, matrixRight, n, m, k, level + 1, fisier_output,
              global_copie_matrix, global_matrixBordata,
              global_celule_vii, global_map_id,
              global_cale_crt, global_cale_opt, global_vizitat);

    for (int i = 0; i < n; i++) {
        free(matrixLeft[i]);
        free(matrixRight[i]);
    }
    free(matrixLeft);
    free(matrixRight);
}

// MODIFICAT: implementTask4 - functie principala de alocare si lansare
void implementTask4(char **matrix, int n, int m, FILE *fisier_output, int gen_K) {
    // Alocăm global matricile temporare pentru Rule1/Rule2
    char **global_copie_matrix = (char **)malloc(n * sizeof(char *));
    char **global_matrixBordata = (char **)malloc((n + 2) * sizeof(char *));

    if (!global_copie_matrix || !global_matrixBordata) {
        fprintf(stderr, "Eroare alocare global_copie_matrix sau global_matrixBordata in implementTask4\n");
        if (global_copie_matrix) free(global_copie_matrix);
        if (global_matrixBordata) free(global_matrixBordata);
        fprintf(fisier_output, "-1\n\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        global_copie_matrix[i] = (char *)malloc(m * sizeof(char));
        if (!global_copie_matrix[i]) {
            fprintf(stderr, "Eroare alocare global_copie_matrix rând %d in implementTask4\n", i);
            for (int k = 0; k < i; ++k) free(global_copie_matrix[k]);
            free(global_copie_matrix);
            for (int k = 0; k < (n + 2); ++k) { // Free matrixBordata already allocated rows
                if (global_matrixBordata[k]) free(global_matrixBordata[k]);
            }
            free(global_matrixBordata);
            fprintf(fisier_output, "-1\n\n");
            return;
        }
    }

    for (int i = 0; i < n + 2; i++) {
        global_matrixBordata[i] = (char *)malloc((m + 2) * sizeof(char));
        if (!global_matrixBordata[i]) {
            fprintf(stderr, "Eroare alocare global_matrixBordata rând %d in implementTask4\n", i);
            for (int k = 0; k < i; ++k) free(global_matrixBordata[k]);
            free(global_matrixBordata);
            for (int k = 0; k < n; ++k) free(global_copie_matrix[k]);
            free(global_copie_matrix);
            fprintf(fisier_output, "-1\n\n");
            return;
        }
    }

    // Alocăm global structurile pentru DFS
    int max_nodes_possible = n * m;
    VarfT4 *global_celule_vii = (VarfT4 *)malloc(max_nodes_possible * sizeof(VarfT4));
    int **global_map_id = (int **)malloc(n * sizeof(int *));
    for(int i = 0; i < n; ++i) {
        global_map_id[i] = (int *)malloc(m * sizeof(int));
    }


    int *global_cale_crt = (int *)malloc(max_nodes_possible * sizeof(int));
    int *global_cale_opt = (int *)malloc(max_nodes_possible * sizeof(int));
    int *global_vizitat = (int *)calloc(max_nodes_possible, sizeof(int)); // calloc initializeaza cu 0

    if (!global_celule_vii || !global_map_id || !global_cale_crt || !global_cale_opt || !global_vizitat) {
        fprintf(stderr, "Eroare alocare global_celule_vii/map_id/cale_crt/opt/vizitat in implementTask4\n");
        if (global_celule_vii) free(global_celule_vii);
        if (global_map_id) {
            for(int i = 0; i < n; ++i) {
                if(global_map_id[i]) free(global_map_id[i]);
            }
            free(global_map_id);
        }
        if (global_cale_crt) free(global_cale_crt);
        if (global_cale_opt) free(global_cale_opt);
        if (global_vizitat) free(global_vizitat);
        // Eliberare matrici temporare
        for (int i = 0; i < n; ++i) {
            if(global_copie_matrix[i]) free(global_copie_matrix[i]);
        }
        free(global_copie_matrix);
        for (int i = 0; i < n + 2; ++i) {
            if(global_matrixBordata[i]) free(global_matrixBordata[i]);
        }
        free(global_matrixBordata);
        fprintf(fisier_output, "-1\n\n");
        return;
    }

    TreeNODE *root = (TreeNODE *)malloc(sizeof(TreeNODE));
    if (!root) {
        fprintf(stderr, "malloc error for root in implementTask4\n");
        // Dealocă tot ce a fost alocat
        if (global_celule_vii) free(global_celule_vii);
        if (global_map_id) {
            for(int i = 0; i < n; ++i) {
                if(global_map_id[i]) free(global_map_id[i]);
            }
            free(global_map_id);
        }
        free(global_cale_crt);
        free(global_cale_opt);
        free(global_vizitat);
        for (int i = 0; i < n; ++i) {
            if(global_copie_matrix[i]) free(global_copie_matrix[i]);
        }
        free(global_copie_matrix);
        for (int i = 0; i < n + 2; ++i) {
            if(global_matrixBordata[i]) free(global_matrixBordata[i]);
        }
        free(global_matrixBordata);
        return;
    }
    root->head = NULL; // Initializăm head-ul listei pentru rădăcină
    root->left = NULL;
    root->right = NULL;

    hamiltonian(matrix, n, m, fisier_output,
                global_celule_vii, global_map_id,
                global_cale_crt, global_cale_opt, global_vizitat);

    if (gen_K > 0) {
        BuildTree(root, matrix, n, m, gen_K, 0, fisier_output,
                  global_copie_matrix, global_matrixBordata,
                  global_celule_vii, global_map_id,
                  global_cale_crt, global_cale_opt, global_vizitat);
    }

    DeleteTree(&root); // Folosim DeleteTree modificată

    // Eliberăm memoria alocată global
    free(global_celule_vii);
    if (global_map_id) {
        for(int i = 0; i < n; ++i) {
            if(global_map_id[i]) free(global_map_id[i]);
        }
        free(global_map_id);
    }
    free(global_cale_crt);
    free(global_cale_opt);
    free(global_vizitat);
    for (int i = 0; i < n; ++i) {
        if(global_copie_matrix[i]) free(global_copie_matrix[i]);
    }
    free(global_copie_matrix);
    for (int i = 0; i < n + 2; ++i) {
        if(global_matrixBordata[i]) free(global_matrixBordata[i]);
    }
    free(global_matrixBordata);
}