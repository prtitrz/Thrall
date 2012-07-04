#include <stdio.h>
#include <pthread.h>
#include "easyzmq.h"
#include "common.h"
#include "debug.h"

static void *worker_routine(void *context)
{
	struct req_data req_data;
	void *receiver = zmq_socket(context, ZMQ_REP);
	zmq_connect(receiver, "inproc://workers");

	while (1){
		m_recv(receiver, &req_data);
		debug_print("offset:%ld, length:%ld, %c", req_data.offset, req_data.length, req_data.op);
		//Send reply back to client
		s_send(receiver, "HELLO");
	}

	zmq_close(receiver);
	return NULL;
}

int main(int argc, const char *argv[])
{
	void *context = zmq_init (1);
	char tcp[14] = "tcp://*:";
	int update_nbr=0;

	//TODO:check argv
	strncat(tcp, argv[1], 5);

	//Socket to receive request
	void *service = zmq_socket(context, ZMQ_ROUTER);
	zmq_bind(service, tcp);

	//Socket to talk to workers
	void *workers = zmq_socket (context, ZMQ_DEALER);
	zmq_bind(workers, "inproc://workers");

	printf("Waiting for request %s\n", argv[1]);
	printf("Create 5 worker thread\n");
	int thread_nbr;
	for (thread_nbr = 0; thread_nbr < 5; thread_nbr++) {
		pthread_t worker;
		pthread_create(&worker, NULL, worker_routine, context);
	}
	//Connect work threads to client threads via a queue
	zmq_device (ZMQ_QUEUE, service, workers);
	/*
	while (1) {
		char *string = s_recv(service);
		s_send(service, string);
		if (strcmp(string, "END") == 0) {
			free(string);
			break;
		}
		free(string);
		update_nbr++;
	}
	printf("Received %d updates\n", update_nbr);
	*/

	zmq_close(service);
	zmq_close(workers);
	zmq_term(context);
	return 0;
}
