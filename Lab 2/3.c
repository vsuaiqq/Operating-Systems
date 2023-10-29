#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_OF_PHILOSOPHERS 5
#define THINKING_TIME 1
#define EATING_TIME 2

sem_t forks[NUM_OF_PHILOSOPHERS];

void* philosopher(void* arg)
{
    int id = *(int*)arg;
    int left_fork = id, right_fork = (id + 1) % NUM_OF_PHILOSOPHERS;

    while (1)
    {
        printf("philosopher %d is thinking\n", id);
        sleep(THINKING_TIME);
        printf("philosopher %d is hungry\n", id);

        sem_wait(&forks[left_fork]);
        printf("philosopher %d takes left fork\n", id);

        sem_wait(&forks[right_fork]);
        printf("philosopher %d takes right fork\n", id);

        printf("philosopher %d is eating\n", id);
        sleep(EATING_TIME);

        sem_post(&forks[left_fork]);
        sem_post(&forks[right_fork]);
    }
}

int main()
{
    pthread_t philosophers[NUM_OF_PHILOSOPHERS];
    int philosophers_ids[NUM_OF_PHILOSOPHERS] = {0, 1, 2, 3, 4};

    for (int i = 0; i < NUM_OF_PHILOSOPHERS; ++i)
    {
        sem_init(&forks[i], 0, 1);
    }
    for (int i = 0; i < NUM_OF_PHILOSOPHERS; ++i)
    {
        pthread_create(&philosophers[i], NULL, philosopher, &philosophers_ids[i]);
    }
    for (int i = 0; i < NUM_OF_PHILOSOPHERS; ++i)
    {
        pthread_join(philosophers[i], NULL);
    }
    for (int i = 0; i < NUM_OF_PHILOSOPHERS; ++i)
    {
        sem_destroy(&forks[i]);
    }

    return 0;
}