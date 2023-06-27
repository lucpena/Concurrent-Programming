#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_STATIONS 4
#define MAX_CAPACITY 10
#define INITIAL_PASSENGERS 50

pthread_mutex_t train_mutex;
pthread_cond_t station_cond;
int passengers[NUM_STATIONS] = {INITIAL_PASSENGERS, INITIAL_PASSENGERS, INITIAL_PASSENGERS, INITIAL_PASSENGERS};

void *train_thread(void *arg)
{
    int current_station = 0;
    int direction = 1;

    while (1)
    {
        printf("Train arrived at station %d\n", current_station);

        // Load passengers
        pthread_mutex_lock(&train_mutex);
        while (passengers[current_station] > 0 && passengers[current_station] <= MAX_CAPACITY)
        {
            passengers[current_station]--;
            printf("Passenger boarded at station %d\n", current_station);
        }
        pthread_mutex_unlock(&train_mutex);

        // Simulate train travel time
        sleep(2);

        // Unload passengers
        pthread_mutex_lock(&train_mutex);
        while (passengers[current_station] < INITIAL_PASSENGERS)
        {
            passengers[current_station]++;
            printf("Passenger disembarked at station %d\n", current_station);
        }
        pthread_mutex_unlock(&train_mutex);

        // Update current station and direction
        if (current_station == 0 && direction == -1)
            direction = 1;
        else if (current_station == NUM_STATIONS - 1 && direction == 1)
            direction = -1;

        current_station += direction;
        usleep(500000); // Sleep for 500 milliseconds before moving to the next station

        // Signal the station thread
        pthread_mutex_lock(&train_mutex);
        pthread_cond_signal(&station_cond);
        pthread_mutex_unlock(&train_mutex);
    }

    return NULL;
}

void *station_thread(void *arg)
{
    int station_id = *(int *)arg;

    while (1)
    {
        pthread_mutex_lock(&train_mutex);
        while (passengers[station_id] > 0 && passengers[station_id] <= MAX_CAPACITY)
        {
            pthread_cond_wait(&station_cond, &train_mutex);
        }
        pthread_mutex_unlock(&train_mutex);
    }

    return NULL;
}

int main()
{
    pthread_t train_tid;
    pthread_t station_tid[NUM_STATIONS];
    int station_ids[NUM_STATIONS];

    pthread_mutex_init(&train_mutex, NULL);
    pthread_cond_init(&station_cond, NULL);

    // Create train thread
    pthread_create(&train_tid, NULL, train_thread, NULL);

    // Create station threads
    for (int i = 0; i < NUM_STATIONS; i++)
    {
        station_ids[i] = i;
        pthread_create(&station_tid[i], NULL, station_thread, &station_ids[i]);
    }

    // Wait for train thread to finish (this will never happen as the train runs forever)
    pthread_join(train_tid, NULL);

    // Clean up
    pthread_mutex_destroy(&train_mutex);
    pthread_cond_destroy(&station_cond);

    return 0;
}
