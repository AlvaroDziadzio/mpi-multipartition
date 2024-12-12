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
                printf("Erro no elemento Output[%d] = %lld na partição [%lld, %lld)\n", i, output[i], npMin, npMax);
                return;
            }
        }
    }

    // Confirmação de que todos os elementos foram particionados corretamente
    printf("\n===Particionamento CORRETO===\n");
}

