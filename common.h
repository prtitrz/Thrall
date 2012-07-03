#define MAXREC 100

struct node {
	char ip[16];
	char location[128];
};

struct setting {
	int totalnode;
	int datanode;
	char coding[10];
	struct node *nodes;
};

struct req_data {
	long offset;
	long length;
	char op;
};

enum data_length{ZERO = 8192, ONE = 16384, TWO = 24576, THREE = 32768};

struct record {
	int dev_num;
	long offset;
	long length;
	char op;
	struct timeval time;
};

int init_conf(struct setting *);

int init_rec_array(struct record *);

int req_send(void *, struct req_data *, char *);
