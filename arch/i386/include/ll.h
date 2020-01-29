#ifndef __LINKED_LIST_H
#define __LINKED_LIST_H
struct LinkedList{
	void *data;
	struct LinkedList *next;
};
int llnew();
void *llopen(int lld);
#endif
