#ifndef __LINKED_LIST_H
#define __LINKED_LIST_H
typedef struct LinkedList{
	void *data;
	struct LinkedList *next;
}LinkedList;
int llnew();
void *llopen(int lld);
#endif
