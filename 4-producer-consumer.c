#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PRODUTOR_MAX    1 
#define CONSUMIDOR_MAX  1
#define BUFFER_MAX      5

void *produtor(void *meuid);
void *consumidor(void *meuid);

pthread_mutex_t mutex;
pthread_mutex_t empty;
pthread_mutex_t full;

unsigned short buffer;

void main(int argc, char *argv[])
{

    int erro;
    int i, n, m;
    int *id;

    pthread_t tid[PRODUTOR_MAX];

    for (i = 0; i < PRODUTOR_MAX; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        erro = pthread_create(&tid[i], NULL, produtor, (void *)(id));

        if (erro)
        {
            printf("erro na criacao do thread %d\n", i);
            exit(1);
        }
    }

    pthread_t tCid[CONSUMIDOR_MAX];

    for (i = 0; i < CONSUMIDOR_MAX; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;int argc; 
        erro = pthread_create(&tCid[i], NULL, consumidor, (void *)(id));

        if (erro)
        {
            printf("erro na criacao do thread %d\n", i);
            exit(1);
        }
    }

    pthread_join(tid[0], NULL);
}

void *produtor(void *id)
{
    unsigned int myid = *((int*) id);
    while (1)
    {
        pthread_mutex_lock(&mutex);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

void *consumidor(void *id)
{
    while (1)
    {

    }
    pthread_exit(0);
}