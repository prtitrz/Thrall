#define THREAD 128
#define QUEUE  256

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "threadpool.h"
#include "easyzmq.h"
#include "debug.h"

#define SLAVE_NUM 1

int tasks = 0, done = 0;
pthread_mutex_t lock;

void replay(void *arg) {
	void *slave = zmq_socket(arg, ZMQ_REQ);
	/*
	if (done % 2) {
		zmq_connect (slave, "tcp://localhost:5562");
	}
	else {
		zmq_connect (slave, "tcp://localhost:5563");
	}*/
	zmq_connect (slave, "tcp://localhost:5562");

	char *string;
	char buf[1024];
	sprintf(buf, "%d", rand());
	s_send(slave, buf);
	string = s_recv(slave);
	if (strcmp(string , buf) != 0) {
		debug_puts("ERROR");
	}
	free(string);
    pthread_mutex_lock(&lock);
    done++;
    pthread_mutex_unlock(&lock);
	zmq_close(slave);
}

int main(int argc, char **argv)
{
    threadpool_t *pool;
	int i, k, ret=0;
	void *slave[SLAVE_NUM];
	char *string;

	void *context = zmq_init(1);


    pthread_mutex_init(&lock, NULL);

    pool = threadpool_init(THREAD, QUEUE, 0);
    fprintf(stderr, "Pool started with %d threads and "
            "queue size of %d\n", THREAD, QUEUE);
	if (!pool) {
		return -1;
	}

	for (i = 0; i < 10; i++) {
		/*k = i % 2;
		if (k!=0 && k!=1) {
			ret = -100;
			break;
		}*/
    	ret = threadpool_add(pool, &replay, context, 0);
		if (ret) {
			break;
		}
       	pthread_mutex_lock(&lock);
        tasks++;
        pthread_mutex_unlock(&lock);
	}

    fprintf(stderr, "RET: %d Added %d tasks\n", ret, tasks);

    while(tasks / 2 > done) {
        sleep(1);
    }
    fprintf(stderr, "Did %d tasks before shutdown\n", done);
    fprintf(stderr, "Did %d tasks\n", done);

	slave[0] = zmq_socket (context, ZMQ_REQ);
	zmq_connect (slave[0], "tcp://localhost:5562");
/*
	slave[1] = zmq_socket (context, ZMQ_REQ);
	zmq_connect (slave[1], "tcp://localhost:5563");
*/
	for (i = 0; i < SLAVE_NUM; i++) {
		s_send(slave[i], "END");
		string = s_recv(slave[i]);
		free(string);
		zmq_close(slave[i]);
	}

	zmq_term(context);
    threadpool_destroy(pool, 0);

    return 0;
}
