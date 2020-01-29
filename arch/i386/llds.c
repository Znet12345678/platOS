//LinkedList Descriptor service. Easy solution for dynamic storage of data structures
#include <ll.h>
#include <stdlib.h>
#include <stdint.h>
#include <libmem.h>
#include <stdio.h>
#include <libio.h>
#include <KMEM.h>
#include <string.h>
void *llopen(int lld){
	struct LinkedList *lst = (struct LinkedList *)LLLOC;
	for(int i = 0; i < lld;i++)
		lst = lst->next;
	return lst->data;	
}
int llnew(){
	int lld = 0;
	if(!page_mapped((void*)LLLOC)){
		void *buf = (void*)allocFree();
		int v = map_page(buf,(void*)LLLOC);
		if(!v)
			panic("Failed to allocate critical page");
	}
	struct LinkedList *lst = (struct LinkedList *)LLLOC;
	while(lst->next != 0){
		puts(".");
		lld++;
		lst = lst->next;
	}
	lst->next = malloc(sizeof(*lst->next));
	
	return lld+1;
}
