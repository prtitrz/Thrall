#define THREAD 128
#define QUEUE 256

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include "debug.h"
#include "threadpool.h"
#define MAXREC 10

struct record {
	int dev_num;
	long offset;
	long length;
	char op;
	struct timeval time;
}records[MAXREC];

struct timeval begin_time;
int late = 0;
pthread_mutex_t lock;

int init_rec_array(struct record *records)
{
	FILE *fp;
	int i;
	double time;
	char tmp[20];

	fp = fopen("./tmpfile", "r+");
	if (fp == NULL) {
		debug_puts("RECORD FILE OPEN ERROR");
		return 1;
	}

	for (i = 0; i < MAXREC; i++) {
		fscanf(fp, "%d,%ld,%ld,%c,%lf", &(records[i]).dev_num, &records[i].offset, \
				&records[i].length, &records[i].op, &time);
		sprintf(tmp, "%lf", time);
		sscanf(tmp, "%ld.%ld", &records[i].time.tv_sec, \
				&records[i].time.tv_usec);
	}
	
	fclose(fp);
	return 0;
}
/*
void trace_replay(void *arg)
{
	struct timeval end_time, result_time;
	double test;
	int i = *(int *)arg;

	gettimeofday(&end_time, NULL);
	timersub(&end_time, &begin_time, &result_time);
	test = records[i].time - (result_time.tv_sec + );
	if (test > 1 || test < -1) {
		debug_print("%d, TIME_ERROR", i);
	}
	test = result_time.tv_sec -  + result_time.tv_usec * 0.000001 - records[i].time;
	if (test > 0) {
		usleep(test);
	}
	else {
		pthread_mutex_lock(&lock);
		late = late + i;
		pthread_mutex_unlock(&lock);
	}
}
*/
int main(int argc, const char *argv[])
{
	int i, ret=0;
	struct timeval end_time, result_time;
	double test;
	threadpool_t *pool;
	
	init_rec_array(records);

	/*
	 * init_rec_array test
	 */
	
	for (i = 0; i < MAXREC; i++) {
		printf("%d,%ld,%ld,%c,%ld,%ld\n", records[i].dev_num, records[i].offset, \
				records[i].length, records[i].op, records[i].time.tv_sec, records[i].time.tv_usec);
	}



	pthread_mutex_init(&lock, NULL);

	pool = threadpool_init(THREAD, QUEUE, 0);

	if(!pool) {
		return -1;
	}

	gettimeofday(&begin_time, NULL);
	/*
	for (i = 0; i < MAXREC; i++) {
		ret = threadpool_add(pool, &trace_replay, (void *)&i, 0);
		if (ret) {
			debug_puts("TASK ADD ERROR");
			break;
		}
	}
	*/
	/*
	gettimeofday(&end_time, NULL);
	timersub(&end_time, &begin_time, &result_time);
	*/
	sleep(1);
	threadpool_destroy(pool, 0);
	debug_print("%d", late);
	
	return 0;
}
