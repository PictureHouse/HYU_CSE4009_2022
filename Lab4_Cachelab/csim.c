//Cho Yune, 2019030400

#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

int hit_count = 0;
int miss_count = 0;
int eviction_count = 0;
int S, E, B;
int m = 64;
int lru = 0;
unsigned long long s, b, tag;

typedef struct {
	unsigned long long tagLine;
	unsigned long long set;
	int lastUsed;
} line;

void makeCache(line cache[S][E]);
int LRU(line cache[S][E], unsigned long long set);
void simulation(line cache[S][E], unsigned long long address);

int main(int argc, char **argv) {
        FILE *file;
        unsigned long long address;
        int size;
        char *traceFile;
        char input;
        while ((input = getopt(argc, argv, "s:E:b:t:")) != -1) {
                switch(input) {
                        case 's':
                                s = atoi(optarg);
                                break;
                        case 'E':
                                E = atoi(optarg);
                                break;
                        case 'b':
                                b = atoi(optarg);
                                break;
                        case 't':
                                traceFile = optarg;
                                break;
                        default:
                                exit(1);
                }
        }
        S = pow(2, s);
        B = pow(2, b);
        tag = m - (s + b);
        line cache[S][E];
        makeCache(cache);
        file  = fopen(traceFile, "r");
        if (file != NULL) {
                while (fscanf(file, " %c %llx,%d", &input, &address, &size) == 3) {
                        switch(input) {
                                case 'I':
                                        break;
                                case 'L':
                                        simulation(cache, address);
                                        break;
                                case 'S':
                                        simulation(cache, address);
                                        break;
                                case 'M':
                                        simulation(cache, address);
                                        simulation(cache, address);
                                        break;
                                default:
                                        break;
                        }
                }
        }
        printSummary(hit_count, miss_count, eviction_count);
        return 0;
}


void makeCache(line cache[S][E]) {
	line currentLine;
	currentLine.tagLine = -1;
	currentLine.lastUsed = 0;
	for (int i = 0; i < S; i++) {
		for (int j = 0; j < E; j++) {
			cache[i][j] = currentLine;
		}
    	}
}

int LRU(line cache[S][E], unsigned long long set) {
	int index = 0;
	int least = cache[set][0].lastUsed;
	for (int i = 0; i < E; i++) {
		if (cache[set][i].lastUsed <= least) {
			index = i;
			least = cache[set][i].lastUsed;
		}
	}
	return index;
}

void simulation(line cache[S][E], unsigned long long address) {
	int hit = 0;
	int emptyFlag = 0;
	int emptyLine = 0;
	line currentLine;
	currentLine.tagLine = address >> (s + b);
	currentLine.set = address << tag;
	currentLine.set = currentLine.set >> (tag + b);
	for (int i = 0; i < E; i++) {
		if (cache[currentLine.set][i].tagLine == -1) {
			emptyFlag = 1;
			emptyLine = i;
		}
		if (cache[currentLine.set][i].tagLine == currentLine.tagLine) {
			hit = 1;
			hit_count++;
			lru++;
			cache[currentLine.set][i].lastUsed = lru;
		}
	}
	if (hit == 0 && emptyFlag == 1) {
		miss_count++;
		cache[currentLine.set][emptyLine].tagLine = currentLine.tagLine;
		cache[currentLine.set][emptyLine].lastUsed = lru;
	}
	if (hit == 0 && emptyFlag == 0) {
		miss_count++;
		eviction_count++;
		cache[currentLine.set][LRU(cache, currentLine.set)].tagLine = currentLine.tagLine;
		cache[currentLine.set][LRU(cache, currentLine.set)].lastUsed = lru;
	}
}
