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
	int i, *vptr;
	// hash_init(i,j); when j becomes 100 weird segfault occurs !!!
	ht1 = hash_init (1000, 100);

	for (i = 0; i < 10; ++i) {
		hash_insert (ht1, i, 35000);
	}
	for (i = 0; i < 10; ++i) {
		hash_get (ht1, i, vptr);
	}
	for (i = 0; i < 10; ++i) {
		hash_delete (ht1, i);
	}
	hash_destroy (ht1);
}
