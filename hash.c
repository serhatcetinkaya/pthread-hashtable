/*
Copyright (C) 2016  Serhat Çetinkaya serhatcetinkayaa@gmail.com
		    Çağrıbey Güvendik cagribeyg@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "hash.h"
#define MAX_K	(MAX_N / MIN_M)
#define MAXMAX 500000

pthread_mutex_t mutex[MAX_K];
int ret = -1;

HashTable *hash_init (int N, int M){
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
		hp[i].total_bucket = N;
	}
	// initializing mutex locks.
	for (i = 0; i < lock_num; i++){
		pthread_mutex_init(&mutex[i], NULL);
	}

	return hp;
}

int hash_insert (HashTable *hp, int k, int v){
	int size = hp[0].total_bucket;
	int index = k % size;
	int lock_num = hp[index].lock_number;
	// mutex lock for prevention.
	//--------------------------
	pthread_mutex_lock(&mutex[lock_num]);
	//--------------------------
	if (k < 0){
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
		hp[index].head = newNode;
	}else{
		// check if specified key value is already inserted or not. If inserted abort.
		while (controlNode){
			if (controlNode->key == k){
				// if key already exists.
				pthread_mutex_unlock(&mutex[lock_num]);
				return ret;
			}
			controlNode = controlNode->next;
		}
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
	// mutex lock for prevention.
	pthread_mutex_lock(&mutex[lock_num]);
	//--------------------------

	int deleted;

	NODE *prevNode, *currentNode;
	currentNode = hp[index].head;
	prevNode = currentNode;
	if (hp[index].head == NULL){
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
			deleted = 1;
		}
		prevNode = currentNode;
		currentNode = currentNode->next;
	}
	// mutex unlock.
	//--------------------------
	pthread_mutex_unlock(&mutex[lock_num]);
	//--------------------------
	return 0;
}


int hash_get (HashTable *hp, int k, int *vptr){
	int size = hp[0].total_bucket;
	int index = k % size;
	int lock_num = hp[index].lock_number;
	// mutex lock.
	//--------------------------
	pthread_mutex_lock(&mutex[lock_num]);
	//--------------------------
	if (k < 0){
		pthread_mutex_unlock(&mutex[lock_num]);
		return ret;
	}

	int found = 0, *ptr;

	NODE *currentNode;
	currentNode = hp[index].head;


	if (hp[index].head == NULL){
		// bucket is empty.
		pthread_mutex_unlock(&mutex[lock_num]);
		return ret;
	}
	while (currentNode != NULL){
		if (currentNode->key == k){
			found = 1;
			*vptr = currentNode->data;
			break;
		}
		currentNode = currentNode->next;
	}
	// mutex unlock.
	//--------------------------
	pthread_mutex_unlock(&mutex[lock_num]);
	//--------------------------

	if (found){
		// value is found.
	}else{
		return ret;
	}
	return 0;
}

int hash_destroy (HashTable *hp){
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
			free(freeingNode);
		}
	}
	// free the hash table itself.
	free(hp);
	for (i = 0; i < lock_num; i++){
		pthread_mutex_unlock(&mutex[i]);
	}
	return 0;
}
