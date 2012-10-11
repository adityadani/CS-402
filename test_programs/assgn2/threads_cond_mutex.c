#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t count_mutex;
pthread_cond_t count_cond;
int count = 0;

void *increament_count(void *id)
{
	int i=0;
	long thread_id = (long)id;

	printf("\n In increament thread %ld ",thread_id);
	for(i=0; i< 15; i++) {
		pthread_mutex_lock(&count_mutex);
		count++;
		if(count == 15) {
			printf("\n Increament Thread : %ld , Condition Reached , Signalling!!!!",thread_id);
			pthread_cond_signal(&count_cond);
		}
		printf("\n Increament Thread : %ld , Count = %d ",thread_id,count);
		pthread_mutex_unlock(&count_mutex);
		sleep(1);
	}
	pthread_exit(NULL);
}

void *watch_count(void *id)
{
	long thread_id = (long)id;
	printf("\n In watch thread : %ld",thread_id);
	while(count < 15) {
		pthread_mutex_lock(&count_mutex);
		pthread_cond_wait(&count_cond, &count_mutex);
		printf("\n Watch Thread : %ld , Condition Signal Received!!!", thread_id);
		count +=1200;
		printf("\n Watch Thread : %ld , Count = %d", thread_id, count);
	}
	pthread_mutex_unlock(&count_mutex);
	pthread_exit(NULL);
}

int main()
{
	pthread_t count_thread[3];
	long t1,t2,t3,i;
	t1=1;
	t2=2;
	t3=3;

	pthread_mutex_init(&count_mutex,NULL);
	pthread_cond_init(&count_cond,NULL);
	
	printf("\nCreating three threads!!!");
	pthread_create(&count_thread[0],NULL,watch_count,(void *)t1);
	pthread_create(&count_thread[1],NULL,increament_count,(void *)t2);
	pthread_create(&count_thread[2],NULL,increament_count,(void *)t3);
	printf("\n All three threads created");

	for(i=0;i<3;i++) {
		pthread_join(count_thread[i],NULL);
	}
	printf("\nMain Thread : Done");
	pthread_mutex_destroy(&count_mutex);
	pthread_cond_destroy(&count_cond);
	pthread_exit(NULL);
}
