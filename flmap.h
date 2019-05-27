#ifndef FLMAP_H
#define FLMAP_H

#include <stdlib.h>
#include <stdio.h>

enum {
    E, // empty
    L, // occupied: index==hash(key)
    S // rellocated: index!=hash(key)
};

typedef struct FLNode{
    unsigned char type; // how the node is used (E, L, S)
    int links[3]; // 0=prev/left, 1=next/right, 2=parent
    struct { // user data
        size_t key;
        void* value;
    } pair;
}FLNode; // 24 on x86, 32 on x64


typedef struct FLHashMap{
    FLNode* table; // hash table
    int free_list; // list of free nodes (link to first free node)
    int N; // number of items in table
    int T; // table size
    int R; // Hopscotch Region
}FLHashMap;


FLHashMap* FLHashMap_new();
void FLHashMap_destroy(FLHashMap* self);
void FLHashMap_destroyCB(FLHashMap* self, void (*callback)(size_t key, void* value));
void* FLHashMap_set(FLHashMap* self, size_t key, void* value);
void* FLHashMap_get(FLHashMap* self, size_t key);
void* FLHashMap_del(FLHashMap* self, size_t key);
void FLHashMap_forEach(FLHashMap* self, void (*callback)(size_t key, void* value));
void FLHashMap_print(FLHashMap* self);
void FLHashMap_printStats(FLHashMap* self);

#endif
