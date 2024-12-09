// TRABALHO 2 - CI1316: Programação Paralela
// Victor Ribeiro Garcia - GRR20203954
// Álvaro R. S. Dziadzio - GRR20203913

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

#include "chrono.h"

#define MAX_THREADS 16
#define INPUT_ELEM 8000000
#define NTIMES 10
// N_PART é definido na compilação (ver implementação no MAKEFILE)

typedef struct {
    int thread_id;
    int n_threads;
    long long *input;
    int n;
    long long *part;
    int n_part;
    long long *output;
    int *partition_starts;
    atomic_int *partition_positions;
    int start;
    int end;
} ThreadInfo;

typedef struct {
        pthread_t thread;
        ThreadInfo *thread_info;
        int is_run;
} ThreadPool;

pthread_barrier_t barrier;
atomic_int running_threads;
ThreadPool threads_pool[MAX_THREADS];

// 
int *alocaIntVet(int tamanho) {
        int *vetor = malloc(sizeof(int) * tamanho);
        return vetor;
}

// Aloca um vetor de long long
long long *alocaLongLongVet(int tamanho) {
        long long *vetor = malloc(sizeof(long long) * tamanho);
        return vetor;
}

// Gera um número aleatório long long
long long randomLongLong() {
    long long high = (long long)random();
    long long low = (long long)random();
    return (high << 32) | low;
}

// Função de comparação para o qsort
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

// Verifica se as partições estão corretas
void verifica_particoes(long long *input, int tam, long long *part, int num_part, long long *output, int *partition_starts) {
    for (int i = 0; i < num_part; i++) {
        int inicio = partition_starts[i];
        int fim = (i == num_part - 1) ? tam : partition_starts[i + 1];

        // Determina os limites da partição atual
        long long lower_bound = (i == 0) ? LLONG_MIN : part[i - 1];
        long long upper_bound = part[i];

        for (int j = inicio; j < fim; j++) {
            if (output[j] < lower_bound || output[j] >= upper_bound) {
                printf("Particionamento INCORRETO\n");
                return;
            }
        }
    }
    printf("Particionamento CORRETO\n");
}

void *thread_pool_task(void *arg) {
        ThreadPool *thread = (ThreadPool *)arg;
        while (1) {
                // Sincroniza todas as threads antes de iniciar
                pthread_barrier_wait(&barrier);

                // Verifica se a thread deve continuar ativa
                if (!thread->is_run) {
                        break;
                }

                // Obtém os dados da task
                ThreadInfo *data = thread->thread_info;
                for (int i = data->start; i < data->end; i++) {
                        int current_part = busca_binaria(data->part, data->n_part, data->input[i]);
                        int index = atomic_fetch_add(&data->partition_positions[current_part], 1);
                        data->output[index] = data->input[i];
                }
                atomic_fetch_sub(&running_threads, 1);
        }
        return NULL;
}

void thread_pool_create(int n_threads) {
    // Inicializa a barreira com o número de threads mais a principal
    pthread_barrier_init(&barrier, NULL, n_threads + 1);

    // Cria e configura cada thread no pool
    for (int i = 0; i < n_threads; i++) {
        ThreadPool *current_thread = &threads_pool[i];
        current_thread->is_run = 1;
        pthread_create(&current_thread->thread, NULL, thread_pool_task, current_thread);
    }
}

void stop_thread_pool(int n_threads) {
    // Desativar todas as threads no pool
    for (int i = 0; i < n_threads; i++) {
        threads_pool[i].is_run = 0;
    }

    // Sincronizar todas as threads com a barreira
    pthread_barrier_wait(&barrier);

    // Esperar que todas as threads terminem sua execução
    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads_pool[i].thread, NULL);
    }

    // Destruir a barreira
    pthread_barrier_destroy(&barrier);
}

// Função que realiza a partição de um vetor de long long
void multi_partition(long long *input, int n, long long *part, int n_part, long long *output, int *partition_starts, int n_threads) {
        int *partition_block = calloc(n_part, sizeof(int));
        atomic_int *partition_positions = malloc(sizeof(atomic_int) * n_part);
        for (int i = 0; i < n_part; i++) atomic_init(&partition_positions[i], 0);

        for (int i = 0; i < n; i++) partition_block[busca_binaria(part, n_part, input[i])]++;

        partition_starts[0] = 0;
        for (int i = 1; i < n_part; i++) {
                partition_starts[i] = partition_starts[i - 1] + partition_block[i - 1];
                atomic_store(&partition_positions[i], partition_starts[i]);
        }

        int chunk_size = (n + n_threads - 1) / n_threads;
        running_threads = n_threads;

        for (int i = 0; i < n_threads; i++) {
                threads_pool[i].thread_info = malloc(sizeof(ThreadInfo));
                *threads_pool[i].thread_info = (ThreadInfo){i, n_threads, input, n, part, n_part, output, partition_starts, partition_positions, i * chunk_size, (i + 1) * chunk_size < n ? (i + 1) * chunk_size : n};
        }

        pthread_barrier_wait(&barrier);
        while (atomic_load(&running_threads) > 0);

        free(partition_block);
        free(partition_positions);
}

int main(int argc, char *argv[]) {

        if (argc != 2) {
                printf("Usage: %s <n_threads>\n", argv[0]);
                return EXIT_FAILURE;
        }

        int n_threads = atoi(argv[1]);
        if (n_threads <= 0 || n_threads > MAX_THREADS) {
                printf("Número inválido de threads. Use valores entre 1 and 8.\n");
                return EXIT_FAILURE;
        }

        // Aloca os vetores de entrada, partição e saída
        long long *part = alocaLongLongVet(N_PART * 32);
        int *partition_starts = alocaIntVet(N_PART);
        long long *input = alocaLongLongVet(INPUT_ELEM);
        long long *output = alocaLongLongVet(INPUT_ELEM);

        chronometer_t parallelTime;
        int n = INPUT_ELEM;
        int n_part = N_PART;

        srand(111);
        for (int i = 0; i < INPUT_ELEM; i++)
                input[i] = randomLongLong();
        for (int i = 0; i < N_PART - 1; i++)
                part[i] = randomLongLong();
        part[N_PART - 1] = LLONG_MAX;

        qsort(part, N_PART, sizeof(long long), compara);

        // Evitar os efeitos de caching
        for (int i = 0; i < 32; i++) {
                memcpy(part + i * n_part, part, n_part * sizeof(*part));
        }
        long long *current_part = part;
        
        chrono_reset(&parallelTime);
        chrono_start(&parallelTime);

        thread_pool_create(n_threads);
        for (int i = 0; i < NTIMES; i++) {
                multi_partition(input, n, current_part, n_part, output, partition_starts, n_threads);
                verifica_particoes(input, n, current_part, n_part, output, partition_starts);
                current_part += n_part;
        }
        stop_thread_pool(n_threads);

        chrono_stop(&parallelTime);
        double total_time_in_seconds = (double)chrono_gettotal(&parallelTime) / 1e9;

        chrono_reportTime(&parallelTime, "multiPartitionTime");
        printf("%.6f\n", total_time_in_seconds);

        // Libera a memória alocada
        free(input);
        free(part);
        free(output);
        free(partition_starts);
        return 0;
}
