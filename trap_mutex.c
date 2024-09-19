#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

int thread_count;
pthread_mutex_t mutex;
double total_int , a , b , h;
int n , local_n;

void Get_input(double* a_p , double* b_p , int* n_p);
void *Thread_work(void* rank);
double Trap(double left_endpt , double right_endpt , int trap_count , double base_len);
double f(double x);

int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;

	pthread_mutex_init(&mutex , NULL);

	printf("Enter the number of threads : \n");
	scanf("%d" , &thread_count);
	thread_handles = malloc(thread_count*sizeof(pthread_t));

	Get_input(&a , &b , &n);

	local_n = n / thread_count;
	h = (b-a) / n;

	for(thread = 0; thread < thread_count; thread++) {
		pthread_create(&thread_handles[thread], NULL, Thread_work,
					   (void*) thread);
	}

	for(thread = 0; thread < thread_count; thread++) {
		pthread_join(thread_handles[thread], NULL);
	}

	printf("With n = %d trapezoids , our estimate is :\n" , n);
	printf("of the integral from %lf to %lf = %lf\n" , a , b , total_int);

	pthread_mutex_destroy(&mutex);

	free(thread_handles);

	return 0;
}


void Get_input(double* a_p , double* b_p , int* n_p){
	printf("Enter the lower bound of the interval: \n");
	scanf("%lf" , a_p);
	printf("Enter the upper bound of the interval: \n");
	scanf("%lf" , b_p);
	printf("Enter the number of trapezoids: \n");
	scanf("%d" , n_p);
}


void *Thread_work(void* rank) {
	long my_rank = (long) rank;
	double local_a , local_b ,local_int;

	local_a = a + my_rank * local_n * h;
	local_b = local_a + local_n*h;

	pthread_mutex_lock(&mutex);
	local_int = Trap(local_a , local_b , local_n , h);
	total_int  += local_int;
	pthread_mutex_unlock(&mutex);
   return NULL;
}

double Trap(double left_endpt , double right_endpt , int trap_count , double base_len){
	double estimate , x;
	int i;
	double f(double x);

	estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
	for(i=1 ; i<=trap_count ; i++){
		x = left_endpt + i*base_len;
		estimate += f(x);
	}
	estimate = estimate * base_len;
	return estimate;
}

double f(double x){
	double val;
	val = (x*x);
	return val;
}


