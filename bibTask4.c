#include "bibTask4.h" // Include noul header
#include <string.h> // Pentru memcpy și memset

// Declarații globale pentru direcții (rămân la fel)
int dirR[] = {-1, -1, -1, 0, 0, 1, 1, 1};
int dirC[] = {-1, 0, 1, -1, 1, -1, 0, 1};

// Structura pentru a păstra datele necesare DFS-ului Hamiltonian
// Aceste structuri nu mai sunt redefinite aici, vin din header
typedef struct {
    int *visited;          // Array de vizitați pentru nodurile componentei
    Position *current_path; // Calea curentă
    int current_length;    // Lungimea curentă a căii
    int *max_length;       // Pointer la lungimea maximă găsită până acum
    Position *best_path;   // Pointer la calea cea mai bună găsită până acum
    int total_nodes_in_component; // Numărul total de noduri în componenta curentă
    AdjList *adj_lists;    // Listele de adiacență pentru componentă
    Position *component_nodes; // Nodurile componentei (pentru conversie index_local -> coordonate)
} DFSData;

// Functie de comparare pentru qsort pentru Position
int comparePositions(const void *a, const void *b) {
    Position *posA = (Position *)a;
    Position *posB = (Position *)b;
    if (posA->row != posB->row) {
        return posA->row - posB->row;
    }
    return posA->col - posB->col;
}

// Functie pentru adaugarea unui vecin sortat in lista de adiacenta
void addSortedNeighbor(AdjListNode **head, int local_idx, Position *all_component_nodes) {
    AdjListNode *newNode = (AdjListNode *)malloc(sizeof(AdjListNode));
    if (!newNode) {
        perror("Failed to allocate AdjListNode");
        return;
    }
    newNode->local_idx = local_idx;
    newNode->next = NULL;

    if (*head == NULL || comparePositions(&all_component_nodes[newNode->local_idx], &all_component_nodes[(*head)->local_idx]) < 0) {
        newNode->next = *head;
        *head = newNode;
    } else {
        AdjListNode *current = *head;
        while (current->next != NULL && comparePositions(&all_component_nodes[newNode->local_idx], &all_component_nodes[current->next->local_idx]) > 0) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// Funcție pentru a elibera o listă de adiacență (care este un array de AdjList)
void freeAdjList(AdjList *adj_lists_array, int num_nodes) { // Am redenumit parametrul
    if (!adj_lists_array) return;
    for (int i = 0; i < num_nodes; i++) {
        AdjListNode *current = adj_lists_array[i].head;
        while (current) {
            AdjListNode *temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(adj_lists_array);
}

int countNodes(char **matrix, int n, int m) {
    int cnt = 0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            if (matrix[i][j] == 'X')
                cnt++;
    return cnt;
}

// Verifică dacă două lanțuri sunt lexicografic ordonate conform regulilor
int isBetter(Position *a, Position *b, int len) {
    for (int i = 0; i < len; i++) {
        if (a[i].row < b[i].row) return 1;
        if (a[i].row > b[i].row) return 0;
        if (a[i].col < b[i].col) return 1;
        if (a[i].col > b[i].col) return 0;
    }
    return 0; // sunt identice
}

void dfs_component(char **matrix, int n, int m, int r, int c, int **visited_global, Position *component_nodes, int *component_count) {
    component_nodes[*component_count] = (Position){r, c};
    (*component_count)++;
    visited_global[r][c] = 1;

    for (int d = 0; d < 8; d++) {
        int nr = r + dirR[d];
        int nc = c + dirC[d];
        if (nr >= 0 && nr < n && nc >= 0 && nc < m && matrix[nr][nc] == 'X' && !visited_global[nr][nc]) {
            dfs_component(matrix, n, m, nr, nc, visited_global, component_nodes, component_count);
        }
    }
}

ConnectedComponent *findConnectedComponents(char **matrix, int n, int m, int *num_components) {
    int **visited_global = (int **)malloc(n * sizeof(int *));
    if (!visited_global) {
        perror("Failed to allocate visited_global rows");
        *num_components = 0;
        return NULL;
    }
    for (int i = 0; i < n; i++) {
        visited_global[i] = (int *)calloc(m, sizeof(int));
        if (!visited_global[i]) {
            perror("Failed to allocate visited_global columns");
            for (int k = 0; k < i; k++) free(visited_global[k]);
            free(visited_global);
            *num_components = 0;
            return NULL;
        }
    }

    ConnectedComponent *components = NULL;
    *num_components = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (matrix[i][j] == 'X' && !visited_global[i][j]) {
                // Realocare pentru a adăuga o nouă componentă
                ConnectedComponent *temp_components = (ConnectedComponent *)realloc(components, (*num_components + 1) * sizeof(ConnectedComponent));
                if (!temp_components) {
                    perror("Failed to reallocate components");
                    // Cleanup existing components and visited_global
                    for (int k = 0; k < *num_components; k++) {
                        if (components[k].nodes) free(components[k].nodes);
                        if (components[k].adj_lists) freeAdjList(components[k].adj_lists, components[k].count);
                    }
                    free(components);
                    for (int k = 0; k < n; k++) free(visited_global[k]);
                    free(visited_global);
                    *num_components = 0;
                    return NULL;
                }
                components = temp_components;

                Position *current_component_nodes = (Position *)malloc(n * m * sizeof(Position));
                if (!current_component_nodes) {
                    perror("Failed to allocate current_component_nodes");
                    // Cleanup and return
                    for (int k = 0; k < *num_components; k++) {
                        if (components[k].nodes) free(components[k].nodes);
                        if (components[k].adj_lists) freeAdjList(components[k].adj_lists, components[k].count);
                    }
                    free(components);
                    for (int k = 0; k < n; k++) free(visited_global[k]);
                    free(visited_global);
                    *num_components = 0;
                    return NULL;
                }
                int current_component_count = 0;

                dfs_component(matrix, n, m, i, j, visited_global, current_component_nodes, &current_component_count);

                components[*num_components].nodes = (Position *)malloc(current_component_count * sizeof(Position));
                if (!components[*num_components].nodes) {
                    perror("Failed to allocate component nodes for final struct");
                    free(current_component_nodes);
                    // Cleanup and return
                    for (int k = 0; k < *num_components; k++) {
                        if (components[k].nodes) free(components[k].nodes);
                        if (components[k].adj_lists) freeAdjList(components[k].adj_lists, components[k].count);
                    }
                    free(components);
                    for (int k = 0; k < n; k++) free(visited_global[k]);
                    free(visited_global);
                    *num_components = 0;
                    return NULL;
                }
                memcpy(components[*num_components].nodes, current_component_nodes, current_component_count * sizeof(Position));
                components[*num_components].count = current_component_count;

                // Sortarea nodurilor componentei pentru o mapare consistentă (opțional, dar bun pentru debug)
                qsort(components[*num_components].nodes, current_component_count, sizeof(Position), comparePositions);

                // Construirea listelor de adiacență sortate pentru această componentă
                AdjList *adj_lists = (AdjList *)calloc(current_component_count, sizeof(AdjList));
                if (!adj_lists) {
                    perror("Failed to allocate adj_lists for component");
                    free(components[*num_components].nodes);
                    free(current_component_nodes);
                    // Cleanup and return
                    for (int k = 0; k < *num_components; k++) {
                        if (components[k].nodes) free(components[k].nodes);
                        if (components[k].adj_lists) freeAdjList(components[k].adj_lists, components[k].count);
                    }
                    free(components);
                    for (int k = 0; k < n; k++) free(visited_global[k]);
                    free(visited_global);
                    *num_components = 0;
                    return NULL;
                }

                // Mapare coordonate -> index local
                int **coord_to_local_idx = (int **)malloc(n * sizeof(int *));
                if (!coord_to_local_idx) {
                    perror("Failed to allocate coord_to_local_idx rows");
                    free(adj_lists);
                    free(components[*num_components].nodes);
                    free(current_component_nodes);
                    for (int k = 0; k < *num_components; k++) {
                        if (components[k].nodes) free(components[k].nodes);
                        if (components[k].adj_lists) freeAdjList(components[k].adj_lists, components[k].count);
                    }
                    free(components);
                    for (int k = 0; k < n; k++) free(visited_global[k]);
                    free(visited_global);
                    *num_components = 0;
                    return NULL;
                }
                for (int row = 0; row < n; row++) {
                    coord_to_local_idx[row] = (int *)malloc(m * sizeof(int));
                    if (!coord_to_local_idx[row]) {
                        perror("Failed to allocate coord_to_local_idx cols");
                        for(int k=0; k<row; k++) free(coord_to_local_idx[k]);
                        free(coord_to_local_idx);
                        free(adj_lists);
                        free(components[*num_components].nodes);
                        free(current_component_nodes);
                        for (int k = 0; k < *num_components; k++) {
                            if (components[k].nodes) free(components[k].nodes);
                            if (components[k].adj_lists) freeAdjList(components[k].adj_lists, components[k].count);
                        }
                        free(components);
                        for (int k = 0; k < n; k++) free(visited_global[k]);
                        free(visited_global);
                        *num_components = 0;
                        return NULL;
                    }
                    for (int col = 0; col < m; col++) {
                        coord_to_local_idx[row][col] = -1; // Inițializare cu -1
                    }
                }

                for (int k = 0; k < current_component_count; k++) {
                    coord_to_local_idx[components[*num_components].nodes[k].row][components[*num_components].nodes[k].col] = k;
                }

                for (int k = 0; k < current_component_count; k++) {
                    Position p = components[*num_components].nodes[k];
                    for (int d = 0; d < 8; d++) {
                        int nr = p.row + dirR[d];
                        int nc = p.col + dirC[d];
                        if (nr >= 0 && nr < n && nc >= 0 && nc < m && matrix[nr][nc] == 'X') {
                            int neighbor_local_idx = coord_to_local_idx[nr][nc];
                            if (neighbor_local_idx != -1) { // Este un vecin din aceeași componentă
                                addSortedNeighbor(&adj_lists[k].head, neighbor_local_idx, components[*num_components].nodes);
                                adj_lists[k].degree++;
                            }
                        }
                    }
                }
                // Atribuirea corectă a pointerului la array-ul de AdjList
                components[*num_components].adj_lists = adj_lists; // Salvează listele de adiacență

                // Eliberează memoria pentru maparea coord_to_local_idx
                for (int row = 0; row < n; row++) {
                    free(coord_to_local_idx[row]);
                }
                free(coord_to_local_idx);
                free(current_component_nodes); // Eliberează memoria temporară

                (*num_components)++;
            }
        }
    }

    // Acestea nu sunt in interiorul buclelor for
    for (int i = 0; i < n; i++) {
        free(visited_global[i]);
    }
    free(visited_global);

    return components;
}

// DFS pentru a găsi drumul Hamiltonian
void dfs_hamiltonian(DFSData *data, int current_local_idx) {
    data->visited[current_local_idx] = 1;
    data->current_path[data->current_length++] = data->component_nodes[current_local_idx];

    if (data->current_length == data->total_nodes_in_component) {
        // Am găsit un drum Hamiltonian complet
        if (data->current_length > *(data->max_length) ||
            (*(data->max_length) != -1 && isBetter(data->current_path, data->best_path, data->current_length))) {
            *(data->max_length) = data->current_length;
            memcpy(data->best_path, data->current_path, data->current_length * sizeof(Position));
        }
    } else {
        // Continuă explorarea
        AdjListNode *neighbor = data->adj_lists[current_local_idx].head;
        while (neighbor != NULL) {
            if (!data->visited[neighbor->local_idx]) {
                dfs_hamiltonian(data, neighbor->local_idx);
            }
            neighbor = neighbor->next;
        }
    }

    // Backtrack
    data->visited[current_local_idx] = 0;
    data->current_length--;
}


void findBestHamiltonianChain(char **matrix, int n, int m, FILE *fisier_output) {
    int num_components;
    ConnectedComponent *components = findConnectedComponents(matrix, n, m, &num_components);

    if (!components && num_components == 0) { // Eroare la alocare sau nu sunt componente
        fprintf(fisier_output, "-1\n");
        return;
    }

    Position *global_best_path = NULL;
    int global_max_length = -1;

    // Buffer-e pentru DFS
    int *visited_component_nodes = NULL;
    Position *current_path_buffer = NULL;
    Position *best_path_component_buffer = NULL;

    // Acestea trebuie sa fie alocate la dimensiunea maxima posibila (n*m)
    // pentru ca DFSData sa poata pointa catre ele
    // Dar sunt alocate o singura data si reutilizate.
    visited_component_nodes = (int *)malloc(n * m * sizeof(int));
    current_path_buffer = (Position *)malloc(n * m * sizeof(Position));
    best_path_component_buffer = (Position *)malloc(n * m * sizeof(Position));

    if (!visited_component_nodes || !current_path_buffer || !best_path_component_buffer) {
        perror("Failed to allocate DFS buffers");
        if (visited_component_nodes) free(visited_component_nodes);
        if (current_path_buffer) free(current_path_buffer);
        if (best_path_component_buffer) free(best_path_component_buffer);
        // Eliberarea componentelor deja găsite
        for (int i = 0; i < num_components; i++) {
            if (components[i].nodes) free(components[i].nodes);
            // Am redenumit parametrul în freeAdjList, trebuie să-l folosesc corect
            if (components[i].adj_lists) freeAdjList(components[i].adj_lists, components[i].count); 
        }
        if (components) free(components);
        fprintf(fisier_output, "-1\n"); // No path if memory allocation fails
        return;
    }

    for (int comp_idx = 0; comp_idx < num_components; comp_idx++) {
        int nr_noduri_comp = components[comp_idx].count;

        // Pruning: Componentele cu 0, 1 nod sau noduri izolate/de grad 1 în exces
        if (nr_noduri_comp == 0) continue; // Componentă goală, improbabil, dar safety
        if (nr_noduri_comp == 1) { // Un singur nod
            // Un singur nod este un drum de lungime 1 (0 muchii)
            int current_len = 1;
            Position single_node_path[1] = {components[comp_idx].nodes[0]};

            if (current_len > global_max_length || 
                (current_len == global_max_length && isBetter(single_node_path, global_best_path, current_len))) {
                global_max_length = current_len;
                if (global_best_path) free(global_best_path);
                global_best_path = (Position *)malloc(global_max_length * sizeof(Position));
                if (!global_best_path) { perror("Failed to alloc global_best_path"); break; } // Handle serious error
                memcpy(global_best_path, single_node_path, global_max_length * sizeof(Position));
            }
            continue;
        }

        // Pruning pentru noduri de grad 0 sau 1
        int nodes_with_degree_one = 0;
        int has_isolated_node = 0;
        for (int k = 0; k < nr_noduri_comp; k++) {
            if (components[comp_idx].adj_lists[k].degree == 0) {
                has_isolated_node = 1; // Un nod izolat într-o componentă > 1 nod
                break;
            }
            if (components[comp_idx].adj_lists[k].degree == 1) {
                nodes_with_degree_one++;
            }
        }

        // Dacă are nod izolat (și nu e o componentă de 1 nod), nu poate fi Hamiltonian
        if (has_isolated_node) {
            continue;
        }
        // Dacă are mai mult de 2 noduri de grad 1, nu poate fi un drum Hamiltonian
        if (nr_noduri_comp > 2 && nodes_with_degree_one > 2) {
            continue;
        }


        // Găsește nodul de start lexicografic minim pentru componentă
        // Nodurile componentei sunt deja sortate după `qsort` în `findConnectedComponents`
        // deci components[comp_idx].nodes[0] este cel lexicografic minim.
        Position start_node_coords = components[comp_idx].nodes[0];
        
        // Găsește indexul local al nodului de start
        int start_local_idx = -1;
        for(int k=0; k<nr_noduri_comp; k++) {
            if (components[comp_idx].nodes[k].row == start_node_coords.row &&
                components[comp_idx].nodes[k].col == start_node_coords.col) {
                start_local_idx = k;
                break;
            }
        }
        
        if (start_local_idx == -1) { // Nu ar trebui să se întâmple dacă logica e corectă
             continue;
        }

        // Reset visited pentru această componentă
        memset(visited_component_nodes, 0, nr_noduri_comp * sizeof(int)); // Important!

        int current_max_length_component = -1;

        DFSData dfs_data = {
            .visited = visited_component_nodes,
            .current_path = current_path_buffer,
            .current_length = 0,
            .max_length = &current_max_length_component,
            .best_path = best_path_component_buffer,
            .total_nodes_in_component = nr_noduri_comp,
            .adj_lists = components[comp_idx].adj_lists, // Aici era warning-ul, este corect
            .component_nodes = components[comp_idx].nodes
        };

        dfs_hamiltonian(&dfs_data, start_local_idx);
        
        // Compară rezultatul componentei curente cu cel mai bun rezultat global
        if (current_max_length_component > global_max_length) {
            global_max_length = current_max_length_component;
            if (global_best_path) free(global_best_path);
            global_best_path = (Position *)malloc(global_max_length * sizeof(Position));
            if (!global_best_path) { perror("Failed to alloc global_best_path"); break; } // Handle serious error
            memcpy(global_best_path, best_path_component_buffer, global_max_length * sizeof(Position));
        } else if (current_max_length_component == global_max_length && current_max_length_component != -1) {
            if (isBetter(best_path_component_buffer, global_best_path, global_max_length)) {
                memcpy(global_best_path, best_path_component_buffer, global_max_length * sizeof(Position));
            }
        }
    }

    if (global_max_length <= 0) { // Un lanț de 1 nod are lungimea 1 (număr de noduri), 0 muchii.
                                 // Cerința e să afișăm numărul de muchii.
                                 // Deci, un lanț de 1 nod e 0 muchii. Un lanț de N noduri e N-1 muchii.
                                 // Dacă global_max_length e 1, înseamnă 0 muchii.
        fprintf(fisier_output, "-1\n");
    } else {
        fprintf(fisier_output, "%d\n", global_max_length - 1); // Afiseaza numarul de muchii
        for (int i = 0; i < global_max_length; i++) {
            fprintf(fisier_output, "(%d,%d) ", global_best_path[i].row, global_best_path[i].col);
        }
        fprintf(fisier_output, "\n");
    }

    // Eliberare memorie
    if (global_best_path) free(global_best_path);
    if (visited_component_nodes) free(visited_component_nodes);
    if (current_path_buffer) free(current_path_buffer);
    if (best_path_component_buffer) free(best_path_component_buffer);
    
    for (int i = 0; i < num_components; i++) {
        if (components[i].nodes) free(components[i].nodes);
        // Parametrul freeAdjList trebuie să fie de tipul corect
        if (components[i].adj_lists) freeAdjList(components[i].adj_lists, components[i].count);
    }
    if (components) free(components);
}

void AddAtBeginning(POINT **head, int x, int y) {
    POINT *newNode = (POINT *)malloc(sizeof(POINT));
    if (!newNode) { perror("Failed to allocate POINT"); return; }
    newNode->valX = x;
    newNode->valY = y;
    newNode->next = *head;
    *head = newNode;
}

void AddAtEnd(POINT **head, int x, int y) {
    if (*head == NULL) {
        AddAtBeginning(head, x, y);
    } else {
        POINT *newNode = (POINT *)malloc(sizeof(POINT));
        if (!newNode) { perror("Failed to allocate POINT"); return; }
        newNode->valX = x;
        newNode->valY = y;
        newNode->next = NULL; 
        POINT *aux = *head;
        while (aux->next != NULL) {
            aux = aux->next;
        }
        aux->next = newNode;
    }
}

void MatrixRule2(char **matrix, int n, int m, POINT **head) {
    int i, j;
    char **copie_matrix = (char **)malloc(n * sizeof(char *));
    if (!copie_matrix) { perror("Failed to alloc copie_matrix rows"); return; }
    for (i = 0; i < n; i++) {
        copie_matrix[i] = (char *)malloc(m * sizeof(char));
        if (!copie_matrix[i]) { 
            perror("Failed to alloc copie_matrix cols"); 
            for(int k=0; k<i; k++) free(copie_matrix[k]); 
            free(copie_matrix); 
            return; 
        }
        for (j = 0; j < m; j++)
            copie_matrix[i][j] = matrix[i][j];
    }

    char **matrixBordata = (char **)malloc((n + 2) * sizeof(char *));
    if (!matrixBordata) { 
        perror("Failed to alloc matrixBordata rows"); 
        for(int k=0; k<n; k++) free(copie_matrix[k]); 
        free(copie_matrix); 
        return; 
    }
    for (i = 0; i < n + 2; i++) {
        matrixBordata[i] = (char *)malloc((m + 2) * sizeof(char));
        if (!matrixBordata[i]) { 
            perror("Failed to alloc matrixBordata cols"); 
            for(int k=0; k<i; k++) free(matrixBordata[k]); 
            free(matrixBordata); 
            for(int k=0; k<n; k++) free(copie_matrix[k]); 
            free(copie_matrix); 
            return; 
        }
        for (j = 0; j < m + 2; j++)
            matrixBordata[i][j] = '-';
    }

    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            matrixBordata[i + 1][j + 1] = matrix[i][j];
        }
    }

    for (i = 1; i < n + 1; i++) {
        for (j = 1; j < m + 1; j++) {
            int nr_vecini = 0;
            // Check all 8 neighbors (including diagonals)
            for (int d = 0; d < 8; d++) {
                int r_neigh = i + dirR[d];
                int c_neigh = j + dirC[d];
                // Check if neighbor is within bordered matrix bounds and is 'X'
                if (r_neigh >= 0 && r_neigh < n + 2 && c_neigh >= 0 && c_neigh < m + 2 && matrixBordata[r_neigh][c_neigh] == 'X') {
                    nr_vecini++;
                }
            }

            // Rule 2 specific logic (cells with 2 neighbors stay alive/become alive)
            if (nr_vecini == 2) {
                // If it was not 'X' and now becomes 'X', add to list
                if (copie_matrix[i - 1][j - 1] != 'X') {
                    AddAtEnd(head, i - 1, j - 1);
                }
                copie_matrix[i - 1][j - 1] = 'X';
            } else if (matrixBordata[i][j] == 'X') { // Cells with 'X' and not 2 neighbors die (become not 'X')
                if (copie_matrix[i-1][j-1] == 'X') { // Only add to list if it changes from 'X'
                     AddAtEnd(head, i - 1, j - 1);
                }
                copie_matrix[i-1][j-1] = '-'; // Mark as dead
            } else { // Cells that were not 'X' and don't become 'X', remain as they were
                copie_matrix[i-1][j-1] = matrixBordata[i][j]; // Maintain original char
            }
        }
    }

    for (i = 0; i < n; i++) {
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

void MatrixRule1(char **matrix, int n, int m, POINT **head) {
    int i, j;
    char **copie_matrix = (char **)malloc(n * sizeof(char *));
    if (!copie_matrix) { perror("Failed to alloc copie_matrix rows"); return; }
    for (i = 0; i < n; i++) {
        copie_matrix[i] = (char *)malloc(m * sizeof(char));
        if (!copie_matrix[i]) { 
            perror("Failed to alloc copie_matrix cols"); 
            for(int k=0; k<i; k++) free(copie_matrix[k]); 
            free(copie_matrix); 
            return; 
        }
        for (j = 0; j < m; j++)
            copie_matrix[i][j] = matrix[i][j];
    }

    char **matrixBordata = (char **)malloc((n + 2) * sizeof(char *));
    if (!matrixBordata) { 
        perror("Failed to alloc matrixBordata rows"); 
        for(int k=0; k<n; k++) free(copie_matrix[k]); 
        free(copie_matrix); 
        return; 
    }
    for (i = 0; i < n + 2; i++) {
        matrixBordata[i] = (char *)malloc((m + 2) * sizeof(char));
        if (!matrixBordata[i]) { 
            perror("Failed to alloc matrixBordata cols"); 
            for(int k=0; k<i; k++) free(matrixBordata[k]); 
            free(matrixBordata); 
            for(int k=0; k<n; k++) free(copie_matrix[k]); 
            free(copie_matrix); 
            return; 
        }
        for (j = 0; j < m + 2; j++)
            matrixBordata[i][j] = '-';
    }

    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            matrixBordata[i + 1][j + 1] = matrix[i][j];
        }
    }

    for (i = 1; i < n + 1; i++) {
        for (j = 1; j < m + 1; j++) {
            int nr_vecini = 0;
            // Check all 8 neighbors (including diagonals)
            for (int d = 0; d < 8; d++) {
                int r_neigh = i + dirR[d];
                int c_neigh = j + dirC[d];
                // Check if neighbor is within bordered matrix bounds and is 'X'
                if (r_neigh >= 0 && r_neigh < n + 2 && c_neigh >= 0 && c_neigh < m + 2 && matrixBordata[r_neigh][c_neigh] == 'X') {
                    nr_vecini++;
                }
            }

            if (matrixBordata[i][j] == 'X') {
                if (nr_vecini < 2 || nr_vecini > 3) {
                    if (copie_matrix[i-1][j-1] == 'X') { // Only add if changes
                         AddAtEnd(head, i - 1, j - 1);
                    }
                    copie_matrix[i - 1][j - 1] = '-'; // Dies
                } else { // Stays alive (2 or 3 neighbors)
                    copie_matrix[i - 1][j - 1] = 'X';
                }
            } else { // Was not 'X'
                if (nr_vecini == 3) { // Becomes alive
                    if (copie_matrix[i-1][j-1] != 'X') { // Only add if changes
                         AddAtEnd(head, i - 1, j - 1);
                    }
                    copie_matrix[i - 1][j - 1] = 'X';
                } else {
                    copie_matrix[i - 1][j - 1] = matrixBordata[i][j]; // Remains dead (or whatever it was)
                }
            }
        }
    }

    for (i = 0; i < n; i++) {
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

void GetCoordinates(char **matrix, int n, int m, POINT **head) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (matrix[i][j] == 'X')
                AddAtEnd(head, i, j);
        }
    }
}

void BuildTree(TreeNODE *node, char **matrix, int n, int m, int k, int level, FILE *fisier_output) {
    if (level == k)
        return;

    char **matrixLeft = (char **)malloc(n * sizeof(char *));
    char **matrixRight = (char **)malloc(n * sizeof(char *));
    if (!matrixLeft || !matrixRight) {
        perror("Failed to alloc matrix for tree branches");
        // Handle error: free partially allocated memory before returning
        if (matrixLeft) { for(int i=0; i<n; i++) if(matrixLeft[i]) free(matrixLeft[i]); }
        if (matrixRight) { for(int i=0; i<n; i++) if(matrixRight[i]) free(matrixRight[i]); }
        free(matrixLeft); free(matrixRight); // Free the pointers to pointers
        return;
    }

    for (int i = 0; i < n; i++) {
        matrixLeft[i] = (char *)malloc(m * sizeof(char));
        matrixRight[i] = (char *)malloc(m * sizeof(char));
        if (!matrixLeft[i] || !matrixRight[i]) {
             perror("Failed to alloc matrix rows for tree branches");
             // Handle error: free partially allocated memory before returning
             for(int k=0; k<=i; k++) { if(matrixLeft[k]) free(matrixLeft[k]); if(matrixRight[k]) free(matrixRight[k]); }
             free(matrixLeft); free(matrixRight); // Free the pointers to pointers
             return;
        }
        for (int j = 0; j < m; j++) {
            matrixLeft[i][j] = matrix[i][j];
            matrixRight[i][j] = matrix[i][j];
        }
    }

    node->left = (TreeNODE *)malloc(sizeof(TreeNODE));
    if (!node->left) { perror("Failed to alloc left node"); 
        // Cleanup matrices before returning
        for (int i = 0; i < n; i++) { free(matrixLeft[i]); free(matrixRight[i]); }
        free(matrixLeft); free(matrixRight);
        return;
    }
    node->left->head = NULL;
    node->left->left = NULL;
    node->left->right = NULL;
    MatrixRule2(matrixLeft, n, m, &(node->left->head));
    findBestHamiltonianChain(matrixLeft, n, m, fisier_output);

    BuildTree(node->left, matrixLeft, n, m, k, level + 1, fisier_output);

    node->right = (TreeNODE *)malloc(sizeof(TreeNODE));
    if (!node->right) { perror("Failed to alloc right node");
        // Cleanup matrices and left node before returning
        for (int i = 0; i < n; i++) { free(matrixLeft[i]); free(matrixRight[i]); }
        free(matrixLeft); free(matrixRight);
        // Delete the left subtree that was already built
        // Atenție: DeleteTree va elibera și head-ul, care poate fi NULL aici.
        DeleteTree(&(node->left)); 
        return;
    }
    node->right->head = NULL;
    node->right->left = NULL;
    node->right->right = NULL;
    MatrixRule1(matrixRight, n, m, &(node->right->head));
    findBestHamiltonianChain(matrixRight, n, m, fisier_output);

    BuildTree(node->right, matrixRight, n, m, k, level + 1, fisier_output);

    for (int i = 0; i < n; i++) {
        free(matrixLeft[i]);
        free(matrixRight[i]);
    }
    free(matrixLeft);
    free(matrixRight);
}

void DeleteList(POINT **head) {
    POINT *current = *head;
    while (current != NULL) {
        POINT *temp = current;
        current = current->next;
        free(temp);
    }
    *head = NULL;
}

void DeleteTree(TreeNODE **root) {
    if (*root == NULL)
        return;

    DeleteTree(&(*root)->left);
    DeleteTree(&(*root)->right);
    DeleteList(&(*root)->head); // Eliberează lista de puncte asociată nodului
    free(*root);
    *root = NULL;
}

void implementTask4(char **matrix, int n, int m, FILE *fisier_output, int gen) {
    POINT *headRoot = NULL;
    GetCoordinates(matrix, n, m, &headRoot);
    
    TreeNODE *root = (TreeNODE *)malloc(sizeof(TreeNODE));
    if (!root) { 
        perror("Failed to alloc root node"); 
        DeleteList(&headRoot); 
        return; 
    }
    
    root->head = headRoot;
    root->left = NULL;
    root->right = NULL;
    
    findBestHamiltonianChain(matrix, n, m, fisier_output);
    BuildTree(root, matrix, n, m, gen, 0, fisier_output);
    
    DeleteTree(&root);
}