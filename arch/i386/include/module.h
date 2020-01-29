#ifndef __MODULE_H
#define __MODULE_H
#include <stdint.h>
void module_register(void *pntr);
#define REGISTER(x) module_register(x)
#endif
