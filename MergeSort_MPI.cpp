#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <iostream>

using namespace std;

#define ENABLE_PRINT_VECTOR 0
#define RANDOM_MAX 1000000000

void Join           (int vec[], int begin, int mid, int end, int vec_aux[]);
void MergeSort      (int vec[], int begin, int end, int vec_aux[]);
void MergeSort      (int vec[], int size);
void PrintVector    (int vec[], int size);
void GenerateVector (int vec[], int size);


int main(int argc, char* argv[]) {
    srand (time(NULL));

    int size = atoi(argv[1]);
    int* vec = (int*) malloc (size * sizeof(int));
    GenerateVector (vec, size);

    // MPI Inicio
    MPI_Init(NULL, NULL);
    int nprocs, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    
    double start_t = MPI_Wtime();
    double start, total = 0.0, total_t=0.0, copy_to=0.0, copy_back=0.0, process=0.0;

    if(my_rank == 0 && ENABLE_PRINT_VECTOR){
        printf("Unsorted:\n");
        PrintVector(vec, size);
    }

    int partition_size = size / nprocs;

    int* sub_vec = (int*) malloc (partition_size * sizeof(int));

    start = MPI_Wtime();
    MPI_Scatter(vec, partition_size, MPI_INT, sub_vec, partition_size, MPI_INT, 0, MPI_COMM_WORLD);
    copy_to += MPI_Wtime() - start;

    start = MPI_Wtime();
    MergeSort(sub_vec, partition_size);
    process += MPI_Wtime() - start;

    start = MPI_Wtime();
    MPI_Gather(sub_vec, partition_size, MPI_INT, vec, partition_size, MPI_INT, 0, MPI_COMM_WORLD);
    copy_back += MPI_Wtime() - start;

    total   = MPI_Wtime() - start_t;

    if(my_rank == 0){
        MergeSort(vec, size);
        if(ENABLE_PRINT_VECTOR){
            printf("Sorted:\n");
            PrintVector(vec, size); 
        }
    }


    free(sub_vec);
    free(vec);

    
    MPI_Barrier(MPI_COMM_WORLD);
    total_t = MPI_Wtime() - start_t;

    for(auto i=0; i<nprocs; i++){
        MPI_Barrier(MPI_COMM_WORLD);
        if(i == my_rank)
            printf("%d %d %lf %lf %lf %lf %lf %lf\n", size, my_rank, copy_to, copy_back, copy_to+copy_back, process, 
                    total, total_t);
    }

    // MPI fim
    MPI_Finalize();
    return 0;
}


void Join(int vec[], int begin, int mid, int end, int aux_vec[]) {
    int left = begin;
    int right = mid;

    for (int i = begin; i < end; ++i) {
        if ((left < mid) and ((right >= end) or (vec[left] < vec[right]))) {
            aux_vec[i] = vec[left];
            ++left;
        }
        else {
            aux_vec[i] = vec[right];
            ++right;
        }
    }

    for (int i = begin; i < end; ++i) {
        vec[i] = aux_vec[i];
    }
}

void MergeSort(int vec[], int begin, int end, int aux_vec[]) {
    int mid = ((begin + end)/2);
    if ((end - begin) < 2) 
        return;

    MergeSort(vec, begin, mid, aux_vec);
    MergeSort(vec, mid, end, aux_vec);

    Join(vec, begin, mid, end, aux_vec);
}

void MergeSort(int vec[], int size) { //função que o usuario realmente chama criando vetor auxiliar
    int* aux_vec = (int*) malloc(size * sizeof(int));
    MergeSort(vec, 0, size, aux_vec);
    free(aux_vec);
}

void PrintVector(int vec[], int size){
    for(auto i=0; i<size; i++){
        printf("%d ", vec[i]);
    }
    printf("\n\n");
    fflush(stdout);
}

void GenerateVector (int vec[], int size){
    for(auto i=0; i<size; i++){
        vec[i] = rand() % (RANDOM_MAX + 1);
    }
}