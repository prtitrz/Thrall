#include <stdio.h>
#include "easyzmq.h"
#include "common.h"
#include "debug.h"

int req_send(void *context, struct req_data *req_data, char *ip)
{
	void *slave;
	size_t size;
	char *string;
	char tcp[28] = "tcp://";

	strncat(tcp, ip, 21);

	slave = zmq_socket (context, ZMQ_REQ);
	zmq_connect (slave, tcp);

	size = sizeof(struct req_data);

	m_send (slave, req_data, size);
	string = s_recv(slave);
	free (string);
	zmq_close (slave);

	return 0;
}
