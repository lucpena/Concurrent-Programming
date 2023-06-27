/*

    Trabalho 1 de Programacao Concorrente

    Aluno: Lucas Araujo Pena
    Matricula: 130056162

    Descricao: O problema proposto para ser resolvido neste trabalho se baseia na
    ideia de um trem que visita estacoes, pegando novos passageiros e deixando
    outros. Ele ira ate a ultima estacao, e depois voltar visitando todas as
    estacoes ate o comeco. Em cada estacao sera deixada um numero aleatorio de
    passageiros, e cada estacao comeca com um numero determinado de passageiro,
    mas nenhum novo passageiro ira chegar nestas estacoes alem do numero especi-
    ficado. Caso as estacoes estejam vazias, o trem ira continuar sua viagem para
    a proxima indefinidadmente.

    O programa utiliza uma thread para o trem e outra tread que gerencia as
    estacoes.

    Como usar: O programa foi desenvolvido em ambiente Linux Mint (Ubuntu) e
    compildao utilizando o GCC com o seguinte comando:

            gcc -pthread _trabalho-1.c -o t

    E compilado com: 
            
            ./t

    

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_STATIONS 4                  // Numero de estacoes
#define MAX_CAPACITY 25                 // Capacidade maxima do trem
#define STATION 40// + (rand() % 50)      // Numero de passageiros na estacao

pthread_mutex_t train_mutex;            // Mutex para a sincronização do trem
pthread_mutex_t train_current_station;  // Mutex para a sincronização do trem

pthread_cond_t train_capacity_cv;       // Variável de condição para a capacidade
pthread_cond_t train_travel_cv;         // Condition variable for train travel

int train_capacity = 0;                 // Numero de passageiros dentro do trem           
int current_station = 0;                // Estacao em que o trem se encontra

// Array onde contem o numero de passageiros em cada estacao: 
// Exemplo: station_waiting[0] == Passageiros na est. 0
int station_waiting[NUM_STATIONS];

// Thread que gerencia o trem
void *train_thread(void *arg)
{
    int direction = 1;

    while (1)
    {
        pthread_mutex_lock(&train_mutex);

        printf("\nO Trem chegou na estacao [[  %d  ]]", current_station);
        if(station_waiting[current_station] == 0 ) printf(" ( VAZIA )");
        printf("\nAssentos vagos: %d\nPassageiros no trem: %d\n", MAX_CAPACITY - train_capacity, train_capacity);
        printf("---------------------------------------------\n");

        // Espera ate o trem estiver cheio ou a estacao estiver vazia
        while (train_capacity < MAX_CAPACITY && station_waiting[current_station] > 0)
        {
            pthread_cond_wait(&train_capacity_cv, &train_mutex);
        }

        // Viajando
        printf("\n\nO trem esta indo para a proxima estacao...\n\n");
        sleep(2);
        printf("---------------------------------------------");

        // Passageiros descendo
        train_capacity -= (rand() % (train_capacity + 1));

        // Define a direcao do trem
        if (current_station == 0 && direction == -1)
            direction = 1;
        else if (current_station == NUM_STATIONS - 1 && direction == 1)
            direction = -1;

        // Altera a direcao do trem
        pthread_mutex_lock(&train_current_station);
        current_station += direction;
        pthread_mutex_unlock(&train_current_station);

        // Sinal para a thread das estacoes
        pthread_cond_signal(&train_travel_cv);
        pthread_mutex_unlock(&train_mutex);
    }
}

// Funcao que gerencia as estacoes
void *station_thread(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&train_mutex);

        // Espera o trem chegar na estacao
        while (train_capacity == MAX_CAPACITY)
        {
            pthread_cond_wait(&train_travel_cv, &train_mutex);
        }         

        // Passageiros embarcando e desembarcando
        if (station_waiting[current_station] != 0)
        {
            printf("\tPessoas na estacao %d: %d\n", current_station, station_waiting[current_station]);
            train_capacity++;
            station_waiting[current_station]--;
            printf("\tPessoas no Trem: %d\n", train_capacity);
            usleep( 200000 );
        }

        // Se o trem ficar cheio ou se a estacao estiver vazia, prossegue 
        if (train_capacity == MAX_CAPACITY || station_waiting[current_station] == 0)
        {
            pthread_cond_signal(&train_capacity_cv);
        }

        pthread_mutex_unlock(&train_mutex);
    }
}

int main()
{
    pthread_t train_tid;
    pthread_t station_tid;
    int station_args[NUM_STATIONS];

    // Inicializacao dos mutex
    pthread_mutex_init(&train_mutex, NULL);
    pthread_mutex_init(&train_current_station, NULL);

    pthread_cond_init(&train_capacity_cv, NULL);
    pthread_cond_init(&train_travel_cv, NULL);

    for (int i = 0; i < NUM_STATIONS; i++)
    {
        station_waiting[i] = STATION;
    }

    // Cria a thread do trem
    pthread_create(&train_tid, NULL, train_thread, NULL);

    // Cria a thread das estacoes
    pthread_create(&station_tid, NULL, station_thread, NULL);
    
    pthread_join(train_tid, NULL);
    pthread_join(station_tid, NULL);    

    return 0;
}
