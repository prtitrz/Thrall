#ifndef __ZHELPERS_H_INCLUDED__
#define __ZHELPERS_H_INCLUDED__

//  Include a bunch of headers that we will need

#include <zmq.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

//  Version checking, and patch up missing constants to match 2.1
#if ZMQ_VERSION_MAJOR == 2
#   if ZMQ_VERSION_MINOR == 0
#       error "Please upgrade to ZeroMQ/2.1 stable for these examples"
#   endif
#elif ZMQ_VERSION_MAJOR == 3
#   error "Please stick with ZeroMQ/2.1 stable for these examples"
#endif
#ifndef ZMQ_ROUTER
#   define ZMQ_ROUTER ZMQ_XREP
#endif
#ifndef ZMQ_DEALER
#   define ZMQ_DEALER ZMQ_XREQ
#endif


//  Provide random number from 0..(num-1)
#if (defined (__WINDOWS__))
#   define randof(num)  (int) ((float) (num) * rand () / (RAND_MAX + 1.0))
#else
#   define randof(num)  (int) ((float) (num) * random () / (RAND_MAX + 1.0))
#endif

extern char * s_recv(void *socket);
extern int m_recv(void *socket, void *req);
extern int nm_recv(void *socket, void *req);
extern int s_send(void *socket, char *string);
extern int m_send(void *socket, void *req, size_t size);
extern int s_sendmore(void *socket, char *string);
extern int m_sendmore(void *socket, void *req, size_t size);
extern void t_sendmore(void *socket, int type);

#endif  //  __ZHELPERS_H_INCLUDED__
