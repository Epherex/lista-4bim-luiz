#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

//Por que sem getch é foda né
#if !defined(_WIN32)
#include <unistd.h>
#include <termios.h>
char getch()
{
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    printf("%c", buf);
    return buf;
}
#endif

typedef struct charNode
{
    char data;
    struct charNode *prox;
} charNode;

typedef struct palavraNode
{
    char *data;
    struct palavraNode *prox;
} palavraNode;

typedef struct correcaoNode
{
    char *data;
    bool acertou;
    struct correcaoNode *prox;
} correcaoNode;

time_t tempoInicio, tempoFim;
palavraNode *inicioGeral = NULL;
palavraNode *inicioComparacao = NULL;
int *tempoatual;
bool rodando = true;

correcaoNode *corrigirPalavra(correcaoNode *inicio, char *entrada, char *gabarito)
{
    correcaoNode *aux, *aux2;
    bool acertou;
    //printf("Comparando |%s|(%d)  com  |%s|(%d) - ", entrada, strlen(entrada), gabarito, strlen(gabarito));
    if (strcmp(entrada, gabarito) == 0)
    {
        acertou = true;
    }
    else
    {
        acertou = false;
    }
    //printf("Resultado %d \n", acertou);

    if (inicio == NULL)
    {
        inicio = (correcaoNode *)malloc(sizeof(correcaoNode));
        inicio->data = entrada;
        if (acertou)
        {
            inicio->acertou = true;
        }
        else
        {
            inicio->acertou = false;
        }
        inicio->prox = NULL;
    }
    else
    {
        aux = inicio;
        while (aux->prox != NULL)
            aux = aux->prox;

        aux2 = (correcaoNode *)malloc(sizeof(correcaoNode));
        aux2->data = entrada;
        if (acertou)
        {
            aux2->acertou = true;
        }
        else
        {
            aux2->acertou = false;
        }
        aux2->prox = NULL;
        aux->prox = aux2;
    }

    return inicio;
}

palavraNode *inserirPalavraNode(palavraNode *inicio, char *entrada)
{
    palavraNode *aux, *aux2;

    if (inicio == NULL)
    {
        inicio = (palavraNode *)malloc(sizeof(palavraNode));
        inicio->data = entrada;
        inicio->prox = NULL;
    }
    else
    {
        aux = inicio;
        while (aux->prox != NULL)
            aux = aux->prox;

        aux2 = (palavraNode *)malloc(sizeof(palavraNode));
        aux2->data = entrada;
        aux2->prox = NULL;
        aux->prox = aux2;
    }

    return inicio;
}

charNode *reset(charNode *inicio)
{
    if (inicio != NULL)
    {
        charNode *atual = inicio, *proximo;
        if (atual->prox == NULL)
        {
            free(atual);
        }
        else
        {
            while (true)
            {
                proximo = atual->prox;
                free(atual);
                atual = proximo;
                if (atual == NULL)
                {
                    break;
                }
            }
        }
    }
    return NULL;
}

int leituraPalavras(palavraNode *inicio)
{
    int quantidade = 0;
    if (inicio != NULL)
    {
        palavraNode *a = inicio;
        printf("\n");
        while (true)
        {
            quantidade++;
            printf("%s ", a->data);
            a = a->prox;
            if (a == NULL)
            {
                break;
            }
        }
    }
    return quantidade;
}

char *obterPalavra(charNode *inicio)
{
    if (inicio != NULL)
    {
        char *buffer;
        if (inicio->prox != NULL)
        {
            buffer = calloc(32, sizeof(char));
            int pointer = 0;
            charNode *a = inicio;
            while (true)
            {
                buffer[pointer] = a->data;
                a = a->prox;
                pointer++;
                if (a == NULL)
                {
                    break;
                }
            }
        }
        else
        {
            buffer = calloc(1, sizeof(char));
            buffer[0] = inicio->data;
        }
        return buffer;
    }
    else
    {
        //"Dois espaços seguidos foram detectados";
        return NULL;
    }
}

charNode *inserirCharNode(charNode *inicio, char entrada)
{
    charNode *aux, *aux2;

    if (inicio == NULL)
    {
        inicio = (charNode *)malloc(sizeof(charNode));
        inicio->data = entrada;
        inicio->prox = NULL;
    }
    else
    {
        aux = inicio;
        while (aux->prox != NULL)
            aux = aux->prox;

        aux2 = (charNode *)malloc(sizeof(charNode));
        aux2->data = entrada;
        aux2->prox = NULL;
        aux->prox = aux2;
    }

    return inicio;
}

void *atualizarTempo()
{
    time(&tempoFim);
    tempoatual = (int)difftime(tempoFim, tempoInicio);
}

void *clockStart()
{
    time(&tempoInicio);
    while (tempoatual <= 60)
    {
        atualizarTempo();
    }
    rodando = false;
    return;
}

void *inputMain()
{
    charNode *inicioAtual = NULL;
    int pos = 0;
    while (rodando)
    {
        char a;
        a = getch();
        if (a == 32)
        {
            char *palavra = obterPalavra(inicioAtual);
            if (palavra != NULL)
            {
                inicioAtual = reset(inicioAtual);
                inicioGeral = inserirPalavraNode(inicioGeral, palavra);
            }
        }
        else
        {
            inicioAtual = inserirCharNode(inicioAtual, a);
        }
    }

    char *palavra = obterPalavra(inicioAtual);
    if (palavra != NULL)
    {
        inicioAtual = reset(inicioAtual);
        inicioGeral = inserirPalavraNode(inicioGeral, palavra);
    }
    return;
}

void *leituraArquivo(int texto)
{
    FILE *arq;
    if (texto == 1)
    {
        arq = fopen("digitacao1.txt", "r");
    }
    else
    {
        arq = fopen("digitacao2.txt", "r");
    }

    char *buffer = calloc(64, sizeof(char));
    int cont = 0;

    char chr = 0;
    while (1)
    {
        chr = fgetc(arq);
        if (chr == 32 || feof(arq))
        //|| chr == 12 || chr == '\n'
        {
            inicioComparacao = inserirPalavraNode(inicioComparacao, buffer);
            buffer = calloc(64, sizeof(char));
            cont = 0;
            if (feof(arq))
            {
                break;
            }
        }
        else
        {
            buffer[cont] = chr;
            cont++;
        }
    }

    fclose(arq);

    ///
    correcaoNode *inicioCorrecao = NULL;
    palavraNode *currentGeral = inicioGeral;
    palavraNode *currentGabarito = inicioComparacao;
    while (1)
    {
        inicioCorrecao = corrigirPalavra(inicioCorrecao, currentGeral->data, currentGabarito->data);
        currentGeral = currentGeral->prox;
        currentGabarito = currentGabarito->prox;

        if (currentGeral == NULL)
        {
            break;
        }
    }

    correcaoNode *ctc = inicioCorrecao;
    int acertos = 0;
    int erros = 0;
    printf("\n\n");
    while (1)
    {
        bool acertou = ctc->acertou;
        if (acertou)
        {
            printf("\033[0;32m");
            acertos++;
        }
        else
        {
            printf("\033[0;31m");
            erros++;
        }
        printf("%s ", ctc->data);
        ctc = ctc->prox;
        if (ctc == NULL)
        {
            break;
        }
    }
    printf("\033[0;32m");
    printf("\n\nSua velocidade é de %d palavras por minuto\nVocê digitou %d palavras erradas e %d palavras certas.\n", (acertos + erros), erros, acertos);
}

void *finishPos(void *textoSelecionado)
{
    //  int rate = leituraPalavras(inicioGeral);
    //  printf("\nVocê digita incriveis %d merdas por segundo\n", rate * 6);
    leituraArquivo((int)textoSelecionado);
}

int main()
{
    int *text;
    printf("Escolha o texto: (1) (2)\n");
    scanf("%d", &text);
    printf("\nGo!\n");
    tempoatual = 0;

    pthread_t principal, input, pos;
    int statusPrincipal, statusInput;

    statusPrincipal = pthread_create(&principal, NULL, clockStart, NULL);
    statusInput = pthread_create(&input, NULL, inputMain, NULL);

    if (statusPrincipal || statusInput)
    {
        printf("Deu merda\n");
        exit(1);
    }

    pthread_join(principal, NULL);
    pthread_cancel(input);
    pthread_create(&pos, NULL, finishPos, (void *)text);
    pthread_join(pos, NULL);

    return 0;
}