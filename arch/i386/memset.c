#include <types.h>
void *memset(void *str,char c,size_t n){
	for(size_t i = 0; i < n;i++)
		((char*)str)[i] = c;
	return str;
}
