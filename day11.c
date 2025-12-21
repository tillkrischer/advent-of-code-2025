#include "hash_table.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    char key[4];
    int connections[25];
    int num_connections;
} Device;

bool parse_file(const char *filename);
void print_devices();
int get_device_id(char *name);
long dfs(int device_id, int dac, int fft);

HashTable *device_ids;
size_t no_devices = 0;
Device devices[1000];
long DP[1000][2][2];

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }

    device_ids = ht_create();

    if (!parse_file(argv[1])) {
        return 1;
    }

    memset(DP, -1, sizeof(DP));

    long part1 = dfs(get_device_id("you"), 1, 1);
    long part2 = dfs(get_device_id("svr"), 0, 0);

    printf("Part 1: %ld\n", part1);
    printf("Part 2: %ld\n", part2);

    return 0;
}

long dfs(int device_id, int dac, int fft) {
    if (DP[device_id][dac][fft] != -1) {
        return DP[device_id][dac][fft];
    }
    long result = 0;
    Device *d = &devices[device_id];
    if (strncmp(d->key, "out", 3) == 0 && dac && fft) {
        result = 1;
    } else {
        if (strncmp(d->key, "dac", 3) == 0) {
            dac = 1;
        }
        if (strncmp(d->key, "fft", 3) == 0) {
            fft = 1;
        }
        for (int i = 0; i < d->num_connections; i++) {
            int next_id = d->connections[i];
            result += dfs(next_id, dac, fft);
        }
    }
    DP[device_id][dac][fft] = result;
    return result;
}

int get_device_id(char *name) {
    int *value = ht_get(device_ids, name);
    if (value != NULL) {
        return *value;
    }
    int id = no_devices;
    Device *d = &devices[id];
    strncpy(d->key, name, 4);
    ht_set(device_ids, name, id);
    no_devices++;
    return id;
}

void parse_device(char *line) {
    char key[4];
    sscanf(line, "%3s", key);
    int device_id = get_device_id(key);
    Device *d = &devices[device_id];
    int offset = 5;
    while (line[offset] != '\0') {
        sscanf(line + offset, "%3s", key);
        offset += 4;
        int connected_id = get_device_id(key);
        d->connections[d->num_connections++] = connected_id;
    }
}

void parse_input(FILE *f) {
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        parse_device(line);
    }
    printf("Parsed %zu devices\n", no_devices);
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
