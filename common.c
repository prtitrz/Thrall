#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "debug.h"

int main(int argc, const char *argv[])
{
	FILE *fp;
	char ip[16];
	char buf[1024];
	int totalnode, datanode;

	fp = fopen("./thrall.conf", "r");
	if (fp == NULL) {
		debug_puts("FILE can't open");
	}

	while (fgets(buf, 1024, fp) != NULL) {
		if (sscanf(buf, "totalnode=%d", &totalnode) == 1) {
			debug_print("%d", totalnode);
			continue;
		}
		if (sscanf(buf, "datanode=%d", &datanode) == 1) {
			debug_print("%d", datanode);
			continue;
		}
		if (sscanf(buf, "addr=%s", ip) == 1) {
			debug_puts(ip);
		}
	}

	fclose(fp);

	return 1;
}
