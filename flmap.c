#include "flmap.h"

#define memalloc malloc
#define memfree free

#define INITIAL_SIZE 16
#define GROW_SCALE 2
#define INITIAL_R 4 // range of probing
#define MAX_LF 0.7
#define NONE -1

// calculate table load factor
static inline double loadFactor(FLHashMap* self){
    return (double)self->N / (double)self->T;
}

// maps between key and slot in table
static inline int hash(size_t key, size_t T){
	return key & (T-1);
	//return ((key>>(sizeof(size_t)*4))^(key>>(sizeof(size_t)*2))^key) & (T-1);
}

// distance between a node's index and it's original hashed index
static inline int distIdx(size_t key, int idx, size_t T){
	int hidx = hash(key, T);
	int dist = hidx-idx;
	return (dist<0)? -1*dist : dist;
}

// private prototypes
static void createTable(FLHashMap* self, int size);
static void resizeTable(FLHashMap* self);
static void removeFreeNode(FLHashMap* self, int idx);
static int findFreeNode(FLHashMap* self, size_t key, void* value, int parent);
static int rellocateNode(FLHashMap* self, int idx);

// create (and intitialize) a table of given size
static void createTable(FLHashMap* self, int size){
    FLNode* table = (FLNode*)memalloc(sizeof(FLNode)*size);
    self->N = 0;
    self->T = size;
    self->table = table;
    self->free_list = 0;
    for (int i=0; i<size; i++){
        table[i].type = E;
        table[i].links[0] = i-1;
        table[i].links[1] = i+1;
        table[i].links[2] = NONE;
    }
    table[size-1].links[1] = NONE;
}

// grow the table
static void resizeTable(FLHashMap* self){
    int T = self->T;
    FLNode* table = self->table;
    createTable(self, T*GROW_SCALE);
    self->R++;
    for (int i=0; i<T; i++){
        FLNode* node = &table[i];
        if (node->type != E){
            FLHashMap_set(self, node->pair.key, node->pair.value);
        }
    }
    memfree(table);
}

// remove a node from free list
static void removeFreeNode(FLHashMap* self, int idx){
    // adjust coalesced chain
    FLNode* table = self->table;
    FLNode* node = &table[idx];
    int iprev = node->links[0];
    int inext = node->links[1];
    if (iprev != NONE){
        FLNode* prev = &table[iprev];
        prev->links[1] = inext;
    }
    if (inext != NONE){
        FLNode* next = &table[inext];
        next->links[0] = iprev;
    }
    if (idx==self->free_list){
        self->free_list = inext;
    }
}

// find a free node
// linear probing allows to find closeby nodes (+cache hits)
// hoptscotch region limits probing to log(T) iterations
// free list guarantees always finding a free node even if probing fails, and it's done in O(1)
static int findFreeNode(FLHashMap* self, size_t key, void* value, int parent){
	FLNode* table = self->table;
	int inode = NONE;
	FLNode* found = NULL;
	for (int i=0; i<self->R; i++){ // at most log(T) probes
		if (parent+i>=self->T) break;
		int tmpi = parent+i;
		FLNode* tmp = &table[tmpi];
		if (tmp->type == E){
			inode = tmpi;
			removeFreeNode(self, inode);
			found = tmp;
			break;
		}
	}
    if (inode == NONE){ // probing failed, use free list
    	inode = self->free_list;
    	found = &table[inode];
    	// adjust free list
		self->free_list = found->links[1];
		if (self->free_list != NONE){
		    table[self->free_list].links[0] = NONE;
		}
    }
	found->links[0] = NONE;
	found->links[1] = NONE;
	found->links[2] = parent;
	found->pair.key = key;
	found->pair.value = value;
	found->type = S;
	self->N++;
    return inode;
}

// rellocate a given node to a new position, updating any and all links
static int rellocateNode(FLHashMap* self, int idx){
    FLNode* table = self->table;
    FLNode* node = &table[idx];
    int inode = findFreeNode(self, node->pair.key, node->pair.value, node->links[2]);
    FLNode* used = &table[inode];
    // relloc data
    used->links[0] = node->links[0];
    used->links[1] = node->links[1];
    if (used->links[0] != NONE) table[used->links[0]].links[2] = inode;
    if (used->links[1] != NONE) table[used->links[1]].links[2] = inode;
    FLNode* parent = &table[used->links[2]];
    if (parent->links[1]==idx) parent->links[1] = inode;
    else parent->links[0] = inode;
    return inode;
}

// set a value to a given key
void* FLHashMap_set(FLHashMap* self, size_t key, void* value){

    if (value == NULL) return NULL;
    
    if (loadFactor(self) >= MAX_LF){
        resizeTable(self);
    }
    
    // calculate hash and operate on the resulting node
    FLNode* table = self->table;
    int idx = hash(key, self->T);
    FLNode* node = &table[idx];
    void* ret = NULL;

    switch (node->type){
        case E: // empty node, insert data here
            {
            removeFreeNode(self, idx); // remove node from free list
            // store data and update type
            node->pair.key = key;
            node->pair.value = value;
            node->type = L;
            node->links[0] = NONE;
            node->links[1] = NONE;
            self->N++;
            }
            break;
        case L: // occupied, search for the key and if not found add it
            {
            FLNode* cur = node;
            int parent = idx;
            do{ // we do a binary search, where links[0]=left and links[1]=right
            	  if (cur->pair.key == key){
            	      ret = cur->pair.value;
                      cur->pair.value = value;
                      cur = NULL;
            	  }
            	  else{
            	      int ilink = (cur->pair.key < key)&1; // calculate which link to take
            	      if (cur->links[ilink] == NONE){
            	          cur->links[ilink] = findFreeNode(self, key, value, parent);
            	          cur = NULL;
            	      }
            	      else{
            	          parent = cur->links[ilink];
            	          cur = &table[parent];
            	      }
            	  }
            }while (cur);
            }
            break;
        case S: // found item rellocated here, must push it (Cuckoo rellocate)
            {
            int inode = rellocateNode(self, idx);
            // change hashed node
            node->pair.key = key;
            node->pair.value = value;
            node->links[0] = NONE;
            node->links[1] = NONE;
            node->links[2] = NONE;
            node->type = L;
            }
            break;
    }
    return ret;
}

// obtain the value of a given key
void* FLHashMap_get(FLHashMap* self, size_t key){
	FLNode* table = self->table;
    int idx = hash(key, self->T);
    FLNode* node = &table[idx];
    if (node->type == L){ // only L types are considered, any other type = not found
        do{ // do a binary search
            if (node->pair.key == key){
                return node->pair.value;
            }
            else{
                int ilink = (node->pair.key < key)&1;
                idx = node->links[ilink];
            }
            node = &table[idx];
        }while (idx!=NONE);
    }
    return NULL;
}

// delete a key-value pair from the hashmap
void* FLHashMap_del(FLHashMap* self, size_t key){
	return NULL;
	FLNode* table = self->table;
    int idx = hash(key, self->T);
    FLNode* node = &table[idx];
    FLNode* prev = NULL;
    void* ret = NULL;
    int idel = NONE;
    if (node->type == L){ // only consider L types
        do{ // binary search
            if (node->pair.key == key){ // binary tree delete
                // TODO
                break;
            }
            else{
                int ilink = (node->pair.key < key)&1;
                idx = node->links[ilink];
            }
            node = &table[idx];
        }while (idx!=NONE);
    }
    if (ret!=NULL && idel!=NONE){
    	self->N--;
    	FLNode* dispose = &table[idel];
    	dispose->type = E;
    	dispose->links[1] = self->free_list;
    	self->free_list = dispose;
    }
    return ret;
}

// create new hashmap instance
FLHashMap* FLHashMap_new(){
    FLHashMap* self = (FLHashMap*)memalloc(sizeof(FLHashMap));
    createTable(self, INITIAL_SIZE);
    self->R = INITIAL_R;
    return self;
}

// destroy hashmap instance
void FLHashMap_destroy(FLHashMap* self){
    FLHashMap_destroyCB(self, NULL);
}

// destroy hashmap instance (with callback function for each element before deletion)
void FLHashMap_destroyCB(FLHashMap* self, void (*callback)(size_t key, void* value)){
	FLHashMap_forEach(self, callback);
    memfree(self->table);
    memfree(self);
}

// execute callback function on each element
void FLHashMap_forEach(FLHashMap* self, void (*callback)(size_t key, void* value)){
    if (callback==NULL) return;
    FLNode* table = self->table;
    for (int i=0; i<self->T; i++){
        if (table[i].type != E)
            callback(table[i].pair.key, table[i].pair.value);
    }
}

// print the map
void FLHashMap_print(FLHashMap* self){
    int NE=0, NL=0, NS=0;
    FLNode* table = self->table;
    printf("FL: %d[\n", self->free_list);
    for (int i=0; i<self->T; i++){
        FLNode* node = &table[i];
        printf("    %d: ", i);
        switch (node->type){
            case E: printf("E "); NE++; break;
            case L: printf("L(%zu) ", node->pair.key); NL++; break;
            case S: printf("S(%zu) ", node->pair.key); NS++; break;
            //default: printf("%c(%zu) ", (node->type==L)?'L':'S', node->pair.key); break;
        }
        printf(" <%d, %d> \n", node->links[0], node->links[1]);
    }
    printf("\n]\n");
    printf("NE: %d\n", NE);
    printf("NL: %d\n", NL);
    printf("NS: %d\n", NS);
    printf("LF: %f\n", loadFactor(self));
    printf("\n");
}

// print statistics about the map
void FLHashMap_printStats(FLHashMap* self){
	printf("sizeof(Node): %d\n", sizeof(FLNode));
    printf("T: %d\n", self->T);
    printf("N: %d\n", self->N);
    printf("R: %d\n", self->R);
    printf("E: %d\n", self->T-self->N);
    printf("LF: %f\n", loadFactor(self));
}
