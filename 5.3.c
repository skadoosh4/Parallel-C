#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<pthread.h>

int thread_count;
pthread_mutex_t mutex;
long long total_tosses;
long long total_circle;

void* Thread_work(void* rank);
double randfrom(double min , double max);

int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;
	double pi_estimate;

	pthread_mutex_init(&mutex , NULL);

	printf("Enter the number of threads : \n");
	scanf("%d" , &thread_count);
	thread_handles = malloc(thread_count*sizeof(pthread_t));

	printf("Enter the total number of tosses \n");
	scanf("%lld" , &total_tosses);

	for(thread = 0; thread < thread_count; thread++) {
		pthread_create(&thread_handles[thread], NULL, Thread_work,
					   (void*) thread);
	}

	for(thread = 0; thread < thread_count; thread++) {
		pthread_join(thread_handles[thread], NULL);
	}


	pi_estimate = (4*total_circle) / (double) total_tosses;
	printf("Pi is approximately :-  %f" , pi_estimate);

	pthread_mutex_destroy(&mutex);

	free(thread_handles);

	return 0;
}


void *Thread_work(void* rank) {
	srand(time(NULL));
   double toss_per_thread;
   toss_per_thread = total_tosses / thread_count;

	long toss , number_in_circle = 0;
	double x , y;
	for(toss=0 ; toss<toss_per_thread ; toss++){
		x = randfrom(-1.0,1.0);
		y = randfrom(-1.0,1.0);
		if((x*x + y*y) <= 1.0){
			number_in_circle++;
		}
	}

	pthread_mutex_lock(&mutex);
	total_circle += number_in_circle;
	pthread_mutex_unlock(&mutex);
   return NULL;
}


double randfrom(double min , double max){
	double range = (max-min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
}

