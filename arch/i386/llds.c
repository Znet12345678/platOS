//LinkedList Descriptor service. Easy solution for dynamic storage of data structures
#include <stderr.h>
#include <ll.h>
#include <stdlib.h>
#include <stdint.h>
#include <libmem.h>
#include <stdio.h>
#include <libio.h>
#include <KMEM.h>
#include <string.h>
void *llopen(int lld){
#ifdef LLDS_DEBUG
	puts("lldsdb:llopen(...)\n");
#endif
	struct LinkedList *lst = (struct LinkedList *)LLLOC;
	for(int i = 0; i < lld;i++)
		lst = lst->next;

	return lst;	
}
int llnew(){
#ifdef LLDS_DEBUG
	puts("lldsdb:llnew()\n");
#endif
	int lld = 0;
	if(!page_mapped((void*)LLLOC)){
#ifdef DEBUG
		puts("Mapping LLDS page\n");
#endif
		void *buf = (void*)allocFree();

		int v = map_page(buf,(void*)LLLOC);
		if(!v)
			_panic("Failed to allocate critical page");
#ifdef DEBUG
		puts("mapped page ");
		putx((uint32_t)allocFree());
		puts(":");
		putx(LLLOC);
		puts("\n");

		bzero((void*)LLLOC,sizeof(struct LinkedList));
#endif
	}
	struct LinkedList *lst = (struct LinkedList *)LLLOC;
	uint8_t *buf = (uint8_t*)LLLOC;
	
	while(lst->next != NULL && !chkerr()){
		lld++;
		lst = lst->next;
	}
	lst->next = malloc(sizeof(*lst->next));

	return lld;

}
void llclose(int llfd){
	struct LinkedList *lst = (struct LinkedList *)LLLOC;
	for(int i = 0; i < llfd-1;i++)
		lst = lst->next;
	free(lst->next);
	lst->next = lst->next->next;
}
