#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

pthread_cond_t woman_cond;
pthread_cond_t man_cond;

pthread_mutex_t mutex;

int people_in_bathroom = 0;
char cur_gender = 0;
int N;

void woman_wants_to_enter()
{
    pthread_mutex_lock(&mutex);
    while (cur_gender == 'M' || people_in_bathroom == N)
    {
        pthread_cond_wait(&woman_cond, &mutex);
    }
    printf("woman enters\n");
    cur_gender = 'F';
    printf("door indicator: %c\n", cur_gender);
    ++people_in_bathroom;
    printf("num of people in bathroom: %d\n\n", people_in_bathroom);
    pthread_mutex_unlock(&mutex);
}

void man_wants_to_enter()
{
    pthread_mutex_lock(&mutex); 
    while (cur_gender == 'F' || people_in_bathroom == N)
    {
        pthread_cond_wait(&man_cond, &mutex);
    }
    printf("man enters\n");
    cur_gender = 'M';
    printf("door indicator: %c\n", cur_gender);
    ++people_in_bathroom;
    printf("num of people in bathroom: %d\n\n", people_in_bathroom);
    pthread_mutex_unlock(&mutex);
}

void woman_leaves()
{
    pthread_mutex_lock(&mutex);
    --people_in_bathroom;
    if (people_in_bathroom == 0)
    {
        cur_gender = 0;
        pthread_cond_broadcast(&man_cond);
    }
    printf("woman leaves\n");
    printf("num of people in bathroom: %d\n\n", people_in_bathroom);
    pthread_mutex_unlock(&mutex);
}

void man_leaves()
{
    pthread_mutex_lock(&mutex);
    --people_in_bathroom;
    if (people_in_bathroom == 0)
    {
        cur_gender = 0;
        pthread_cond_broadcast(&woman_cond);
    }
    printf("man leaves\n");
    printf("num of people in bathroom: %d\n\n", people_in_bathroom);
    pthread_mutex_unlock(&mutex);
}

void* man_thread(void* arg)
{
    int num_of_men = *((int*)arg);
    for (int i = 0; i < num_of_men; ++i)
    {
        man_wants_to_enter();
        man_leaves();
    }
    return NULL;
}

void* woman_thread(void* arg)
{
    int num_of_women = *((int*)arg);
    for (int i = 0; i < num_of_women; ++i)
    {
        woman_wants_to_enter();
        woman_leaves();
    }
    return NULL;
}

void simulate_bathroom(const int num_of_men, const int num_of_women)
{
    pthread_t men[num_of_men];
    pthread_t women[num_of_women];
    int enter = 1;

    for (int i = 0, j = 0; i < num_of_men, j < num_of_women; ++i, ++j)
    {
        if (i >= 0) pthread_create(&men[i], NULL, man_thread, &enter);
        if (j >= 0) pthread_create(&women[i], NULL, woman_thread, &enter);
    }
    for (int i = 0, j = 0; i < num_of_men, j < num_of_women; ++i, ++j)
    {
        if (i >= 0) pthread_join(men[i], NULL);
        if (j >= 0) pthread_join(women[i], NULL);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("invalid number of arguments\n");
        return 1;
    }
    if (!sscanf(argv[1], "%d", &N)) 
    {
        printf("invalid parameter\n");
        return 1;
    }

    int num_of_men, num_of_women;
    printf("enter number of men who want to go bathroom: ");
    if(!scanf("%d", &num_of_men)) 
    {
        printf("invalid input\n");
        return 1;
    }
    printf("enter number of women who want to go bathroom: ");
    if(!scanf("%d", &num_of_women)) 
    {
        printf("invalid input\n");
        return 1;
    }
    printf("\n");

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&woman_cond, NULL);
    pthread_cond_init(&man_cond, NULL);

    simulate_bathroom(num_of_men, num_of_women);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&woman_cond);
    pthread_cond_destroy(&man_cond);

    return 0;
}