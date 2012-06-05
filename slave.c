#include <stdio.h>
#include "easyzmq.h"

int main(int argc, const char *argv[])
{
	void *context = zmq_init (1);
	char tcp[14] = "tcp://*:";
	int update_nbr=0;

	//  TODO:check argv
	strncat(tcp, argv[1], 5);

	//  Socket to receive request
	void *service = zmq_socket (context, ZMQ_REP);
	zmq_bind (service, tcp);

	printf ("Waiting for request %s\n", argv[1]);
	while (1) {
		char *string = s_recv (service);
		s_send (service, string);
		if (strcmp (string, "END") == 0) {
			free (string);
			break;
		}
		free (string);
		update_nbr++;
	}
	//  Now broadcast exactly 1M updates followed by END
	printf ("Received %d updates\n", update_nbr);
//	for (update_nbr = 0; update_nbr < 1000000; update_nbr++)
//		s_send (publisher, "Rhubarb");

//	s_send (publisher, "END");

	zmq_close (service);
	zmq_term (context);
	return 0;
}
