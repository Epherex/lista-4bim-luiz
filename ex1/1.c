#include <stdio.h>
#include <stdlib.h>

int comp(const void *a, const void *b) {
    return *(int*) b - *(int*) a;
}

int main() {
    int numeros[300], conjunto1[300], conjunto2[300], i1 = 0, i2 = 0;
    long long int soma1 = 0, soma2 = 0;
    FILE *arq = fopen("numeros_particao.txt", "r");
    for(int i = 0; i < 300; i++) {
        fscanf(arq, "%d", &numeros[i]);
    }
    qsort(numeros, 300, sizeof(int), comp);
    for(int i = 0; i < 300; i++) {
        if(soma1 < soma2) {
            soma1 += numeros[i];
            conjunto1[i1] = numeros[i];
            i1++;
        } else {
            soma2 += numeros[i];
            conjunto2[i2] = numeros[i];
            i2++;
        }
    }
    int diff = abs(soma1 - soma2);
    while(diff > 7) {        
        int j1 = 0, j2 = 0;
        int menordiff = abs(diff/2 - abs(conjunto1[j1] - conjunto2[j2]));
        for(int i = 0; i < i1; i++) {
            for(int j = 0; j < i2; j++) {
                int d = abs(diff/2 - abs(conjunto1[i] - conjunto2[j]));
                if(d < menordiff && (soma1 < soma2 && conjunto1[i] < conjunto2[j] || soma1 > soma2 && conjunto1[i] > conjunto2[j])) {
                    j1 = i;
                    j2 = j;
                    menordiff = d;
                }
            }
        }
        soma1 -= conjunto1[j1];
        soma2 -= conjunto2[j2];
        soma1 += conjunto2[j2];
        soma2 += conjunto1[j1];
        diff = abs(soma1 - soma2);
        if(soma1 > soma2) {
            int temp = conjunto1[j1];
            conjunto1[j1] = conjunto2[j2];
            conjunto2[j2] = temp;
        } else {
            int temp = conjunto2[j2];
            conjunto2[j2] = conjunto1[j1];
            conjunto1[j1] = temp;
        }
    }
    fclose(arq);
    arq = fopen("resultado.txt", "w");
    fprintf(arq, "%d\n", diff);
    for(int i = 0; i < i1; i++) {
        fprintf(arq, "%d,1\n", conjunto1[i]);
    }
    for(int i = 0; i < i2; i++) {
        fprintf(arq, "%d,2\n", conjunto2[i]);
    }
    fclose(arq);
}