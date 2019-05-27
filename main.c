#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "flmap.h"

#define MAX_ITEMS 150000000

typedef struct{
	size_t key;
	void* value;
}Pair;

void randTest(){
	srand(93);
	
	Pair* items = (Pair*)malloc(sizeof(Pair)*MAX_ITEMS);
	int i;
	FLHashMap* map = FLHashMap_new();
	//FLHashMap_print(map);
	printf("Inserting\n");
	for (i=0; i<MAX_ITEMS; i++){
		items[i].key = (size_t)(i*10 + rand()%10);
		items[i].value = (void*)rand();
		//printf("%zu -> %d ...\n", items[i].key, (int)(items[i].key%map->T));
		FLHashMap_set(map, items[i].key, items[i].value);
		//FLHashMap_print(map);
	}

	//FLHashMap_PrintStats(map);
	
	/*
	printf("Deleting\n");
	for (i=0; i<MAX_ITEMS/2; i++){
		int index = rand()%MAX_ITEMS;
		while (items[index].value == NULL){
			index = rand()%MAX_ITEMS;
		}
		if (index < 0) index*=-1;
		if (index == 0) index = 1;
		void* val = FLHashMap_del(map, items[index].key);
		if (val != items[index].value || FLHashMap_Get(map, items[index].key) != NULL){
			printf("Deletion FAILURE: <%zu, %p> -> <%zu, %p>\n",
				items[index].key, items[index].value, items[index].key, val);
		}
		items[index].value = NULL;
	}
	*/
	printf("Searching\n");
	for (int x=0; x<10; x++){
		for (i=0; i<MAX_ITEMS; i++){
			if (items[i].value == NULL) continue;
			void* val = FLHashMap_get(map, items[i].key);
			if (val != items[i].value){
				printf("Lookup FAILURE: <%zu, %p> != <%zu, %p>\n",
					items[i].key, items[i].value, items[i].key, val);
			}
		}
	}
	
	FLHashMap_printStats(map);
	
	FLHashMap_destroy(map);
	free(items);
}

int main(int argc, char** argv){

    randTest();

    return 0;

}
