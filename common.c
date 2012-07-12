#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "debug.h"

int init_conf(struct setting *setting) 
{
	FILE *fp;
	char buf[1024];
	int i=0;

	fp = fopen("./thrall.conf", "r");
	if (fp == NULL) {
		debug_puts("FILE can't open");
		return 1;
	}

	while (fgets(buf, 1024, fp) != NULL) {
		if (sscanf(buf, "totalnode=%d", &setting->totalnode) == 1) {
			setting->nodes = (struct node *)malloc(setting->totalnode * sizeof(struct node));
			continue;
		}
		if (sscanf(buf, "datanode=%d", &setting->datanode) == 1) {
			continue;
		}
		if (sscanf(buf, "setting=%s", &setting->coding) == 1) {
			continue;
		}
		if (sscanf(buf, "addr=%s", setting->nodes[i].ip) == 1) {
			i++;
			continue;
		}
		/*
		if (sscanf(buf, "loc=%s", location) == 1) {
			debug_puts(location);
			continue;
		}
			debug_print("%d", setting->totalnode);
			debug_print("%d", setting->datanode);
			debug_print("%d", setting->coding);
			debug_print("%d, %s", i, setting->nodes[i].ip);
		*/
	}

	fclose(fp);

	return 0;
}

int init_rec_array(struct record *records)
{
	FILE *fp;
	int i;
	double time;
	char tmp[20];

	fp = fopen("./tmpfile", "r+");
	if (fp == NULL) {
		debug_puts("RECORD FILE OPEN ERROR");
		return 1;
	}

	for (i = 0; i < MAXREC; i++) {
		fscanf(fp, "%d,%ld,%ld,%c,%lf", &(records[i]).dev_num, &records[i].offset, \
				&records[i].length, &records[i].op, &time);
		sprintf(tmp, "%lf", time);
		sscanf(tmp, "%ld.%ld", &records[i].time.tv_sec, \
				&records[i].time.tv_usec);
		/*
		 * According the doc, the meaning of offset is LBA. So..
		 */
		records[i].offset = records[i].offset * 512;
	}
	
	fclose(fp);
	return 0;
}

