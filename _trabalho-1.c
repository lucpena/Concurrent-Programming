#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_STATIONS 4
#define MAX_CAPACITY 10
#define STATION 20 //+ (rand() % 50)

pthread_mutex_t train_mutex;            // Mutex para a sincronização do trem
pthread_mutex_t train_current_station;  // Mutex para a sincronização do trem
pthread_mutex_t station_mutex;          // Mutex para a sincronização do trem

pthread_cond_t train_capacity_cv;       // Variável de condição para a capacidade
pthread_cond_t train_travel_cv;         // Condition variable for train travel

int train_capacity = 0;
int random_number = 0;
int current_station = 0;

int station_waiting[NUM_STATIONS];

void *train_thread(void *arg)
{
    int direction = 1;

    while (1)
    {
        // Travel from the first to the last station
        pthread_mutex_lock(&train_mutex);

        printf("\n\n\tO Trem chegou na estacao %d com %d passageiros.\n\n", current_station, train_capacity);

        // Wait until the train is at capacity
        while (train_capacity < MAX_CAPACITY)
        {
            pthread_cond_wait(&train_capacity_cv, &train_mutex);
        }

        // Viajando
        printf("\nO trem esta indo para a proxima estacao...\n");
        sleep(1);

        // Unload passengers
        train_capacity -= (rand() % 5) + 5;

        // Update current station and direction
        if (current_station == 0 && direction == -1)
            direction = 1;
        else if (current_station == NUM_STATIONS - 1 && direction == 1)
            direction = -1;

        pthread_mutex_lock(&train_current_station);
        current_station += direction;
        pthread_mutex_unlock(&train_current_station);

        // Signal the station thread to continue
        pthread_cond_signal(&train_travel_cv);
        pthread_mutex_unlock(&train_mutex);
    }
}

// Function executed by the station threads
void *station_thread(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&train_mutex);

        pthread_mutex_lock(&train_current_station);
        int this_current_station = current_station;
        pthread_mutex_unlock(&train_current_station);

        // Wait until the train is at the station
        printf("\nGeneral Kenobi\n");
        while (train_capacity == MAX_CAPACITY)
        {
            pthread_cond_wait(&train_travel_cv, &train_mutex);
        }



        if (station_waiting[this_current_station] != 0)
        {
            printf("\tPessoas na estação %d: %d\n", current_station, station_waiting[this_current_station]);
            printf("\tPessoas no Trem: %d\n", train_capacity);
            train_capacity++;
            station_waiting[this_current_station]--;
        }

        // If the train reaches maximum capacity, signal it
        if (train_capacity == MAX_CAPACITY || station_waiting[this_current_station] == 0)
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
    pthread_mutex_init(&station_mutex, NULL);

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
