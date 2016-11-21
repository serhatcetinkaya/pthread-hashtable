#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define WAITING 0
#define BORED 1
#define ASKING 2
#define LEFT (student_no+1)%N
#define RIGHT (student_no+2)%N
#define N 100

int state[N];
pthread_cond_t cond;
pthread_mutex_t mutex[N];

void *student_func(void * num);
void *ta_func(void * ptr);


int main(int argc, char *argv[])
{
	int i;
	//N student threads 
	pthread_t students[N];
	int student_number[N];
	//Mutex initiation and state defining
	for(i=0;i<N;i++)
	{
		state[i]= WAITING;
		student_number[i]=i;
		pthread_mutex_init(&mutex[i], NULL);
	}

	pthread_cond_init(&cond, NULL);

	//TA thread
	pthread_t ta;
	
	
	//Create N student thread
	for(i=0; i<N;i++)
	{
		pthread_t tpr = students[i];
		pthread_create(&tpr, NULL, student_func, (void*)i);
		printf("Student %d is initially waiting\n",i+1);
	}
	
	//Create TA thread
	pthread_create(&ta, NULL, ta_func, (void *) &i);
	 

	return 0;
}
void *student_func(void * student_no){
	int std_no;
	std_no= (int) student_no;
	
	pthread_mutex_lock (&mutex[std_no]);

	printf("Student %d has lock and going into TA room\n",std_no );
	state[std_no]=BORED;
	test(std_no);

	while(state[std_no]!=ASKING)
	{
		pthread_cond_wait(&cond,&mutex[std_no]);
	}
	pthread_mutex_unlock(&mutex[std_no]);

}


void test(int student_no)
{
	if(state[student_no]==BORED && state[LEFT]!=ASKING&&state[RIGHT]!=ASKING)
	{
		state[student_no]=ASKING;
		sleep(2);
		printf("Student %d is asking\n",student_no);
		pthread_cond_signal(&cond);
	}
}
void *ta_func(void * ptr){

}
