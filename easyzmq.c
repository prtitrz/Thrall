#include "easyzmq.h"

//  Receive 0MQ string from socket and convert into C string
//  Caller must free returned string. Returns NULL if the context
//  is being terminated.
char * s_recv (void *socket) {
    zmq_msg_t message;
    zmq_msg_init (&message);
    if (zmq_recv (socket, &message, 0))
        return (NULL);
    int size = zmq_msg_size (&message);
    char *string = malloc (size + 1);
    memcpy (string, zmq_msg_data (&message), size);
    zmq_msg_close (&message);
    string [size] = 0;
    return (string);
}

//FIXME:
int m_recv (void *socket, void *req) {
	zmq_msg_t message;
	zmq_msg_init (&message);
	if (zmq_recv (socket, &message, 0))
		return -1;
	int size = zmq_msg_size (&message);
	memcpy ((char *)req, zmq_msg_data (&message), size);
	zmq_msg_close (&message);
	return 0;
}

int nm_recv (void *socket, void *req) {
	zmq_msg_t message;
	zmq_msg_init (&message);
	if (zmq_recv (socket, &message, ZMQ_NOBLOCK))
		return -1;
	int size = zmq_msg_size (&message);
	memcpy ((char *)req, zmq_msg_data (&message), size);
	zmq_msg_close (&message);
	return 0;
}

//  Convert C string to 0MQ string and send to socket
int s_send (void *socket, char *string) {
    int rc;
    zmq_msg_t message;
    zmq_msg_init_size (&message, strlen (string));
    memcpy (zmq_msg_data (&message), string, strlen (string));
    rc = zmq_send (socket, &message, 0);
    zmq_msg_close (&message);
    return (rc);
}

//FIXME
int m_send (void *socket, void *req, size_t size) {
	int rc;
	zmq_msg_t message;
	zmq_msg_init_size (&message, size);
	memcpy (zmq_msg_data (&message), req, size);
	rc = zmq_send (socket, &message, 0);
	zmq_msg_close (&message);
	return (rc);
}

//  Sends string as 0MQ string, as multipart non-terminal
int s_sendmore (void *socket, char *string) {
    int rc;
    zmq_msg_t message;
    zmq_msg_init_size (&message, strlen (string));
    memcpy (zmq_msg_data (&message), string, strlen (string));
    rc = zmq_send (socket, &message, ZMQ_SNDMORE);
    zmq_msg_close (&message);
    return (rc);
}

//FIXME
void t_sendmore (void *socket, int type) {
	char string[5];
	sprintf(string, "%d", type);
	s_sendmore(socket, string);
}

//FIXME
int m_sendmore (void *socket, void *req, size_t size) {
	int rc;
	zmq_msg_t message;
	zmq_msg_init_size (&message, size);
	memcpy (zmq_msg_data (&message), req, size);
	rc = zmq_send (socket, &message, ZMQ_SNDMORE);
	zmq_msg_close (&message);
	return (rc);
}

//  Receives all message parts from socket, prints neatly
void s_dump (void *socket)
{
    puts ("----------------------------------------");
    while (1) {
        //  Process all parts of the message
        zmq_msg_t message;
        zmq_msg_init (&message);
        zmq_recv (socket, &message, 0);

        //  Dump the message as text or binary
        char *data = zmq_msg_data (&message);
        int size = zmq_msg_size (&message);
        int is_text = 1;
        int char_nbr;
        for (char_nbr = 0; char_nbr < size; char_nbr++)
            if ((unsigned char) data [char_nbr] < 32
            ||  (unsigned char) data [char_nbr] > 127)
                is_text = 0;

        printf ("[%03d] ", size);
        for (char_nbr = 0; char_nbr < size; char_nbr++) {
            if (is_text)
                printf ("%c", data [char_nbr]);
            else
                printf ("%02X", (unsigned char) data [char_nbr]);
        }
        printf ("\n");

        int64_t more;           //  Multipart detection
        size_t more_size = sizeof (more);
        zmq_getsockopt (socket, ZMQ_RCVMORE, &more, &more_size);
        zmq_msg_close (&message);
        if (!more)
            break;      //  Last message part
    }
}

//  Set simple random printable identity on socket
void s_set_id (void *socket)
{
    char identity [10];
    sprintf (identity, "%04X-%04X", randof (0x10000), randof (0x10000));
    zmq_setsockopt (socket, ZMQ_IDENTITY, identity, strlen (identity));
}


//  Sleep for a number of milliseconds
void s_sleep (int msecs)
{
#if (defined (__WINDOWS__))
    Sleep (msecs);
#else
    struct timespec t;
    t.tv_sec  =  msecs / 1000;
    t.tv_nsec = (msecs % 1000) * 1000000;
    nanosleep (&t, NULL);
#endif
}

//  Return current system clock as milliseconds
int64_t s_clock (void)
{
#if (defined (__WINDOWS__))
    SYSTEMTIME st;
    GetSystemTime (&st);
    return (int64_t) st.wSecond * 1000 + st.wMilliseconds;
#else
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (int64_t) (tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
}

//  Print formatted string to stdout, prefixed by date/time and
//  terminated with a newline.

void s_console (const char *format, ...)
{
    time_t curtime = time (NULL);
    struct tm *loctime = localtime (&curtime);
    char *formatted = malloc (20);
    strftime (formatted, 20, "%y-%m-%d %H:%M:%S ", loctime);
    printf ("%s", formatted);
    free (formatted);

    va_list argptr;
    va_start (argptr, format);
    vprintf (format, argptr);
    va_end (argptr);
    printf ("\n");
}
