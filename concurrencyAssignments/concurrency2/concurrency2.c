/* CS444 - Concurrency2
 * Drake Seifert & Scott Merrill
 * Source: http://pseudomuto.com/development/2014/03/01/dining-philosophers-in-c/
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define true 1
#define false 0
#define ENABLE_SLEEP true

typedef struct {
  int position;
  int count;
  sem_t *forks;
  sem_t *lock;
} params_t;

void initialize_semaphores(sem_t *lock, sem_t *forks, int num_forks);
void run_all_threads(pthread_t *threads, sem_t *forks, sem_t *lock, int num_philosophers);

void *philosopher(void *params);
void think(int position);
void eat(int position);

char name[5][20];

int main(int argc, char *args[])
{
  srand(time(NULL));
  int num_philosophers = 5;
  strcpy(name[0], "Alan Watts");
  strcpy(name[1], "Descartes");
  strcpy(name[2], "Socrates");
  strcpy(name[3], "Plato");
  strcpy(name[4], "Aristotle");

  sem_t lock;
  sem_t forks[num_philosophers];
  pthread_t philosophers[num_philosophers];

  initialize_semaphores(&lock, forks, num_philosophers);
  run_all_threads(philosophers, forks, &lock, num_philosophers);
  pthread_exit(NULL);
}

void initialize_semaphores(sem_t *lock, sem_t *forks, int num_forks)
{
  int i;
  for(i = 0; i < num_forks; i++) {
    sem_init(&forks[i], 0, 1);
  }

  sem_init(lock, 0, num_forks - 1);
}

void run_all_threads(pthread_t *threads, sem_t *forks, sem_t *lock, int num_philosophers)
{
  int i;
  for(i = 0; i < num_philosophers; i++) {
    params_t *arg = malloc(sizeof(params_t));
    arg->position = i;
    arg->count = num_philosophers;
    arg->lock = lock;
    arg->forks = forks;

    pthread_create(&threads[i], NULL, philosopher, (void *)arg);
  }
}

void *philosopher(void *params)
{
  params_t self = *(params_t *)params;

  while(true) {

    //Think
    think(self.position);

    //Pick up fork
    sem_wait(self.lock);
    sem_wait(&self.forks[self.position]);
    printf("%s picked up fork %d\n", name[self.position], self.position);
    sem_wait(&self.forks[(self.position + 1) % self.count]);
    printf("%s picked up fork %d\n", name[self.position], self.position + 1);

    //Eat
    eat(self.position);

    //Put down fork
    sem_post(&self.forks[self.position]);
    printf("%s put down fork %d\n", name[self.position], self.position);
    sem_post(&self.forks[(self.position + 1) % self.count]);
    printf("%s put down fork %d\n", name[self.position], self.position + 1);
    sem_post(self.lock);
  }

  think(self.position);
  pthread_exit(NULL);
}

void think(int position)
{
  int wait_time = rand() % 20 + 1;
  printf("Philosopher %s thinking for %d seconds\n", name[position], wait_time);
  if(ENABLE_SLEEP)
    sleep(wait_time);
}

void eat(int position)
{
  int wait_time = rand() % 8 + 2;
  printf("Philosopher %s eating for %d seconds\n", name[position], wait_time);
  if(ENABLE_SLEEP)
    sleep(wait_time);
}
