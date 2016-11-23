#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define N 10
/// TA states
#define SLEEPING 0
#define HELPING 1
#define CHECKING 2

/// student states
#define WITHTA 0
#define SITANDWAIT 1
#define PROGRAMMING 2

// global variables
int ta = SLEEPING;
int chairs[3] = {-1,-1,-1};
int chair0 = -1, chair1 = -1, chair2 = -1;
int students[N] = {2,2,2,2,2,2,2,2,2,2};
pthread_mutex_t mutex[N];
pthread_mutex_t ta_mutex;
pthread_mutex_t run;
pthread_t students_thread[N], ta_thread;

void student_state(int stdno);
void ta_state();
void *ta_func(void *args);
void *student_func(void *args);

int main (int argc, char *argv[]){
  int i;

  for (i = 0; i < N; i++){
		printf("initializing mutex %d\n", i);
		pthread_mutex_init(&mutex[i], NULL);
	}
  pthread_mutex_init(&ta_mutex, NULL);
  pthread_mutex_init(&run, NULL);
  pthread_create(&ta_thread, NULL, ta_func, NULL);

  for(i = 0; i < N; i++){
		pthread_t tpr = students_thread[i];
		pthread_create(&tpr, NULL, student_func, &i);
		printf("Student %d is initially programming\n",i);
	}

  for (i = 0; i < N; i++){
		pthread_t tpr2 = students_thread[i];
		pthread_join(tpr2, NULL);
	}

  pthread_join(ta_thread, NULL);
  return 0;
}

void student_state(int stdno){
  int i = stdno;
  pthread_mutex_lock(&mutex[i]);
  switch (students[i]) {
    case WITHTA:
      printf("student %d is with ta\n", i);
      //ta_state();
      break;
    case SITANDWAIT:
      //ta_state();
      break;
    case PROGRAMMING:
      if (ta == SLEEPING){
        pthread_mutex_lock(&ta_mutex);
        printf("student %d awakens ta\n", i);
        sleep(1);
        //pthread_mutex_lock(&ta_mutex);
        ta = HELPING;
        //pthread_mutex_unlock(&ta_mutex);
        students[i] = WITHTA;
        printf("ta is helping student %d\n", i);
        //pthread_mutex_lock(&ta_mutex);
        ta = CHECKING;
        //ta_state();
        //pthread_mutex_unlock(&ta_mutex);
        students[i] = PROGRAMMING;
        pthread_mutex_unlock(&ta_mutex);
        break;
      }
      if (ta == HELPING){
        pthread_mutex_lock(&ta_mutex);
        if (chair0 == -1){
          chair0 == stdno;
          sleep(1);
          students[i] == SITANDWAIT;
          printf("student %d sits and waits\n", i);
          //ta_state();
          pthread_mutex_unlock(&ta_mutex);
          break;
        }
        else if (chair1 == -1){
          chair1 == stdno;
          sleep(1);
          students[i] == SITANDWAIT;
          printf("student %d sits and waits\n", i);
          //ta_state();
          pthread_mutex_unlock(&ta_mutex);
          break;
        }
        else if (chair2 == -1){
          chair2 == stdno;
          sleep(1);
          students[i] == SITANDWAIT;
          printf("student %d sits and waits\n", i);
          //ta_state();
          pthread_mutex_unlock(&ta_mutex);
          break;
        }
      }
      pthread_mutex_unlock(&ta_mutex);
      break;
  }
  pthread_mutex_unlock(&mutex[i]);
}

void ta_state(){
  int i;
  switch (ta) {
    case HELPING:
      for (i = 0; i < N; i++){
        if (students[i] == WITHTA){
          break;
        }
      }
      sleep(1);
      students[i] = PROGRAMMING;
      ta = CHECKING;
      break;
    case SLEEPING:
      break;
    case CHECKING:
      printf("ta is checking chairs\n");
      if (chair0 != -1){
        sleep(1);
        ta = HELPING;
        printf("ta is helping student %d\n", chair0);
        pthread_mutex_lock(&mutex[chair0]);
        sleep(1);
        students[chair0] == WITHTA;
        pthread_mutex_unlock(&mutex[chair0]);
        chair0 = -1;
        break;
      }
      printf("chair 0 is empty\n");
      if (chair1 != -1){
        sleep(1);
        ta = HELPING;
        printf("ta is helping student %d\n", chair1);
        pthread_mutex_lock(&mutex[chair1]);
        sleep(1);
        students[chair1] == WITHTA;
        pthread_mutex_unlock(&mutex[chair1]);
        chair1 = -1;
        break;
      }
      printf("chair 1 is empty\n");
      if (chair2 != -1){
        sleep(1);
        ta = HELPING;
        printf("ta is helping student %d\n", chair2);
        pthread_mutex_lock(&mutex[chair2]);
        sleep(1);
        students[chair2] == WITHTA;
        pthread_mutex_unlock(&mutex[chair2]);
        chair2 = -1;
        break;
      }
      sleep(1);
      ta = SLEEPING;
      printf("chairs are empty ta will sleep\n");
      break;
    }
}
void *student_func(void *args){

  int *ptr = (int *) args;
  int i = *ptr;
  while(1){
    student_state(i);
  }
}

void *ta_func(void *args){
  while(1){
    pthread_mutex_lock(&ta_mutex);
    ta_state();
    pthread_mutex_unlock(&ta_mutex);
  }
}
