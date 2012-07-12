#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "easyzmq.h"
#include "common.h"
#include "debug.h"

/*
   Signal handling

   Call s_catch_signals() in your application at startup, and then exit
   your main loop if s_interrupted is ever 1. Works especially well with
   zmq_poll.
   */
static int s_interrupted = 0;
pthread_mutex_t lock;
int req_num = 0;
int fd;

static void s_signal_handler (int signal_value)
{
	s_interrupted = 1;
}

static void s_catch_signals (void)
{
	struct sigaction action;
	action.sa_handler = s_signal_handler;
	action.sa_flags = 0;
	sigemptyset (&action.sa_mask);
	sigaction (SIGINT, &action, NULL);
	sigaction (SIGTERM, &action, NULL);
}

static void *worker_routine(void *context)
{
	struct req_data req_data;
	char *buf;
	void *receiver = zmq_socket(context, ZMQ_REP);
	zmq_connect(receiver, "inproc://workers");

	s_catch_signals();

	while (1){
		m_recv(receiver, &req_data);
		//Send reply back to client
		switch(req_data.length) {
			case ZERO:
				buf = (char *)malloc((ZERO + 1) * sizeof(char));
				break;
			case ONE:
				buf = (char *)malloc((ONE + 1) * sizeof(char));
				break;
			case TWO:
				buf = (char *)malloc((TWO + 1) * sizeof(char));
				break;
			case THREE:
				buf = (char *)malloc((THREE + 1) * sizeof(char));
				break;
			default:
				debug_puts("LENGTH ERROR");
		}
		lseek(fd, req_data.offset, SEEK_SET);
		read(fd, buf, req_data.length);
		s_send(receiver, buf);
		free(buf);
		pthread_mutex_lock(&lock);
		req_num++;
		pthread_mutex_unlock(&lock);

		if (s_interrupted) {
			debug_puts("CTRL+C....");
			break;
		}
	}

	zmq_close(receiver);
	return NULL;
}

int main(int argc, const char *argv[])
{
	void *context = zmq_init (1);
	char tcp[14] = "tcp://*:";

	//TODO:check argv
	strncat(tcp, argv[1], 5);

	//Socket to receive request
/*
	void *service = zmq_socket(context, ZMQ_ROUTER);
	zmq_bind(service, tcp);
	//Socket to talk to workers
	void *workers = zmq_socket (context, ZMQ_DEALER);
	zmq_bind(workers, "inproc://workers");

	pthread_mutex_init(&lock, NULL);
	*/

	void *service = zmq_socket(context, ZMQ_REP);
	zmq_bind(service, tcp);

	fd = open("/dev/sdb", O_RDONLY);
	if (fd == -1) {
		debug_puts("FILE can't open");
		return 1;
	}

	/*
	printf("Waiting for request %s\n", argv[1]);
	printf("Create 5 worker thread\n");
	int thread_nbr;
	for (thread_nbr = 0; thread_nbr < 5; thread_nbr++) {
		pthread_t worker;
		pthread_create(&worker, NULL, worker_routine, context);
	}
	//Connect work threads to client threads via a queue
	zmq_device (ZMQ_QUEUE, service, workers);
	*
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
	struct req_data req_data;
	char *buf;

	s_catch_signals();

	while (1){
		m_recv(service, &req_data);
		//Send reply back to client
		switch(req_data.length) {
			case ZERO:
				buf = (char *)malloc((ZERO + 1) * sizeof(char));
				break;
			case ONE:
				buf = (char *)malloc((ONE + 1) * sizeof(char));
				break;
			case TWO:
				buf = (char *)malloc((TWO + 1) * sizeof(char));
				break;
			case THREE:
				buf = (char *)malloc((THREE + 1) * sizeof(char));
				break;
			default:
				debug_puts("LENGTH ERROR");
		}
		lseek(fd, req_data.offset, SEEK_SET);
		read(fd, buf, req_data.length);
		s_send(service, buf);
		free(buf);

		req_num++;

		if (s_interrupted) {
			debug_puts("CTRL+C....");
			break;
		}
	}

	debug_print("%d", req_num);

	zmq_close(service);
	zmq_term(context);
	return 0;
}
