/*
 * For some reason, please don't set THREAD maxxer than 256.
 */
#define THREAD 128
#define QUEUE 256

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include "debug.h"
#include "threadpool.h"
#include "common.h"
#include "easyzmq.h"

struct timeval begin_time;
long int late = 0;
long int done = 0;
pthread_mutex_t lock;
long int temp[THREAD];
struct setting setting;
struct record records[MAXREC];
void *context;

void trace_replay(void *arg)
{
	struct req_data req_data;
	struct timeval end_time, result_time, test_time;
	/*long int i = *(long int *)arg;*/
	long int i;
	char *ip;
	i = *(long int *)arg;
	*(long int *)arg += THREAD;
/*
	int mod;
*	mod = (long int *)arg - temp; 
*	debug_print("%ld", temp[mod]);
*	temp[mod] = temp[mod] + THREAD;
*/
	gettimeofday(&end_time, NULL);
	timersub(&end_time, &begin_time, &result_time);
	timersub(&records[i].time, &result_time, &test_time);
	do {
		if (test_time.tv_sec < 0) {
			pthread_mutex_lock(&lock);
			late = late + i;
			debug_print("%ld:%ld.%ld\t%ld.%ld, TIME LATE", i, records[i].time.tv_sec, records[i].time.tv_usec, result_time.tv_sec, result_time.tv_usec);
			pthread_mutex_unlock(&lock);
			break;
		}
		if (test_time.tv_sec != 0) {
			debug_print("%ld, TIME_ERROR", i);
			break;
		}
		if (test_time.tv_usec >= 0) {
			usleep(test_time.tv_usec);
			/*
			debug_print("%ld, %ld", i, test_time.tv_usec);
			pthread_mutex_lock(&lock);
			done++;
			pthread_mutex_unlock(&lock);
			*/
		}
	} while(0);
	/*
	 * TODO: do something
	if (records[i].dev_num == 1) {
	 */
	req_data.offset = records[i].offset;
	req_data.length = records[i].length;
	req_data.op = records[i].op;
	ip = setting.nodes[records[i].dev_num].ip;
		/*
		debug_print("%ld", i);
		*/
	req_send(context, &req_data, ip);
	gettimeofday(&end_time, NULL);
	timersub(&end_time, &begin_time, &result_time);
	timersub(&result_time, &records[i].time, &records[i].res_time);
}

int main(int argc, const char *argv[])
{
	long int i, ret=0;
	threadpool_t *pool;
	struct timeval total_time;
	
	/*
	 * init temp array for record trace num
	 */
	for (i = 0; i < THREAD; i++) {
		temp[i] = i;
	}

	/*
	 * read the record file
	 */
	init_rec_array(records);

	/*
	 * init_rec_array test
	 */
	/*	
	for (i = 0; i < MAXREC; i++) {
		printf("%d,%ld,%ld,%c,%ld,%ld\n", records[i].dev_num, records[i].offset, \
				records[i].length, records[i].op, records[i].time.tv_sec, records[i].time.tv_usec);
	}
	*/	

	pthread_mutex_init(&lock, NULL);

	pool = threadpool_init(THREAD, QUEUE, 0);

	if(!pool) {
		return -1;
	}

	ret = init_conf(&setting);

	if(ret) {
		return -1;
	}

	context = zmq_init(1);

	gettimeofday(&begin_time, NULL);

	/*
	 * add trace play to the threadpool and begin the trace play
	 */
	for (i = 0; i < MAXREC; i++) {
		/*ret = threadpool_add(pool, &trace_replay, (void *)&temp[i], 0);
		 */
		ret = threadpool_add(pool, &trace_replay, (void *)&temp[i % THREAD], 0);
		if (ret) {
			debug_puts("TASK ADD ERROR");
			break;
		}
	}
	
	sleep(1);
	threadpool_destroy(pool, 0);
	free(setting.nodes);
	zmq_term (context);
	debug_print("%ld", late);

	total_time.tv_sec = total_time.tv_usec = 0;

	debug_puts("Trace play over, now calculate mean response time");
	for (i = 0; i < MAXREC; i++) {
		timeradd(&total_time, &records[i].res_time, &total_time);
	}
	debug_print("Play %ld traces in %ld.%lds", MAXREC, total_time.tv_sec, total_time.tv_usec);
	
	return 0;
}
