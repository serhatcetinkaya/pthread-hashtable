#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "hash.h"

HashTable *ht1; // space allocated inside library

int main()
{
	int i; 
	
	ht1  = hash_init (1000, 100);

	for (i = 0; i < 10; ++i) {
		hash_insert (ht1, i, 35000); 
	}

	for (i = 0; i < 10; ++i) {
		hash_delete (ht1, i); 
	}
	hash_destroy (ht1); 
}


