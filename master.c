#include <stdio.h>
#include "easyzmq.h"

int main (int argc, const char* argv[])
{
	void *context = zmq_init (1);
	int i;
	char test[] = "test";
	char *string;
	void *slave[2];

	//  Second, synchronize with publisher
	slave[0] = zmq_socket (context, ZMQ_REQ);
	zmq_connect (slave[0], "tcp://localhost:5562");

	slave[1] = zmq_socket (context, ZMQ_REQ);
	zmq_connect (slave[1], "tcp://localhost:5563");

	for (i = 0; i < 100000; i++) {
		if (i%2) {
			s_send (slave[0], test);
			string = s_recv(slave[0]);
			free (string);
		}
		else {
			s_send (slave[1], test);
			string = s_recv(slave[1]);
			free(string);
		}
	}


	s_send (slave[0], "END");
	string = s_recv (slave[0]);
	free (string);
	s_send (slave[1], "END");
	string = s_recv (slave[1]);
	free (string);
/*
	//  Third, get our updates and report how many we got
	int update_nbr = 0;
	while (1) {
	//	char *string = s_recv (subscriber);
	}
*/
	zmq_close (slave[0]);
	zmq_close (slave[1]);
	zmq_term (context);
	return 0;
}
