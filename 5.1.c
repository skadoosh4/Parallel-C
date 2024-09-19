#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

int thread_count;
pthread_mutex_t mutex;
double* A;
double* X;
double* Y;
int m=0 , n=0;

void read_matrix(char* prompt, double A[], int m, int n);
void print_matrix(char* title, double A[], int m, int n);
void read_vector(char* prompt, double x[], int n);
void print_vector(char* title, double y[], int m);

void* Thread_work(void* rank);

int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;

	pthread_mutex_init(&mutex , NULL);

	printf("Enter the number of threads : \n");
	scanf("%d" , &thread_count);
	thread_handles = malloc(thread_count*sizeof(pthread_t));

	printf("Enter the number of rows\n");
	scanf("%d", &m);
	printf("Enter the number of columns\n");
	scanf("%d", &n);

	A = malloc(m*n*sizeof(double));
	X = malloc(n*sizeof(double));
	Y = malloc(m*sizeof(double));

	read_matrix("Enter the matrix", A, m, n);
	print_matrix("Input matrix", A, m, n);

	read_vector("Enter the vector", X, n);
	print_vector("Input vector", X, n);


	for(thread = 0; thread < thread_count; thread++) {
		pthread_create(&thread_handles[thread], NULL, Thread_work,
					   (void*) thread);
	}

	for(thread = 0; thread < thread_count; thread++) {
		pthread_join(thread_handles[thread], NULL);
	}

	print_vector("Result : ", Y, m);


	pthread_mutex_destroy(&mutex);

	free(A);
	free(X);
	free(Y);
	free(thread_handles);

	return 0;
}

void read_matrix(char* prompt, double A[], int m, int n) {
   int             i, j;

   printf("%s\n", prompt);
   for (i = 0; i < m; i++){
	   for(j=0 ; j<n ; j++){
		   scanf("%lf" , &A[i*n+j]);
	   }
   }
}

void read_vector(char* prompt, double x[], int n) {
   int   i;

   printf("%s\n", prompt);
   for (i = 0; i < n; i++){
	   scanf("%lf" , &x[i]);
   }
}

void print_matrix(char* title, double A[], int m, int n) {
   int   i, j;

   printf("%s\n", title);
   for (i = 0; i < m; i++) {
      for (j = 0; j < n; j++){
    	  printf("%4.1f ", A[i*n + j]);
      }
      printf("\n");
   }
}

void print_vector(char* title, double y[], int m) {
   int   i;

   printf("%s\n", title);
   for (i = 0; i < m; i++){
	   printf("%4.1f ", y[i]);
   }
   printf("\n");
}

void *Thread_work(void* rank) {
   long my_rank = (long) rank;
   int i, j , x=0;
   int local_m = m/thread_count;
   double* local_y;
   local_y = malloc(local_m*sizeof(double));
   int my_first_row = my_rank*local_m;
   int my_last_row = my_first_row + local_m - 1;

   for (i = my_first_row; i <= my_last_row; i++ , x++) {
      local_y[i] = 0.0;
      for (j = 0; j < n; j++)
          local_y[x] += A[i*n+j]*X[j];
   }

   pthread_mutex_lock(&mutex);
   for(int i=0 ; i<local_m ; i++){
	   Y[(my_rank*local_m)+i] = local_y[i];
   }
   pthread_mutex_unlock(&mutex);
   return NULL;
}




