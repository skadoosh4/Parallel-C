#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void Read_n(int* n_p, int* local_n_p, int my_rank, int comm_sz, MPI_Comm comm);
void Read_vector(double local_vec1[], double local_vec2[], int local_n, MPI_Datatype type, int my_rank, int comm_sz, MPI_Comm comm);
void Print_vector(double local_vec[], int local_n, int n, MPI_Datatype type, char title[], int my_rank, MPI_Comm comm);
void Sum_vector(double local_vec1[], double local_vec2[], double vector_sum[], int local_n);

int main(void){
    int n, local_n;
    int my_rank, comm_sz;
    double *local_vec1, *local_vec2, *vector_sum;
    MPI_Comm comm;
    MPI_Datatype type;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    comm = MPI_COMM_WORLD;

    Read_n(&n, &local_n, my_rank, comm_sz, comm);

    MPI_Type_contiguous(local_n, MPI_DOUBLE, &type);
    MPI_Type_commit(&type);

    local_vec1 = malloc(n * sizeof(double));
    local_vec2 = malloc(n * sizeof(double));
    vector_sum = malloc(n * sizeof(double));

    Read_vector(local_vec1, local_vec2, local_n, type, my_rank, comm_sz, comm);
    Print_vector(local_vec1, local_n, n, type, "Vector 1:", my_rank, comm);
    Print_vector(local_vec2, local_n, n, type, "Vector 2:", my_rank, comm);
    Sum_vector(local_vec1, local_vec2, vector_sum, local_n);
    Print_vector(vector_sum, local_n, n, type, "Vector Sum:", my_rank, comm);
    if(my_rank == 0) printf("\n");

    free(vector_sum);
    free(local_vec2);
    free(local_vec1);
    MPI_Finalize();
    return 0;
}

void Read_n(int* n_p, int* local_n_p, int my_rank, int comm_sz, MPI_Comm comm) {
    if(my_rank == 0) {
        printf("Enter the size n of the vectors:\n");
        scanf("%d", n_p);
        *local_n_p = *n_p / comm_sz;
    }
    MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
    MPI_Bcast(local_n_p, 1, MPI_INT, 0, comm);
}

void Read_vector(double local_vec1[], double local_vec2[], int local_n, MPI_Datatype type, int my_rank, int comm_sz, MPI_Comm comm){
    double* a = NULL;
    double* b = NULL;
    int i;
    if(my_rank == 0){
        a = malloc(local_n*comm_sz*sizeof(double));
        printf("Enter the values for first vector:\n");
        for(i = 0; i < local_n*comm_sz; i++){
            scanf("%lf", &a[i]);
        }

        b = malloc(local_n*comm_sz*sizeof(double));
        printf("Enter the values for the second vector:\n");
        for(i = 0; i < local_n*comm_sz; i++){
            scanf("%lf", &b[i]);
        }

        MPI_Scatter(a, 1, type, local_vec1, 1, type, 0, comm);
        MPI_Scatter(b, 1, type, local_vec2, 1, type, 0, comm);

        free(a);
        free(b);
    }else{
        MPI_Scatter(a, 1, type, local_vec1, 1, type, 0, comm);
        MPI_Scatter(b, 1, type, local_vec2, 1, type, 0, comm);
    }
}

void Print_vector(double local_vec[], int local_n, int n, MPI_Datatype type, char title[], int my_rank, MPI_Comm comm){
    double* a = NULL;
    int i;

    if (my_rank == 0) {
        a = malloc(n * sizeof(double));
        MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm);

        printf("%s\n", title);
        for(i = 0; i < n; i++) {
            printf("%.2f ", a[i]);
        }
        printf("\n");

        free(a);
    }else{
      MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm);
   }
}

void Sum_vector(double local_vec1[], double local_vec2[], double vector_sum[], int local_n){
    for(int i = 0; i <= local_n; i++){
        vector_sum[i] = local_vec1[i] + local_vec2[i];
    }
}
