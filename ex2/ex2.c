#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <pthread.h>

struct timespec tempoInicio, tempoFim;
int* tempoatual;

void* atualizarTempo(){
    clock_gettime(CLOCK_MONOTONIC_RAW, &tempoFim);
    tempoatual = ((tempoFim.tv_sec - tempoInicio.tv_sec) * 1000000 + (tempoFim.tv_nsec - tempoInicio.tv_nsec) / 1000)/1000000;
    printf("%d\n", tempoatual);
}

void* clockStart(){
    clock_gettime(CLOCK_MONOTONIC_RAW, &tempoInicio);
    while(tempoatual <= 60){
        atualizarTempo();
    }
    return;
}

void* inputMain(){

}

int main(){
    tempoatual = 0;

    pthread_t principal, input;
    int  statusPrincipal, statusInput;

    statusPrincipal = pthread_create( &principal, NULL, clockStart, NULL);
    statusInput = pthread_create( &input, NULL, inputMain, NULL);
    
    if(statusPrincipal || statusInput){
        printf("Deu merda\n");
        exit(1);
    }

    pthread_join(principal, NULL);
    //Executa esse código após o timer acabar

    return 0;
}