#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

const int MAX_STRING = 99;

int thread_count;
int msg = 0;
int receiver = 0;
char* message;
pthread_mutex_t mutex;

void Get_args(int argc, char* argv[]);


/* Thread function */
void* Thread_work(void* rank);

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;
	message = malloc(MAX_STRING*sizeof(char));

	Get_args(argc , argv);

	pthread_mutex_init(&mutex , NULL);

	/* allocate array for threads */
	thread_handles = malloc(thread_count*sizeof(pthread_t));

	/* start threads */
	for(thread = 0; thread < thread_count; thread++) {
		pthread_create(&thread_handles[thread], NULL, Thread_work,
					   (void*) thread);
	}

	/* wait for threads to complete */
	for(thread = 0; thread < thread_count; thread++) {
		pthread_join(thread_handles[thread], NULL);
	}


	free(message);
	free(thread_handles);
	pthread_mutex_destroy(&mutex);

	return 0;
}


void Get_args(int argc , char* argv[]) {
   thread_count = strtol(argv[1], NULL, 10);
   if (thread_count <= 0) exit(0);
}  /* Get_args */

/*-------------------------------------------------------------------
 * Function:    Thread_work
 * Purpose:     Producer rank 1: create msg
 *		Consumer rank 0: print out msg
 * In arg:      rank
 * Global var:  mutex, msg, message
 */

void *Thread_work(void* rank) {
	long my_rank = (long) rank;
	int send =0 , recv =0;
	while(1){
		if(receiver==my_rank){
			pthread_mutex_lock(&mutex);

			if(recv==0){
				if(msg){
					printf("\n Th %ld > %s" , my_rank , message);
					msg = 0;
					recv = 1;
				}
				else{
					pthread_mutex_unlock(&mutex);
				}
			}

			if(send==0 && msg == 0){
				sprintf(message , "Received: Hello from %ld" , my_rank);
				msg = 1;
				receiver = (my_rank+1) % thread_count;
				send = 1;
			}

			if(send==1 && recv==1){
				pthread_mutex_unlock(&mutex);
				break;
			}
		}
	}
	return NULL;
}
