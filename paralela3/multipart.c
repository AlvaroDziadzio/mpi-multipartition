// TRABALHO 3 - CI1316: Programação Paralela
// Victor Ribeiro Garcia - GRR20203954
// Álvaro R. S. Dziadzio - GRR20203913

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mpi.h>

#include "verifica.h"
#include "chrono.h"

#define NTIMES 10      // Número de vezes que o código será executado

long long numElem;

// Alocar Vetor Int
int *alocaVetInt(int n) {
    if (n <= 0) {
        printf("Erro: Tamanho do vetor deve ser maior que zero.\n");
        return NULL; // Retorna NULL se a alocação for inválida
    }
    int *vetor = malloc(sizeof(int) * n);
    if (!vetor) {
        printf("Erro ao alocar memória para vetor de inteiros. Tentativa de alocar %d elementos.\n", n);
        exit(EXIT_FAILURE); // Encerra o programa em caso de falha de alocação
    }
    return vetor;
}

// Alocar Vetor Long Long
long long *alocaVetLongLong(int n) {
    if (n <= 0) {
        printf("Erro: Tamanho do vetor deve ser maior que zero.\n");
        return NULL; // Retorna NULL se a alocação for inválida
    }
    long long *vetor = malloc(sizeof(long long) * n);
    if (!vetor) {
        printf("Erro ao alocar memória para vetor de long long. Tentativa de alocar %d elementos.\n", n);
        exit(EXIT_FAILURE); // Encerra o programa em caso de falha de alocação
    }
    return vetor;
}

// Gera um número long long aleatório
long long randomLongLong() {
    long long high = (long long)random();
    long long low = (long long)random();
    return (high << 32) | low;
}

// Preenche um vetor com valores aleatórios
void randomVet(long long *vetor, int n) {
        for (int i = 0; i < n; i ++) {
                vetor[i] = randomLongLong();
        }
}

// Função de comparação para qsort
int compara(const void *a, const void *b) {
    long long x = *(const long long *)a;
    long long y = *(const long long *)b;

    if (x < y) return -1; // x é menor que y
    if (x > y) return 1;  // x é maior que y
    return 0;             // x é igual a y
}

// Busca binária para encontrar a partição correta
int busca_binaria(long long *part, int n_part, long long value) {
        int inicio = 0;
        int fim = n_part - 1;
        while (inicio < fim) {
                int meio = (inicio + fim) / 2;
                if (value < part[meio]) {
                        fim = meio;
                } else {
                        inicio = meio + 1;
                }
        }
        return inicio;
}

void multi_partition_mpi(long long *input, int n, long long *P, int np, int *partitionStart, long long *output) {

        int pos = 0;
        int *num_part = calloc(np, sizeof(int)); 
        int *sendSizes = alocaVetInt(np);
        int *recvSizes = alocaVetInt(np);
        int *sendPos = alocaVetInt(np);
        int *recvPos = alocaVetInt(np);
        int *insertPos = alocaVetInt(np);
        int *partitionPos = calloc(np, sizeof(int));

        for (int i = 0; i < n; i++) {
                pos = busca_binaria(P, np, input[i]);
                num_part[pos]++;
        }

        pos = 0;
        for (int i = 0; i < np - 1; i++) {
                pos += num_part[i];
                partitionPos[i + 1] = pos;
        }

        for (int i = 0; i < np; i++) {
                insertPos[i] = partitionPos[i];
        }

        for (int i = 0; i < n; i++) {
                int part = busca_binaria(P, np, input[i]);
                output[insertPos[part]++] = input[i];
        }

        for (int i = 0; i < np; i++) {
                sendSizes[i] = num_part[i]; 
                sendPos[i] = partitionPos[i];          
        }

        MPI_Alltoall(sendSizes, 1, MPI_INT, recvSizes, 1, MPI_INT, MPI_COMM_WORLD);

        int recv_size = 0;
        for (int i = 0; i < np; i++) {
                recvPos[i] = recv_size;
                recv_size += recvSizes[i];
        }
        *partitionStart = recv_size;

        long long *recvElem = alocaVetLongLong(recv_size);

        MPI_Alltoallv(output, sendSizes, sendPos, MPI_LONG_LONG, recvElem, recvSizes, recvPos, MPI_LONG_LONG, MPI_COMM_WORLD);

        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        free(sendSizes);
        free(sendPos);
        free(recvSizes);
        free(recvPos);
        free(recvElem);
        free(num_part);
        free(partitionPos);
        free(insertPos);
}

int main(int argc, char *argv[]) {

        if (argc != 3) {
                return EXIT_FAILURE;
        }

        MPI_Init(&argc, &argv);

        int rank, size;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        long long numElem = atoll(argv[1]);
        int numProc = atoi(argv[2]);

        if (numProc != size) {
                if (rank == 0) {
                        printf("Erro: número de processos diferente do número de partições.\n");
                }
                MPI_Finalize();
                return EXIT_FAILURE;
        }

        srand(2024 * 100 + rank);

        int partitionStart; 
        int np = numProc;
        long long *P = alocaVetLongLong(np);
        long long n = numElem / np;
        long long *input = alocaVetLongLong(n);
        long long *output = alocaVetLongLong(n);

        if (rank == 0) {
                randomVet(P, np - 1);
                P[np - 1] = LLONG_MAX;
                qsort(P, np, sizeof(long long), compara);
        }

        MPI_Bcast(P, np, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

        randomVet(input, n);

        chronometer_t mp_mpi_time;

        for(int i = 0; i < NTIMES; i++) {
                chrono_reset(&mp_mpi_time);
                chrono_start(&mp_mpi_time);
                multi_partition_mpi(input, n, P, np, &partitionStart, output);
                MPI_Barrier(MPI_COMM_WORLD);
                chrono_stop(&mp_mpi_time);

    	        if (rank == 0) {
    	        	verifica_particoes(input, n, P, np, output, &partitionStart);
                        printf("numElem = %lld", numElem);
    	        	chrono_reportTime(&mp_mpi_time, "mp_mpi_time");
    	        	double total_time_in_seconds = (double) chrono_gettotal(&mp_mpi_time) / ((double)1000*1000*1000);
                	printf("total_time_in_seconds: %lf s\n", total_time_in_seconds);
                	double OPS = ((double)numElem) / total_time_in_seconds;
                	printf("Throughput: %lf OP/s\n", OPS);
    	        }
    	}
	
        MPI_Finalize();

        free(input);
        free(output);
        free(P);

        return EXIT_SUCCESS;
}
