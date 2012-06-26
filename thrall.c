#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "debug.h"
#define MAXREC 10

struct record {
	int dev_num;
	long offset;
	long length;
	char op;
	double time;
}records[MAXREC];

int init_rec_array(struct record *records)
{
	FILE *fp;
	int i;

	fp = fopen("./tmpfile", "r+");
	if (fp == NULL) {
		debug_puts("RECORD FILE OPEN ERROR");
		return 1;
	}

	for (i = 0; i < MAXREC; i++) {
		fscanf(fp, "%d,%ld,%ld,%c,%lf", &(records[i]).dev_num, &records[i].offset, \
				&records[i].length, &records[i].op, &records[i].time);
	}
	
	fclose(fp);
	return 0;
}

int main(int argc, const char *argv[])
{
	int i;
	struct timeval begin_time, end_time, result_time;
	double test;
	
	i = init_rec_array(records);

	/*
	 * init_rec_array test
	 */
	/*
	for (i = 0; i < MAXREC; i++) {
		printf("%d,%ld,%ld,%c,%lf\n", records[i].dev_num, records[i].offset, \
				records[i].length, records[i].op, records[i].time);
	}
	*/

	gettimeofday(&begin_time, NULL);
	test = begin_time.tv_sec + begin_time.tv_usec * 0.000001;
	printf("%lf\n", test);
	gettimeofday(&end_time, NULL);
	timersub(&end_time, &begin_time, &result_time);
	printf("%d, %d, %d\n", result_time.tv_sec, result_time.tv_usec, end_time.tv_usec - begin_time.tv_usec);
	
	return 0;
}
