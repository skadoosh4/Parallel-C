#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void get_prefix_sum(
    double *local_arr,
    int local_n,
    double *local_prefix_sum,
    int rank,
    int size,
    int *temp){
    local_prefix_sum[0] = local_arr[0];
    for (int i = 1; i < local_n; i++){
        local_prefix_sum[i] = local_prefix_sum[i - 1] + local_arr[i];
    }


    int last_element;
    int offset;
    last_element = local_prefix_sum[local_n - 1];
    MPI_Scan(&last_element, temp, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    offset = *temp - last_element;
    for (int i = 0; i < local_n; i++){
        local_prefix_sum[i] += offset;
    }
}

void print_array(double *array, int n, int rank, char *msg){
    if (rank == 0){
        printf("%s ", msg);
        for (int i = 0; i < n; i++){
            printf("%f ", array[i]);
        }
        printf("\n");
    }
}

void generate_data(double *arr, int n, int local_n, double *local_arr){
	int lower =0 , upper = 40;
    for (int i = 0; i < n; i++){
    	arr[i] = (rand() % upper+1 - lower) + lower;
    }
    MPI_Scatter(arr, local_n, MPI_DOUBLE, local_arr, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

void read_n(int *n, int *local_n, int rank, int size){
    if (rank == 0){
        printf("How many numbers do you want?\n");
        scanf("%d", n);
    }
    MPI_Bcast(n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    *local_n = *n / size;
}

int main(){
    MPI_Init(NULL, NULL);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double *arr, *local_arr;
    double *prefix_sum, *local_prefix_sum;
    int n, local_n;
    int temp = 0;

    read_n(&n, &local_n, rank, size);

    arr = malloc(sizeof(double) * n);
    local_arr = malloc(sizeof(double) * local_n);
    prefix_sum = malloc(sizeof(double) * n);
    local_prefix_sum = malloc(sizeof(double) * local_n);

    generate_data(arr, n, local_n, local_arr);
    print_array(arr, n, rank, "Original array::");
    get_prefix_sum(local_arr, local_n, local_prefix_sum, rank, size, &temp);
    MPI_Gather(local_prefix_sum, local_n, MPI_DOUBLE, prefix_sum, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    print_array(prefix_sum, n, rank, "Prefix sum::");

    free(arr);
    free(local_arr);
    free(prefix_sum);
    free(local_prefix_sum);

    MPI_Finalize();
    return 0;
}
