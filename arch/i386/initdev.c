#include <vfs.h>
#include <ps2.h>
#include <libio.h>
#include <libmem.h>
#include <stdlib.h>
#include <stdint.h>
void map_devs(){
	struct dev *stdin_dev = malloc(sizeof(*stdin_dev));	
	stdin_dev->begin = 0;
	stdin_dev->clustersize = 1;
	stdin_dev->read = stdin_read;
	stdin_dev->write = NULL;
	struct dev *stdout_dev = malloc(sizeof(*stdout_dev));
	stdout_dev->begin = 0;
	stdout_dev->clustersize = 1;
	stdout_dev->read = NULL;
	stdout_dev->write = stdout_write;
	struct dev *ata = malloc(sizeof(*ata));
	ata->begin = 0;
	ata->clustersize = 512;
	ata->read = ata_read;
	ata->write = ata_write;
	struct dev_list *dev_list = (struct dev_list *)kernel32_dev_first_node;
	dev_list->dev = stdin_dev;
	dev_list->nxt = malloc(sizeof(*dev_list->nxt));
	dev_list = dev_list->nxt;
	dev_list->dev = stdout_dev;
	dev_list->nxt = malloc(sizeof(*dev_list->nxt));
	dev_list = dev_list->nxt;
	dev_list->dev = ata;
	dev_list->nxt = 0;
}
