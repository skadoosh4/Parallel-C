#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mpi.h>


void Gen_data(float local_data[], int local_data_count, int data_count, float min_meas, float max_meas, int my_rank, MPI_Comm comm);
void Get_input(int* bin_count_p,float* min_meas_p, float* max_meas_p, int* data_count_p, int* local_data_count_p, int my_rank, int comm_sz, MPI_Comm comm);
void Find_bins(int bin_counts[], float local_data[], int local_bin_cts[], int local_data_count, float bin_maxes[], int bin_count, float min_meas, MPI_Comm comm);
int Which_bin(float data, float bin_maxes[], int bin_count, float min_meas);
void Set_bins(float bin_maxes[], int local_bin_cts[], float min_meas, float max_meas, int bin_count, int my_rank, MPI_Comm comm);
void print_histogram(float bin_maxes[], int bin_counts[], int bin_count, float min_meas);

int main(){
	float min_meas , max_meas;
	float* bin_maxes;
	int* bin_counts;
	int* local_bin_cts;
	int data_count , local_data_count , bin_count , my_rank , comm_sz;
	float* data;
	float* local_data;
	MPI_Comm comm;

	MPI_Init(NULL , NULL);
	comm = MPI_COMM_WORLD;
	MPI_Comm_rank(comm , &my_rank);
	MPI_Comm_size(comm , &comm_sz);

	Get_input(&bin_count , &min_meas , &max_meas , &data_count , &local_data_count , my_rank , comm_sz , comm);

	bin_maxes = malloc(bin_count*sizeof(float));
	bin_counts = malloc(bin_count*sizeof(int));
	local_bin_cts = malloc(bin_count*sizeof(int));
	data = malloc(data_count*sizeof(float));
	local_data = malloc(local_data_count*sizeof(float));

	Gen_data(local_data,local_data_count,data_count,min_meas,max_meas,my_rank,comm);
	Set_bins(bin_maxes,local_bin_cts,min_meas,max_meas,bin_count,my_rank,comm);
	Find_bins(bin_counts,local_data,local_bin_cts,local_data_count,bin_maxes,bin_count,min_meas,comm);

	MPI_Reduce(local_bin_cts , bin_counts , bin_count , MPI_INT , MPI_SUM , 0 , comm);

	if(my_rank==0){
		print_histogram(bin_maxes , bin_counts , bin_count , min_meas);
	}
	free(bin_maxes);
	free(bin_counts);
	free(local_bin_cts);
	free(local_data);
	free(data);

	MPI_Finalize();
}


void Gen_data(float local_data[], int local_data_count, int data_count, float min_meas, float max_meas, int my_rank, MPI_Comm comm) {
  float* data;
  if(my_rank == 0) {
    float range = max_meas - min_meas;
    data = malloc(data_count*sizeof(float));
    printf("Data : ");
    int i;
    for(i=0;i<data_count;i++) {
      data[i] = (float) rand() / (float) RAND_MAX * range + min_meas;
      printf("%f ," , data[i]);
    }
  }
  MPI_Scatter(data,local_data_count,MPI_FLOAT,local_data,local_data_count,MPI_FLOAT, 0, comm);
  if(my_rank == 0) free(data);
}


void Get_input(int* bin_count_p,float* min_meas_p, float* max_meas_p, int* data_count_p, int* local_data_count_p, int my_rank, int comm_sz, MPI_Comm comm) {

  if(my_rank == 0) {
    printf("Enter the number of bins : ");
    fflush(stdout);
    scanf(" %d", bin_count_p);
    printf("Enter the minimum value : ");
    fflush(stdout);
    scanf(" %f", min_meas_p);
    printf("Enter the maximum value : ");
    fflush(stdout);
    scanf(" %f", max_meas_p);
    if(*max_meas_p < *min_meas_p) {
      float* temp = max_meas_p;
      max_meas_p = min_meas_p;
      min_meas_p = temp;
    }
    printf("Enter the number of random values to be used : ");
    fflush(stdout);
    scanf(" %d", data_count_p);
    *local_data_count_p = *data_count_p / comm_sz;
  }
  MPI_Bcast(bin_count_p,1,MPI_INT,0,comm);
  MPI_Bcast(min_meas_p,1,MPI_FLOAT,0,comm);
  MPI_Bcast(max_meas_p,1,MPI_FLOAT,0,comm);
  MPI_Bcast(data_count_p,1,MPI_INT,0,comm);
  MPI_Bcast(local_data_count_p,1,MPI_INT,0,comm);
}


void Find_bins(int bin_counts[], float local_data[], int local_bin_cts[], int local_data_count, float bin_maxes[], int bin_count, float min_meas, MPI_Comm comm){
  int i;
  int bin;
  for(i = 0; i < local_data_count; i++) {
    bin = Which_bin(local_data[i],bin_maxes,bin_count,min_meas);
    local_bin_cts[bin]++;
  }
}


int Which_bin(float data, float bin_maxes[], int bin_count, float min_meas) {
  int i;
  for(i = 0; i < bin_count-1; i++) {
    if(data <= bin_maxes[i])
    	break;
  }
  return i;
}


void Set_bins(float bin_maxes[], int local_bin_cts[], float min_meas, float max_meas, int bin_count, int my_rank, MPI_Comm comm) {
  float range = max_meas - min_meas;
  float interval = range / bin_count;
  int i;
  for(i = 0; i < bin_count; i++) {
    bin_maxes[i] = min_meas + (interval * (float)(i+1));
    local_bin_cts[i] = 0;
  }
}

void print_histogram(float bin_maxes[], int bin_counts[], int bin_count, float min_meas) {
  int width = 40;
  int max = 0;
  int row_width;
  int i;
  int j;
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



