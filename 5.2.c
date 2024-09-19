#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

int thread_count;
pthread_mutex_t mutex;
float* bin_maxes;
float* data;
int bin_count , data_count;
float min_meas , max_meas;
int* bin_counts;
int local_data_count;

void Gen_data(float min_meas ,float max_meas, float local_data[],int data_count);
void Get_input(int* bin_count_p,float* min_meas_p, float* max_meas_p, int* data_count_p);
int Which_bin(float data, float bin_maxes[], int bin_count);
void Set_bins(float min_meas,float max_meas, float bin_maxes[],  int bin_counts[], int bin_count);
void print_histogram(float  bin_maxes[], int bin_counts[], int bin_count, float min_meas);
void* Thread_work(void* rank);

int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;

	pthread_mutex_init(&mutex , NULL);

	printf("Enter the number of threads : \n");
	scanf("%d" , &thread_count);
	thread_handles = malloc(thread_count*sizeof(pthread_t));

	Get_input(&bin_count , &min_meas , &max_meas , &data_count);

	bin_maxes = malloc(bin_count*sizeof(float));
	bin_counts = malloc(bin_count*sizeof(int));
	data = malloc(data_count*sizeof(float));

	local_data_count = data_count / thread_count;

	Gen_data(min_meas, max_meas, data, data_count);
	Set_bins(min_meas, max_meas, bin_maxes, bin_counts, bin_count);

	for(thread = 0; thread < thread_count; thread++) {
		pthread_create(&thread_handles[thread], NULL, Thread_work,
					   (void*) thread);
	}

	for(thread = 0; thread < thread_count; thread++) {
		pthread_join(thread_handles[thread], NULL);
	}

	print_histogram(bin_maxes , bin_counts , bin_count , min_meas);

	pthread_mutex_destroy(&mutex);

	free(thread_handles);
    free(data);
	free(bin_maxes);
	free(bin_counts);

	return 0;
}

void Get_input(int* bin_count_p,float* min_meas_p, float* max_meas_p, int* data_count_p){
	    printf("Enter the number of bins : ");
	    scanf(" %d", bin_count_p);
	    printf("Enter the minimum value : ");
	    scanf(" %f", min_meas_p);
	    printf("Enter the maximum value : ");
	    scanf(" %f", max_meas_p);
	    if(*max_meas_p < *min_meas_p) {
	      float* temp = max_meas_p;
	      max_meas_p = min_meas_p;
	      min_meas_p = temp;
	    }
	    printf("Enter the number of random values to be used : ");
	    scanf(" %d", data_count_p);
}

void *Thread_work(void* rank) {
	long my_rank = (long) rank;
	float* local_data;
	int* local_bin;
	int bin;
	local_bin = malloc(bin_count*sizeof(int));
	local_data = malloc(local_data_count*sizeof(float));
	for(int i=0 ; i<local_data_count ; i++){
		local_data[i] = data[(my_rank*local_data_count)+i];
	}
	for(int i=0 ; i<local_data_count ; i++){
		bin = Which_bin(local_data[i] , bin_maxes , bin_count);
		local_bin[bin]++;
	}
	pthread_mutex_lock(&mutex);
	for(int i=0 ; i<bin_count ; i++){
		bin_counts[i] += local_bin[i];
	}
	pthread_mutex_unlock(&mutex);
   return NULL;
}

void print_histogram(float bin_maxes[], int bin_counts[], int bin_count, float min_meas) {
  int max = 0;
  int i;
  for(i = 0; i < bin_count; i++) {
    if(bin_counts[i] > max)
      max = bin_counts[i];
  }
  printf("\n");
  for(i = 0; i < bin_count; i++) {
	  printf("%10.3f  | " , bin_maxes[i-1]);
	  printf("%10.3f" , bin_maxes[i]);
	  printf("  %d\n",bin_counts[i]);
  }
}


void Gen_data(float min_meas ,float max_meas, float local_data[],int data_count) {
    float range = max_meas - min_meas;
    printf("Data : ");
    int i;
    for(i=0;i<data_count;i++) {
      data[i] = (float) rand() / (float) RAND_MAX * range + min_meas;
      printf("%f ," , data[i]);
    }
}

int Which_bin(float data, float bin_maxes[], int bin_count) {
  int i;
  for(i = 0; i < bin_count-1; i++) {
    if(data <= bin_maxes[i]){
    	break;
    }
  }
  return i;
}

void Set_bins(float min_meas,float max_meas, float bin_maxes[],  int bin_counts[], int bin_count) {
   float bin_width;
   int   i;

   bin_width = (max_meas - min_meas)/bin_count;

   for (i = 0; i < bin_count; i++) {
      bin_maxes[i] = min_meas + (i+1)*bin_width;
      bin_counts[i] = 0;
   }
}
