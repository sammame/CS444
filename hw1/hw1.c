/*
 * CS444 HW1 - Concurrency 1: The Producer-Consumer Problem
 * Drake Seifert
 * Scott Merrill
 */

#include <stdio.h>  //printf, scanf, NULL, ...
#include <stdlib.h> //srand, rand
#include <time.h>   //time
#include <unistd.h> //sleep
#include <string.h> //memset
#include <pthread.h>

#define false 0
#define true 1
#define BUFFER_CAPACITY 32
#define LOOP_COUNT 500      //Set how long the program runs
#define THREAD_COUNT 2	    //Number of producer and consumer threads
#define ENABLE_SLEEP false  //turn sleep on or off for testing

struct item {
	int value;
	int wait;
};

//Global variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t prod_cond, cons_cond;

struct item buffer[BUFFER_CAPACITY];
int buffer_size = 0;
int buffer_in_use = false;
int buffer_is_full = false;
int buffer_is_empty = true;

void* producer(void* arg)
{
	/*TODO: implement randomness with rdrand x86 ASM and
			Mersenne Twister*/

	//Extract and dereference parameter value
	int *thread_num_ptr = (int*) arg;
	int thread_num = *thread_num_ptr;

	for(int i = 0; i < LOOP_COUNT; i++) {

		//start critical section
		pthread_mutex_lock(&mutex);

		//wait if buffer is in use
		while(buffer_in_use || buffer_is_full) {
			pthread_cond_wait(&prod_cond, &mutex);
		}

		//place random value and waiting period in buffer
		buffer_in_use = true;
		buffer[buffer_size].value = rand() % 1000;
		buffer[buffer_size].wait = rand() % 8 + 2;

		//change buffer values
		buffer_size++;
		if(buffer_size == BUFFER_CAPACITY) {
			buffer_is_full = true;
			buffer_is_empty = false;
		} else {
			buffer_is_full = false;
			buffer_is_empty = false;
		}
		printf("PRODUCER #%d: Buffer size is %d\n", thread_num, buffer_size);
		buffer_in_use = false;

		//signal consumer
		pthread_cond_signal(&cons_cond);

		//end critical section
		pthread_mutex_unlock(&mutex);

		//wait for 3 to 7 seconds
		int wait_time = rand() % 5 + 3;
		printf("PRODUCER #%d: Waiting for %d seconds\n",
			    thread_num, wait_time);
		if(ENABLE_SLEEP)
			sleep(wait_time);

	}

	pthread_exit(0);
}

void* consumer(void* arg)
{	
	//Extract and dereference parameter value
	int *thread_num_ptr = (int*) arg;
	int thread_num = *thread_num_ptr;

	int wait_time;

	for(int i = 0; i < LOOP_COUNT; i++) {

		//start critical section
		pthread_mutex_lock(&mutex);

		//wait if buffer is in use
		while(buffer_in_use || buffer_is_empty) {
			pthread_cond_wait(&cons_cond, &mutex);
		}

		//print value from buffer
		buffer_in_use = true;
		printf("CONSUMER #%d: My value is %d\n", 
			   thread_num, buffer[buffer_size - 1].value);
		wait_time = buffer[buffer_size - 1].wait;

		//change buffer values
		buffer_size--;
		if(buffer_size == 0) {
			buffer_is_empty = true;
			buffer_is_full = false;
		} else {
			buffer_is_empty = false;
			buffer_is_full = false;
		}
		printf("CONSUMER #%d: Buffer size is %d\n", thread_num, buffer_size);
		buffer_in_use = false;

		//signal producer
		pthread_cond_signal(&prod_cond);

		//end critical section
		pthread_mutex_unlock(&mutex);

		//wait for 2 to 9 seconds
		printf("CONSUMER #%d: Waiting for %d seconds\n", 
			   thread_num, wait_time);
		if(ENABLE_SLEEP)
			sleep(wait_time);

	}

	pthread_exit(0);
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	//clear buffer
	memset(buffer, '\0', sizeof(buffer));

	//init condition variables
	pthread_cond_init(&prod_cond, NULL);
	pthread_cond_init(&cons_cond, NULL);

	//create thread IDs
	pthread_t prod_tid[THREAD_COUNT], cons_tid[THREAD_COUNT];

	//begin threads
	for(int i = 0; i < THREAD_COUNT; ++i) {
		pthread_create(&prod_tid[i], NULL, producer, &i);
	}

	for (int i = 0; i < THREAD_COUNT; ++i) {
		pthread_create(&cons_tid[i], NULL, consumer, &i);
	}

	//wait for threads
	for(int i = 0; i < THREAD_COUNT; ++i) {
		pthread_join(prod_tid[i], NULL);
	}

	for(int i = 0; i < THREAD_COUNT; ++i) {
		pthread_join(cons_tid[i], NULL);
	}

	//free mutex and condition variables
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&prod_cond);
	pthread_cond_destroy(&cons_cond);

	printf("\nConcurrency program has ended. Goodbye :)\n\n");

	return 0;
}