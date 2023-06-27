#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5
#define ESQ(id) (id-1)%N
#define DIR(id)	(id+1)%N
#define PENSANDO 0
#define QUER_COMER 1
#define COMENDO 2

sem_t s[N];
sem_t sem_talher[N];

int estado[N];

void *filosofos (void *arg);
void pega_talher (int n);
void devolve_talher (int n);

int main () {
	int i;
	int *id;

	for (i=0; i<N; i++) {
		sem_init(&sem_talher[i], 0, 1);
	}

	//semaforo 
	for (i=0; i<N; i++) {
		sem_init(&s[i], 0, 1);
	}
	pthread_t r[N];

	//criacao das threads de filosofos
	for (i = 0; i < N ; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&r[i], NULL, filosofos, (void*) (id));
	}

    pthread_join(r[0],NULL);
	return 0;
}


void *filosofos (void *arg) {
	int n = *((int *) arg);
	while(1) {
		//pensar
		printf("Filosofo %d pensando ...\n", n);
		sleep(3);
		pega_talher(n);
		//comer
		printf("\tFilosofo %d comendo ...\n", n);
		sleep(3);
        printf("\tFilosofo %d acabou de comer ...\n", n);
		devolve_talher(n);
	} 
}

void pega_talher (int n) {

	sem_wait(&sem_talher);
	estado[n] = QUER_COMER;

	if(n == 4){
	   sem_wait(&s[DIR(n)]);
	   sleep(1);
	   sem_wait(&s[ESQ(n)]);

	}else{
	   sem_wait(&s[ESQ(n)]);
	   sleep(1);
	   sem_wait(&s[DIR(n)]);
	}
	sem_post(&sem_talher);
}

void devolve_talher (int n) {
	sem_wait(&sem_talher);
	estado[n] = PENSANDO;

    sem_post(&s[ESQ(n)]);
    sem_post(&s[DIR(n)]);
    sem_post(&sem_talher);
}

