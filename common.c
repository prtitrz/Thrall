#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "debug.h"


int main(int argc, const char *argv[])
{
	FILE *fp;
	char buf[1024];
	struct setting setting;
	int i=0;

	fp = fopen("./thrall.conf", "r");
	if (fp == NULL) {
		debug_puts("FILE can't open");
	}

	while (fgets(buf, 1024, fp) != NULL) {
		if (sscanf(buf, "totalnode=%d", &setting.totalnode) == 1) {
			debug_print("%d", setting.totalnode);
			setting.nodes = (struct node *)malloc(4 * sizeof(struct node));
			continue;
		}
		if (sscanf(buf, "datanode=%d", &setting.datanode) == 1) {
			debug_print("%d", setting.datanode);
			continue;
		}
		if (sscanf(buf, "setting=%s", &setting.coding) == 1) {
			debug_print("%d", setting.coding);
			continue;
		}
		if (sscanf(buf, "addr=%s", setting.nodes[i].ip) == 1) {
			debug_print("%d, %s", i, setting.nodes[i].ip);
			i++;
			continue;
		}
		/*
		if (sscanf(buf, "loc=%s", location) == 1) {
			debug_puts(location);
			continue;
		}
		*/
	}

	free(setting.nodes);
	fclose(fp);

	return 1;
}
