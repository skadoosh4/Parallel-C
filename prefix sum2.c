#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void Read_n(int* n_p, int* local_n_p, int my_rank, int comm_sz,
    MPI_Comm comm);

void Generate_data(double local_data[], int local_n, int comm_sz,
    int my_rank, MPI_Comm comm);

void Partial_sums(double local_data[], double local_sums[], int local_n,
    int comm_sz, int my_rank, MPI_Comm comm);

void Print_result(double local_data[], double local_sums[], int local_n,
    int comm_sz, int my_rank, MPI_Comm comm);

int main(void) {
    int n, local_n;
    double *local_arr, *local_sums;
    int my_rank, comm_sz;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    Read_n(&n, &local_n, my_rank, comm_sz, MPI_COMM_WORLD);

    local_arr = malloc(local_n * sizeof(double));
    local_sums = malloc(local_n * sizeof(double));

    Generate_data(local_arr, local_n, comm_sz, my_rank, MPI_COMM_WORLD);

    Partial_sums(local_arr, local_sums, local_n, comm_sz, my_rank, MPI_COMM_WORLD);

    Print_result(local_arr, local_sums, local_n, comm_sz, my_rank, MPI_COMM_WORLD);

    free(local_arr);
    free(local_sums);
    MPI_Finalize();
    return 0;
}

void Read_n(int* n_p, int* local_n_p, int my_rank, int comm_sz,
    MPI_Comm comm) {

    if(my_rank == 0) {
        printf("Enter the size n of the array:\n");
        // Added fflush(stdout) to prevent hanging
        fflush(stdout);
        scanf("%d", n_p);
        *local_n_p = *n_p / comm_sz;
    }

    MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
    MPI_Bcast(local_n_p , 1, MPI_INT, 0, comm);
}

void Generate_data(double local_data[], int local_n, int comm_sz,
    int my_rank, MPI_Comm comm) {

    for(int i = 0; i < local_n; i++) {
        local_data[i] = i + 1 + (my_rank*local_n);
    }
}


void Partial_sums(double local_data[], double local_sums[], int local_n,
    int comm_sz, int my_rank, MPI_Comm comm) {

        double last_partial_sum = 0;
        for(int i = 0; i < local_n; i++) {
            last_partial_sum += local_data[i];
        }
        MPI_Scan(&last_partial_sum, &local_sums[local_n-1], 1, MPI_DOUBLE, MPI_SUM, comm);

        for(int i = local_n-2; i >= 0; i--) {
            local_sums[i] = local_sums[i+1] - local_data[i+1];
        }
}

void Print_result(double local_data[], double local_sums[], int local_n,
    int comm_sz, int my_rank, MPI_Comm comm) {

    double *a = NULL;
    double *b = NULL;

    if(my_rank == 0) {
        a = malloc(local_n * comm_sz * sizeof(double));
        b = malloc(local_n * comm_sz * sizeof(double));
        MPI_Gather(local_data, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm);
        MPI_Gather(local_sums, local_n, MPI_DOUBLE, b, local_n, MPI_DOUBLE, 0, comm);

        printf("Original Array: ");
        for(int i = 0; i < local_n*comm_sz; i++) {
            printf("%.2f ", a[i]);
        }
        printf("\n");
        printf("Prefix Sums: ");
        for(int i = 0; i < local_n*comm_sz; i++) {
            printf("%.2f ", b[i]);
        }
        printf("\n");

    } else {
        MPI_Gather(local_data, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm);
        MPI_Gather(local_sums, local_n, MPI_DOUBLE, b, local_n, MPI_DOUBLE, 0, comm);
    }

}
