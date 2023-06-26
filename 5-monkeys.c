#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MACACOS_AB 10 // macacos que andam de A para B
#define MACACOS_BA 10 // macacos que andam de B para A
#define GORILAS     3

pthread_mutex_t vez;
pthread_mutex_t corda;
pthread_mutex_t AB;
pthread_mutex_t BA;

unsigned char passandoPelaCordaAB = 0;
unsigned char passandoPelaCordaBA = 0;

void *macacoAB(void *id)
{
    int i = *((int *)id);
    while (1)
    {
        printf("Macaco %d quer passar de A para B. \n", i);

        pthread_mutex_lock(&vez);                   // Sincronizando com a outra thread
            pthread_mutex_lock(&AB);                // Somente um macaco de A para B acessa

                passandoPelaCordaAB++;
                if(passandoPelaCordaAB == 1)
                    pthread_mutex_lock(&corda);     // Acessando a corda

            pthread_mutex_unlock(&AB);
        pthread_mutex_unlock(&vez);

        printf("\tMacaco %d passado de A para B. \n", i);
        sleep(1);

        pthread_mutex_lock(&AB);

            passandoPelaCordaAB--;
            if(passandoPelaCordaAB == 0 )
                pthread_mutex_unlock(&corda);

        pthread_mutex_unlock(&AB);

    }
    pthread_exit(0);
}

void *macacoBA(void *id)
{
    int i = *((int *)id);
    while (1)
    {
        printf("Macaco %d quer passar de B para A. \n", i);
        // Procedimentos para acessar a corda
         pthread_mutex_lock(&vez);                   // Sincronizando com a outra thread
            pthread_mutex_lock(&BA);                // Somente um macaco de A para B acessa

                passandoPelaCordaBA++;
                if(passandoPelaCordaBA == 1)
                    pthread_mutex_lock(&corda);     // Acessando a corda

            pthread_mutex_unlock(&BA);
        pthread_mutex_unlock(&vez);

        printf("\tMacaco %d passado de B para A \n", i);
        sleep(1);

        pthread_mutex_lock(&BA);

            passandoPelaCordaBA--;
            if(passandoPelaCordaBA == 0 )
                pthread_mutex_unlock(&corda);

        pthread_mutex_unlock(&BA);
    }
    pthread_exit(0);
}

void *gorila(void *id)
{
    int i = *((int *)id);
    while (1)
    {
        printf("Gorila %d quer passar.\n", i);
        pthread_mutex_lock(&vez);
        pthread_mutex_lock(&corda);
            printf("\tGorila %d passando...\n", i);
            sleep(5);
        pthread_mutex_unlock(&corda);
        pthread_mutex_unlock(&vez);
        sleep(1);
    }
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    pthread_t macacos[MACACOS_AB + MACACOS_BA];
    pthread_t gorilas[GORILAS];

    int *id;
    int i = 0;

    for (i = 0; i < MACACOS_AB + MACACOS_BA; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        if (i % 2 == 0)
        {
            if (pthread_create(&macacos[i], NULL, macacoAB, (void *)id))
            {
                printf("Não pode criar a thread %d\n", i);
                return -1;
            }
        }
        else
        {
            if (pthread_create(&macacos[i], NULL, macacoBA, (void *)id))
            {
                printf("Não pode criar a thread %d\n", i);
                return -1;
            }
        }
    }

    for (i = 0; i < GORILAS ; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&gorilas[i], NULL, gorila, (void *) (id));
	}

    pthread_join(macacos[0], NULL);
    return 0;
}
