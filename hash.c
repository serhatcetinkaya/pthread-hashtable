
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "hash.h"
#define MAX_K	(MAX_N / MIN_M)

pthread_mutex_t mutex[MAX_K];

HashTable *hash_init (int N, int M){
	printf ("hash_init called\n");
	// contol for validity of N & M values.
	if ((N % M) != 0){
		printf("Not valid parameters for hash_init() !!\n");
		return NULL;
	}
	// declaring local variables for ease of use.
	int size = N, lock_num = M, region_size = (N/M), i;
	// initializing HashTable
	HashTable *hp = NULL;
	hp = (HashTable *) calloc(N, sizeof(HashTable));

	for (i = 0; i < N; i++){
		hp[i].head = NULL;
		hp[i].lock_number = (i / region_size);
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
	printf ("hash_insert called\n");
	if (k < 1){
		printf("invalid key value !\n");
		return -1;
	}
	int size = hp[0].total_bucket;
	int index = k % size;
	int lock_num = hp[index].lock_number;
	NODE *controlNode;
	controlNode = (NODE *) malloc(sizeof(NODE));
	controlNode = hp[index].head;
	// mutex lock for prevention.
	//--------------------------
	pthread_mutex_lock(&mutex[lock_num]);
	//--------------------------
	// initializing new node.
	NODE *newNode;
	newNode = (NODE *) malloc(sizeof(NODE));
	newNode->key = k;
	newNode->data = v;
	newNode->next = NULL;
	// check if there is a collision for insertion and insert the new node.
	if (hp[index].head == NULL){
		printf("inserting node with key:%d\tvalue:%d\n", newNode->key, newNode->data);
		hp[index].head = newNode;
	}else{
		// check if specified key value is already inserted or not. If inserted abort.
		while (controlNode != NULL){
			if (controlNode->data == v && controlNode->key == k){
				printf("specified key value pair already exist !\n");
				return -1;
			}
			controlNode = controlNode->next;
		}
		printf("inserting second node with key:%d\tvalue:%d\n", newNode->key, newNode->data);
		newNode->next = (hp[index].head);
		hp[index].head = newNode;
	}
	// mutex unlock.
	//--------------------------
	pthread_mutex_unlock(&mutex[lock_num]);
	//--------------------------
	return 0;
}

int hash_delete (HashTable *hp, int k) {

	printf ("hash_delete called\n");
	if (k < 1){
		printf("invalid key value !\n");
		return -1;
	}

	int deleted;
	int size = hp[0].total_bucket;
	int index = k % size;
	int lock_num = hp[index].lock_number;
	// mutex lock for prevention.
	//--------------------------
	pthread_mutex_lock(&mutex[lock_num]);
	//--------------------------
	NODE *prevNode, *currentNode;
	currentNode = hp[index].head;
	prevNode = currentNode;
	if (currentNode == NULL){
		printf("given key is not present in hash table !\n");
		return -1;
	}

	while (currentNode != NULL){
		if (currentNode->key == k){
			deleted = 1;
			if (currentNode == hp[index].head){
				hp[index].head = currentNode->next;
			}else{
				prevNode->next = currentNode->next;
			}
			free(currentNode);
			break;
		}
		prevNode = currentNode;
		currentNode = currentNode->next;
	}
	// mutex unlock.
	//--------------------------
	pthread_mutex_unlock(&mutex[lock_num]);
	//--------------------------
	if (deleted){
		printf("node with given key is deleted succesfully !\n");
	}else{
		printf("node with given key is not present in hash table !\n");
	}

	return 0;
}


int hash_get (HashTable *hp, int k, int *vptr){
	printf ("hash_get called\n");
	if (k < 1){
		printf("invalid key value !\n");
		return -1;
	}

	int found = 0;
	int size = hp[0].total_bucket;
	int index = k % size;
	int lock_num = hp[index].lock_number;

	NODE *currentNode;
	currentNode = hp[index].head;
	// mutex lock.
	//--------------------------
	pthread_mutex_lock(&mutex[lock_num]);
	//--------------------------

	if (hp[index].head == NULL){
		printf("node for given key is not present in the hash table !\n");
		return -1;
	}
	while (currentNode != NULL){
		if (currentNode->key == k){
			found = 1;
			vptr = &(currentNode->data);
			printf("*vptr:%d\n", (int)(*vptr));
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
	}else{
		printf("node for given key value is not found !\n");
	}

	return 0;
}

int hash_destroy (HashTable *hp){
	printf ("hash_destroy called\n");
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
			printf("freeing node\n");
			free(freeingNode);
		}
	}
	// free the hash table itself.
	printf("freeing hash table\n");
	free(hp);
	for (i = 0; i < lock_num; i++){
		pthread_mutex_unlock(&mutex[i]);
	}
	return 0;
}
