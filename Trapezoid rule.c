#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mpi.h>


void Get_input(double* a_p , double* b_p , int* n_p , int my_rank);
double Trap(double left_endpt , double right_endpt , int trap_count , double base_len);


int main(void){
	int my_rank , comm_sz , n , local_n;
	double a , b , h , local_a , local_b;
	double local_int , total_int;


	MPI_Init(NULL , NULL);
	MPI_Comm_rank(MPI_COMM_WORLD , &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD , &comm_sz);

	Get_input(&a , &b , &n , my_rank);

	h = (b-a) / n;
	local_n = n/comm_sz;

	local_a = a + my_rank * local_n * h;
	local_b = local_a + local_n*h;
	local_int = Trap(local_a , local_b , local_n , h);

	MPI_Reduce(&local_int , &total_int , 1 , MPI_DOUBLE , MPI_SUM , 0 , MPI_COMM_WORLD); //Computing the sum of the areas
	if(my_rank == 0){
		printf("With n = %d trapezoids , our estimate is :\n" , n);
		printf("of the integral from %lf to %lf = %lf\n" , a , b , total_int);
	}
	MPI_Finalize();
}

void Get_input(double* a_p , double* b_p , int* n_p , int my_rank){
	if(my_rank == 0){
		printf("Enter the lower bound of the interval: \n");
		scanf("%lf" , a_p);
		printf("Enter the upper bound of the interval: \n");
		scanf("%lf" , b_p);
		printf("Enter the number of trapezoids: \n");
		scanf("%d" , n_p);
	}
		MPI_Bcast(a_p , 1 , MPI_DOUBLE , 0 , MPI_COMM_WORLD);
		MPI_Bcast(b_p , 1 , MPI_DOUBLE , 0 , MPI_COMM_WORLD);
		MPI_Bcast(n_p , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
}


double Trap(double left_endpt , double right_endpt , int trap_count , double base_len){
	double estimate , x;
	int i;

	double f(double x);

	estimate = (f(left_endpt) + f(right_endpt)) / 2.0;   // area of a single trapezoid
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
