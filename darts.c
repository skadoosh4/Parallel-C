#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<mpi.h>
#include<string.h>

void Get_input(long* total_tosses , int my_rank);
long tosses(long toss_per_process , int my_rank);
double randfrom(double min , double max);

int main(){
	int my_rank , comm_sz;
	long total_tosses , toss_per_process , number_in_circle_process , total_circle;
	double pi_estimate;
	MPI_Comm comm;

	MPI_Init(NULL, NULL);
	comm = MPI_COMM_WORLD;
	MPI_Comm_rank(comm , &my_rank);
	MPI_Comm_size(comm , &comm_sz);

	Get_input(&total_tosses , my_rank);

	toss_per_process = total_tosses / comm_sz;

	number_in_circle_process = tosses(toss_per_process , my_rank);

	MPI_Reduce(&number_in_circle_process , &total_circle , 1 , MPI_LONG , MPI_SUM , 0 , MPI_COMM_WORLD);

	if(my_rank==0){
		pi_estimate = (4*total_circle) / (double) total_tosses;
		printf("Pi is approximately :-  %f" , pi_estimate);
		fflush(stdout);
	}

	MPI_Finalize();
	return 0;
}

void Get_input(long* total_tosses , int my_rank){
	if(my_rank==0){
		printf("Enter the total number of tosses \n");
		fflush(stdout);
		scanf("%ld",total_tosses);
	}
	MPI_Bcast(total_tosses , 1 , MPI_LONG , 0 , MPI_COMM_WORLD);
}

long tosses(long toss_per_process , int my_rank){
	srand(time(NULL));
	long toss , number_in_circle = 0;
	double x , y;
	for(toss=0 ; toss<toss_per_process ; toss++){
		x = randfrom(-1.0,1.0);
		y = randfrom(-1.0,1.0);
		if((x*x + y*y) <= 1.0){
			number_in_circle++;
		}
	}
	return number_in_circle;
}

double randfrom(double min , double max){
	double range = (max-min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
}
