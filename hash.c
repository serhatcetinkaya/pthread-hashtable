
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "hash.h"
#define MAX_K	(MAX_N / MIN_M)
#define MAXMAX 500000

pthread_mutex_t mutex[MAX_K];
int ret = -1;

HashTable *hash_init (int N, int M){
	printf ("hash_init called\n");
	// contol for validity of N & M values.
	if ((N % M) != 0){
		printf("Not valid parameters for hash_init() !!\n");
		return NULL;
	}
	// declaring local variables for ease of use.
	int size = N, lock_num = (N/M), region_size = M, i;
	// initializing HashTable
	HashTable *hp = NULL;
	hp = (HashTable *) calloc(N, sizeof(HashTable));

	for (i = 0; i < N; i++){
		hp[i].head = NULL;
		hp[i].lock_number = (i / region_size);
		//printf("lock_number= %d\n", hp[i].lock_number);
		hp[i].total_bucket = N;
	}
	// initializing mutex locks.
	for (i = 0; i < lock_num; i++){
		printf("initializing mutex %d\n", i);
		pthread_mutex_init(&mutex[i], NULL);
	}

	return hp;
}

int hash_insert (HashTable *hp, int k, int v){
	printf ("hash_insert called, k= %d\tv= %d\n",k,v);
	int size = hp[0].total_bucket;
	int index = k % size;
	printf("index= %d\n", index);
	int lock_num = hp[index].lock_number;
	// mutex lock for prevention.
	//--------------------------
	pthread_mutex_lock(&mutex[lock_num]);
	//--------------------------
	if (k < 0){
		printf("invalid key value !\n");
		pthread_mutex_unlock(&mutex[lock_num]);

		return ret;
	}

	NODE *controlNode;
	controlNode = (NODE *) malloc(sizeof(NODE));
	controlNode = hp[index].head;

	// initializing new node.
	NODE *newNode;
	newNode = (NODE *) malloc(sizeof(NODE));
	newNode->key = k;
	newNode->data = v;
	newNode->next = NULL;
	// check if there is a collision for insertion and insert the new node.
	if (hp[index].head == NULL){
		//-----printf("inserting node with key:%d\tvalue:%d\n", newNode->key, newNode->data);
		//printf("added\n");
		hp[index].head = newNode;
	}else{
		// check if specified key value is already inserted or not. If inserted abort.
		while (controlNode){
			if (/*controlNode->data == v && */controlNode->key == k){
				//-----printf("specified key value pair already exist !\n");
				//printf("oooooo\n");
				pthread_mutex_unlock(&mutex[lock_num]);
				return ret;
			}
			controlNode = controlNode->next;
		}
		//-----printf("inserting second node with key:%d\tvalue:%d\n", newNode->key, newNode->data);
		//printf("added\n");
		newNode->next = hp[index].head;
		hp[index].head = newNode;
	}
	// mutex unlock.
	//--------------------------
	pthread_mutex_unlock(&mutex[lock_num]);
	//--------------------------
	return 0;
}

int hash_delete (HashTable *hp, int k) {
	int x = 0;
	int size = hp[0].total_bucket;
	int index = k % size;
	int lock_num = hp[index].lock_number;
	while (x < MAXMAX) x++;
	printf ("hash_delete called, index= %d k = %d\n",index,k);
	// mutex lock for prevention.
	pthread_mutex_lock(&mutex[lock_num]);
	//--------------------------
	/*if (k < 0){
		pthread_mutex_unlock(&mutex[lock_num]);
		return ret;
	}*/

	int deleted;

	NODE *prevNode, *currentNode;
	currentNode = hp[index].head;
	prevNode = currentNode;
	//printf("javid\n");
	if (hp[index].head == NULL){
		//-----printf("given key is not present in hash table !\n");
		//printf("serhat\n");
		pthread_mutex_unlock(&mutex[lock_num]);
		return ret;
	}

	while (currentNode != NULL){
		if (currentNode->key == k){

			if (currentNode == hp[index].head){
				hp[index].head = currentNode->next;
				currentNode->next = NULL;
			}else{
				prevNode->next = currentNode->next;
				currentNode->next = NULL;
			}
			//free(currentNode);
			deleted = 1;
		}
		prevNode = currentNode;
		currentNode = currentNode->next;
	}
	if (deleted){
		printf("deleted k=%d\n",k);
	}
	// mutex unlock.
	//--------------------------
	pthread_mutex_unlock(&mutex[lock_num]);
	//--------------------------
	return 0;
}


int hash_get (HashTable *hp, int k, int *vptr){
	//-----printf ("hash_get called\n");
	int size = hp[0].total_bucket;
	int index = k % size;
	int lock_num = hp[index].lock_number;
	// mutex lock.
	//--------------------------
	pthread_mutex_lock(&mutex[lock_num]);
	//--------------------------
	if (k < 0){
		//-----printf("invalid key value !\n");
		pthread_mutex_unlock(&mutex[lock_num]);
		return ret;
	}

	int found = 0, *ptr;

	NODE *currentNode;
	currentNode = hp[index].head;


	if (hp[index].head == NULL){
		//------printf("node for given key is not present in the hash table !\n");
		pthread_mutex_unlock(&mutex[lock_num]);
		return ret;
	}
	while (currentNode != NULL){
		if (currentNode->key == k){
			found = 1;
			//------------------
			*vptr = currentNode->data;
			//------------------

			//printf("*vptr:%d\tk=%d\n", (int)(*vptr),k);
			break;
		}
		currentNode = currentNode->next;
	}
	// mutex unlock.
	//--------------------------
	pthread_mutex_unlock(&mutex[lock_num]);
	//--------------------------

	if (found){
		printf("node for given key value is found !\n");
		printf("*index=%d\tvptr:%d\tk=%d\n", index,(int)(*vptr),k);
	}else{
		printf("node for given key value is not found !\n");
		printf("*vptr:?\tk=%d\n",k);
		return ret;
	}

	return 0;
}

int hash_destroy (HashTable *hp){
	//-----printf ("hash_destroy called\n");
	// initializing size.
	int size = hp[0].total_bucket;
	int lock_num = hp[0].lock_number;
	int i;
	for (i = 0; i < lock_num; i++){
		pthread_mutex_lock(&mutex[i]);
	}
	NODE *freeingNode;

	// iterate if there is a node unfreed. If there is free it.
	for (i = 0; i < size; i++){
		while (hp[i].head != NULL){
			freeingNode = hp[i].head;
			hp[i].head = hp[i].head->next;
			//-----printf("freeing node\n");
			free(freeingNode);
		}
	}
	// free the hash table itself.
	//-----printf("freeing hash table\n");
	free(hp);
	for (i = 0; i < lock_num; i++){
		pthread_mutex_unlock(&mutex[i]);
	}
	return 0;
}
