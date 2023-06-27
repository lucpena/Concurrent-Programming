#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define MAX_USUARIOS 10 //número de usuários
#define MAX_CARTAS 20 //quantidade de cartas na mochila

sem_t usuario;
sem_t pombo;
sem_t mochila;

int cartasNaMochila = 0;

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i;
    
	sem_init(&usuario, 0, MAX_CARTAS);
	sem_init(&mochila, 0, 1);
    sem_init(&pombo, 0, 0);

    pthread_t usuario[MAX_USUARIOS];
    int *id;
    for(i = 0; i < MAX_USUARIOS; i++){
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&(usuario[i]),NULL,f_usuario, (void *) (id));
    }
	pthread_t pombo;
	id = (int *) malloc(sizeof(int));
	*id = 0;
	pthread_create(&(pombo),NULL,f_pombo, (void*) (id));

	pthread_join(pombo,NULL);
   
}


void * f_pombo(void *arg){
  
    while(1){
        //Inicialmente está em A, aguardar/dorme a mochila ficar cheia (20 cartas)
        sem_wait(&pombo);        
        //Leva as cartas para B e volta para A
        printf("\tPombo esta levando as cartas...\n");
        sleep(5);
        cartasNaMochila = 0;
        //Acordar os usuários   
        for(int i = 0; i < MAX_CARTAS; i++)
        {
        	sem_post(&usuario);
        }
    }
}

void * f_usuario(void *arg){
    
    while(1)
    {	
    	sem_wait(&usuario);
		//Escreve uma carta
		//printf("Usuario esta escrevendo uma carta.\n");
		sleep(rand() % 5 );
		//Caso o pombo não esteja em A ou a mochila estiver cheia, então dorme	
		sem_wait(&mochila);
		
		cartasNaMochila++;
		printf("Cartas na mochila: %d\n", cartasNaMochila);    
		if( cartasNaMochila == MAX_CARTAS )
		{
			sem_post(&pombo);
		}	
	
		sem_post(&mochila);
		//Posta sua carta na mochila do pombo	
		
	}
}
