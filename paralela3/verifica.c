// TRABALHO 3 - CI1316: Programação Paralela
// Victor Ribeiro Garcia - GRR20203954
// Álvaro R. S. Dziadzio - GRR20203913

#include <stdio.h>
#include "verifica.h"

void verifica_particoes(long long *input, int n, long long *P, int np, long long *output, int *Pos) {
    for (int j = 0; j < np; j++) {
        long long npMin = (j == 0) ? 0 : P[j - 1];
        long long npMax = P[j];
        int inicio = Pos[j];
        int fim = (j == np - 1) ? n : Pos[j + 1];

        for (int i = inicio; i < fim; i++) {
            if (output[i] < npMin || output[i] >= npMax) {
                printf("\n===Particionamento COM ERROS===\n");
                return;
            }
        }
    }
    printf("\n===Particionamento CORRETO===\n");
}

