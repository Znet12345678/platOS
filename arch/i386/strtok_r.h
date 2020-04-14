#ifndef __STRTOK_R_H
#define __STRTOK_R_H
#include <string.h>

#if __GNUC__ >= 3
# define __glibc_unlikely(cond)        __builtin_expect ((cond), 0)
# define __glibc_likely(cond)        __builtin_expect ((cond), 1)
#else
# define __glibc_unlikely(cond)        (cond)
# define __glibc_likely(cond)        (cond)
#endif
#include <types.h>
char *strtok_r (char *s, const char *delim, char **save_ptr);
#endif
