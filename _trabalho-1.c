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
pthread_cond_t station_cv;         // Condition variable for train travel

int train_capacity = 0;
int random_number = 0;
int current_station = 0;

void *train_thread(void *arg)
{
    int i;

    while (1)
    {
        // Travel from the first to the last station
        for (i = 0; i < NUM_STATIONS; i++)
        {
            pthread_mutex_lock(&train_mutex);

            pthread_mutex_lock(&train_current_station);
            current_station = i;
            pthread_mutex_unlock(&train_current_station);

            printf("\n\n\tO Trem chegou na estacao %d com %d passageiros.\n\n", i, train_capacity);

            // Wait until the train is at capacity
            while (train_capacity < MAX_CAPACITY)
            {
                pthread_cond_wait(&train_capacity_cv, &train_mutex);
            }

            // Viajando
            printf("\nO trem esta indo para a proxima estacao...\n");
            sleep(2);

            // Unload passengers
            train_capacity -= (rand() % 5) + 5;            

            // Signal the station thread to continue
            pthread_cond_signal(&train_travel_cv);
            pthread_mutex_unlock(&train_mutex);
        }

        // Travel from the last to the first station
        for (i = NUM_STATIONS - 2; i > 0; i--)
        {
            pthread_mutex_lock(&train_mutex);

            pthread_mutex_lock(&train_current_station);
            current_station = i;
            pthread_mutex_unlock(&train_current_station);

            printf("\n\n\tO Trem chegou na estacao %d com %d passageiros.\n\n", i, train_capacity);

            // Wait until the train is at capacity
            while (train_capacity < MAX_CAPACITY)
            {
                pthread_cond_wait(&train_capacity_cv, &train_mutex);
            }

            // Viajando
            printf("\nO trem esta indo para a proxima estacao...\n");
            sleep(2);

            // Unload passengers
            train_capacity -= (rand() % 5) + 5;

            // Signal the station thread to continue
            pthread_cond_signal(&train_travel_cv);
            pthread_mutex_unlock(&train_mutex);
        }
    }
}

// Function executed by the station threads
void *station_thread(void *arg)
{
    int station_num = *((int *)arg);
    int station_waiting = STATION;

    while (1)
    {
        pthread_mutex_lock(&train_mutex);

        pthread_mutex_lock(&train_current_station);
        int this_current_station = current_station;
        pthread_mutex_unlock(&train_current_station);

        // Wait until the train is at the station
        while (train_capacity == MAX_CAPACITY)        
            pthread_cond_wait(&train_travel_cv, &train_mutex);
        

        if( station_waiting != 0 )
        {
            train_capacity++;
            station_waiting--;
            printf("\tPessoas no Trem: %d\n", train_capacity);
            printf("\tPessoas na estação %d: %d\n", current_station, station_waiting);
        }

        // If the train reaches maximum capacity, signal it
        if (train_capacity == MAX_CAPACITY || station_waiting == 0)        
            pthread_cond_signal(&train_capacity_cv);
        

        pthread_mutex_unlock(&train_mutex);
    }
}

int main()
{
    pthread_t train_tid;
    pthread_t station_tid[NUM_STATIONS];
    int station_args[NUM_STATIONS];

    // Initialize mutex and condition variables
    pthread_mutex_init(&train_mutex, NULL);
    pthread_mutex_init(&train_current_station, NULL);
    pthread_mutex_init(&station_mutex, NULL);

    pthread_cond_init(&train_capacity_cv, NULL);
    pthread_cond_init(&train_travel_cv, NULL);
    pthread_cond_init(&station_cv, NULL);

    // Create train thread
    pthread_create(&train_tid, NULL, train_thread, NULL);

    //Create station threads
    for (int i = 0; i < NUM_STATIONS; i++)
    {
        station_args[i] = i;
        pthread_create(&station_tid[i], NULL, station_thread, &station_args[i]);
    }

    // Join train thread
    pthread_join(train_tid, NULL);

    // Join station threads
    for (int i = 0; i < NUM_STATIONS; i++)
    {
        pthread_join(station_tid[i], NULL);
    }

    return 0;
}
