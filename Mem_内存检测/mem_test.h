#ifndef _MEM_H_
#define _MEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

#define MEM_TEST 1

#define MEM_BUNKS   1000000
#define MEM_FILL    0x55555555

#define DEBUG(format, args...)  do{fprintf(stdout, format, ##args);}while(0)
#define INFO(format, args...)  do{fprintf(stdout, format, ##args);}while(0)
#define WARN(format, args...)  do{fprintf(stdout, format, ##args);}while(0)
#define ERR(format, args...)  do{fprintf(stderr, format, ##args);}while(0)

#define MEM_ERR_IS_NULL     (-1)
#define MEM_ERR_OVER_FLOW   (-2)
#define MEM_ERR_EXTRA_MALL  (-3)

/**
    _______________________________________________________________
    |           |        |                                |        |
    |append_info|MEM_FILL|          user malloc           |MEM_FILL|
    |___________|________|________________________________|________|
*/

typedef struct _mem_append_info {
    int         size;
    char        file[14];
    uint16_t    line;
} MemAppendInfo;

typedef struct _mem_debug_iterm {
    struct _mem_debug_iterm *next;
    void        *key;
    uint32_t    id;
} MemDebugIterm;

typedef struct _mem_debug {
    uint32_t    flags;
    uint32_t    size;
    uint32_t    seqid;
    uint32_t    count;
    pthread_mutex_t lock;
    MemDebugIterm   **bunks;
} MemDebug;

extern MemDebug *_g_mem_debug;
#define MEM_GET_APPEND_INFO(ptr) ((MemAppendInfo *)((ptr) - sizeof(MemAppendInfo) -4))

void *_mem_malloc(int size, const char *file, uint16_t line);
int _mem_check(void *ptr, const char *file, uint16_t line);
int _mem_check_point(int start_id, const char *file, uint16_t line);
void _mem_free(void *ptr, const char *file, uint16_t line);

// API
void mem_init(int flag);

#define mem_malloc(size)    _mem_malloc(size, __FILE__, __LINE__)
#define mem_free(ptr)       _mem_free(ptr, __FILE__, __LINE__)
#define mem_check(ptr)      _mem_check(ptr, __FILE__, __LINE__)
#define mem_check_point(start_id)   _mem_check_point(start_id, __FILE__, __LINE__)
#endif
