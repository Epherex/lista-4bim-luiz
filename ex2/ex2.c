#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

time_t tempoInicio, tempoFim;
int* tempoatual;

void* atualizarTempo(){
    time (&tempoFim);
    tempoatual = (int) difftime(tempoFim, tempoInicio);
    printf("%d\n", tempoatual);
}

void* clockStart(){
    time (&tempoInicio);
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