#include "hash_table.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct {
    int indicator_lights[10];
    int joltage_levels[10];
    int buttons[15][10];
    int no_buttons;
} Machine;
Machine machines[200];
int no_machines = 0;

typedef struct {
    int items[1024][10];
    int head;
    int tail;
} Queue;

bool parse_file(const char *filename);
void parse_input(FILE *f);
void parse_machine(char *line, Machine *m);
void print_machine(const Machine *m);
void enqueue(Queue *q, int *item);
void dequeue(Queue *q, int *item);
bool is_empty(Queue *q);
void bfs(Machine *m, HashTable *dists);
int find_min_presses(Machine *m);
bool list_equals(const int a[10], const int b[10]);
void number_list_to_string(const int numbers[10], char *out);
int get_adj_states(Machine *m, int state[10], int adj_states[15][10]);
bool apply_indicator_button(Machine *m, int state[10], int button_index, int new_state[10]);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }

    if (!parse_file(argv[1])) {
        return 1;
    }

    // for (int i = 0; i < no_machines; i++) {
    //     print_machine(&machines[i]);
    // }
    //

    int total_presses = 0;
    for (int i = 0; i < no_machines; i++) {
        int presses = find_min_presses(&machines[i]);
        printf("Machine %d presses: %d\n", i, presses);
        total_presses += presses;
    }
    printf("Total presses: %d\n", total_presses);

    return 0;
}

int ht_get_by_list(HashTable *ht, const int list[10]) {
    char key[64];
    number_list_to_string(list, key);
    int *res = ht_get(ht, key);
    return res ? *res : -1;
}

void ht_set_by_list(HashTable *ht, const int list[10], int value) {
    char key[64];
    number_list_to_string(list, key);
    ht_set(ht, key, value);
}

int find_min_presses(Machine *m) {
    HashTable *dists = ht_create();

    bfs(m, dists);
    int result = ht_get_by_list(dists, m->indicator_lights);
    assert(result != -1);
    ht_free(dists);
    return result;
}

void bfs(Machine *m, HashTable *dists) {
    Queue q = {0};
    int start[10] = {0};
    int state[10];
    int adj_states[15][10];
    int adj_count;
    enqueue(&q, start);
    ht_set_by_list(dists, start, 0);
    while (!is_empty(&q)) {
        dequeue(&q, state);
        if (list_equals(state, m->indicator_lights)) {
            return;
        }
        adj_count = get_adj_states(m, state, adj_states);
        for (int i = 0; i < adj_count; i++) {
            if (ht_get_by_list(dists, adj_states[i]) == -1) {
                int dist = ht_get_by_list(dists, state);
                ht_set_by_list(dists, adj_states[i], dist + 1);
                enqueue(&q, adj_states[i]);
            }
        }
    }
}

bool list_equals(const int a[10], const int b[10]) {
    for (int i = 0; i < 10; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

void enqueue(Queue *q, int *item) {
    memcpy(q->items[q->tail], item, 10 * sizeof(int));
    q->tail = (q->tail + 1) % 1024;
}

void dequeue(Queue *q, int *item) {
    memcpy(item, q->items[q->head], 10 * sizeof(int));
    q->head = (q->head + 1) % 1024;
}

bool is_empty(Queue *q) { return q->head == q->tail; }

int get_adj_states(Machine *m, int state[10], int adj_states[15][10]) {
    int adj_count = 0;
    for (int i = 0; i < m->no_buttons; i++) {
        bool valid = true;
        int new_state[10];
        valid = apply_indicator_button(m, state, i, new_state);
        if (valid) {
            memcpy(adj_states[adj_count], new_state, 10 * sizeof(int));
            adj_count++;
        }
    }
    return adj_count;
}

bool apply_indicator_button(Machine *m, int state[10], int button_index, int new_state[10]) {
    for (int i = 0; i < 10; i++) {
        new_state[i] = state[i] ^ m->buttons[button_index][i];
    }
    return true;
}

void number_list_to_string(const int numbers[10], char *out) {
    char buffer[64];
    out[0] = '\0';
    for (int i = 0; i < 10; i++) {
        if (i > 0) {
            strcat(out, ",");
        }
        sprintf(buffer, "%d", numbers[i]);
        strcat(out, buffer);
    }
}

void parse_bool_list(const char *s, int state[10]) {
    for (size_t i = 0; i < 10 && s[i] != '\0'; i++) {
        if (s[i] == '#') {
            state[i] = 1;
        } else {
            state[i] = 0;
        }
    }
}

void parse_position_list(const char *s, int positions[10]) {
    memset(positions, 0, 10 * sizeof(int));
    char *token;
    char *str = strdup(s);
    token = strtok(str, ",");
    while (token != NULL) {
        int pos = atoi(token);
        if (pos >= 0 && pos < 10) {
            positions[pos] = 1;
        }
        token = strtok(NULL, ",");
    }
    free(str);
}

void parse_number_list(const char *s, int button[10]) {
    char *token;
    char *str = strdup(s);
    int i = 0;
    token = strtok(str, ",");
    while (token != NULL && i < 10) {
        button[i] = atoi(token);
        token = strtok(NULL, ",");
        i++;
    }
    free(str);
}

void parse_machine(char *line, Machine *m) {
    char item[64];
    int n;
    sscanf(line, "[%[.#]]%n", item, &n);
    parse_bool_list(item, m->indicator_lights);
    line += n;

    while (sscanf(line, " (%[0-9,])%n", item, &n)) {
        parse_position_list(item, m->buttons[m->no_buttons]);
        m->no_buttons++;
        line += n;
    }

    sscanf(line, " {%[0-9,]}", item);
    parse_number_list(item, m->joltage_levels);
}

void print_machine(const Machine *m) {
    printf("Indicator lights:\n");
    for (int i = 0; i < 10; i++) {
        printf("%d,", m->indicator_lights[i]);
    }
    printf("\nButtons:\n");
    for (int i = 0; i < m->no_buttons; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%d,", m->buttons[i][j]);
        }
        printf("\n");
    }
    printf("Joltage levels:\n");
    for (int i = 0; i < 10; i++) {
        printf("%d,", m->joltage_levels[i]);
    }
    printf("\n\n");
}

void parse_input(FILE *f) {
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        parse_machine(line, &machines[no_machines]);
        no_machines++;
    }
    printf("Parsed %d machines\n", no_machines);
}

bool parse_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        return false;
    }

    parse_input(f);

    if (ferror(f)) {
        perror("read error");
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}
