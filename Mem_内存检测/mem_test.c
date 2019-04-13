#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mem_test.h"

MemDebug *_g_mem_debug = NULL;


int _mem_hash(char *key, int key_len)
{
    int hash = 0;
    int seed = 131;
    int i;

    for (i = 0; i < key_len; i ++) {
        hash = *(key ++) + (hash * seed);
    }
    hash = (hash & 0x7fffffff) % MEM_BUNKS;
    return hash;
}

int _mem_ptr_is_exist(void *ptr)
{
    int hash = _mem_hash((char *)&ptr, sizeof(void *));
    MemDebugIterm *root = _g_mem_debug->bunks[hash];
    while (root != NULL) {
        if (ptr == root->key) {
            return 1;
        }
        root = root->next;
    }
    return 0;
}

void _mem_bunks_add(void *ptr)
{
    MemAppendInfo *append_info = MEM_GET_APPEND_INFO(ptr);
    if (_mem_ptr_is_exist(ptr)) {
        WARN("[malloc] ptr=%p file=%s line=%d is already exist!\n",
            ptr, append_info->file, append_info->line);
        return;
    }
    MemDebugIterm * iterm = (MemDebugIterm *)malloc(sizeof(MemDebugIterm));
    assert(iterm != NULL);
    bzero(iterm, sizeof(MemDebugIterm));
    iterm->key = ptr;

    int hash = _mem_hash((char *)&ptr, sizeof(void *));
    //DEBUG("ptr=%p, hash=%d\n", ptr, hash);
    pthread_mutex_lock(&(_g_mem_debug->lock));
    _g_mem_debug->seqid ++;
    iterm->id = _g_mem_debug->seqid;
    MemDebugIterm *root = _g_mem_debug->bunks[hash];
    iterm->next = root;
    _g_mem_debug->bunks[hash] = iterm;
    _g_mem_debug->size += append_info->size;
    _g_mem_debug->count ++;
    DEBUG("[malloc] id=%d ptr=%p file=%s line=%d total=%d.\n", 
        iterm->id, ptr, append_info->file, append_info->line, _g_mem_debug->count);
    pthread_mutex_unlock(&(_g_mem_debug->lock));
}

void _mem_bunks_remove(void *ptr)
{
    MemAppendInfo *append_info = MEM_GET_APPEND_INFO(ptr);
    int hash = _mem_hash((char *)&ptr, sizeof(void *));
    //DEBUG("ptr=%p, hash=%d\n", ptr, hash);
    pthread_mutex_lock(&_g_mem_debug->lock);
    MemDebugIterm *iterm = _g_mem_debug->bunks[hash];
    MemDebugIterm *prev = NULL;
    while (iterm != NULL) {
        if (iterm->key == ptr) {
            if (prev != NULL) {
                prev->next = iterm->next;
            } else {
                _g_mem_debug->bunks[hash] = iterm->next;
            }
            _g_mem_debug->size -= append_info->size;
            _g_mem_debug->count --;
            DEBUG("[free] id=%d ptr=%p size=%d malloc in (file=%s line=%d) total=%d.\n",
                iterm->id, ptr, append_info->size, append_info->file,
                append_info->line, _g_mem_debug->count);
            free(iterm);
            return;
        }
        prev = iterm;
        iterm = iterm->next;
    }
    WARN("[free] ptr=%p NOT FOUND!\n", ptr);
    pthread_mutex_unlock(&_g_mem_debug->lock);
}

void *_mem_malloc(int size, const char *file, uint16_t line)
{
    void *ptr = NULL;
    if ((_g_mem_debug->flags & MEM_TEST) != 0) {
        if (size <= 0) {
            WARN("[malloc] file=%s line=%d malloc size <= 0!\n",
                file, line);
            return NULL;
        }
        int total_size = size + sizeof(MemAppendInfo) + sizeof(int)*2;
        ptr = malloc(total_size);
        bzero(ptr, total_size);

        MemAppendInfo *append_info = (MemAppendInfo *)ptr;
        append_info->size = size;
        strncpy(append_info->file, file, sizeof(append_info->file));
        append_info->line = line;
        ptr += sizeof(MemAppendInfo);
        *((int *)ptr) = MEM_FILL;
        ptr += sizeof(int);
        *((int *)(ptr + size)) = MEM_FILL;
        
        _mem_bunks_add(ptr);
    } else {
        ptr = malloc(size);
    }
    return ptr;
}

void _mem_free(void *ptr, const char *file, uint16_t line)
{
    if ((_g_mem_debug->flags & MEM_TEST) == 0) {
        free(ptr);
        return;
    }
    if (ptr == NULL) {
        WARN("[free] ptr in file=%s line=%d is NULL!\n", file, line);
        return;
    }
    _mem_check(ptr, file, line);
    _mem_bunks_remove(ptr);
    free(ptr - sizeof(int) -sizeof(MemAppendInfo));
}

int _mem_check(void *ptr, const char *file, uint16_t line)
{
    int res = 0;
    if ((_g_mem_debug->flags & MEM_TEST) == 0) {
        WARN("[check] not in MEM_TEST status!\n");
        return 0;
    }
    if (ptr == NULL) {
        WARN("[check] ptr in file=%s line=%d is NULL!\n", file, line);
        return MEM_ERR_IS_NULL;
    }
    MemAppendInfo *append_info = MEM_GET_APPEND_INFO(ptr);
    if (*((int *)(ptr - sizeof(int))) != MEM_FILL) {
        ERR("[check] ptr=%p size=%d malloc in (file=%s line=%d) check in (file=%s line=%d) OVER FLOW at start!\n",
            ptr, append_info->size, append_info->file, 
            append_info->line, file, line);
        res = MEM_ERR_OVER_FLOW;
    }
    if (*(int *)(ptr + append_info->size) != MEM_FILL) {
        ERR("[check] ptr=%p size=%d malloc in (file=%s line=%d) check in (file=%s line=%d) OVER FLOW at end!\n",
            ptr, append_info->size, append_info->file, 
            append_info->line, file, line);
        res = MEM_ERR_OVER_FLOW;
    }
    return res;
}

int _mem_check_point(int start_id, const char *file, uint16_t line)
{
    if ((_g_mem_debug->flags & MEM_TEST) == 0) {
        WARN("[check] not in MEM_TEST status!\n");
        return 0; 
    }
    if (start_id == 0) {
        pthread_mutex_lock(&_g_mem_debug->lock);
        start_id = _g_mem_debug->seqid;
        pthread_mutex_unlock(&_g_mem_debug->lock);
        return start_id;
    }
    int i, res;
    int extra_amlloc_count = 0;
    int over_flow_count = 0;
    pthread_mutex_lock(&_g_mem_debug->lock);
    INFO("\n  =======  Start test memery(pthread id: %lu) ======\n", pthread_self());
    for (i = 0; i < MEM_BUNKS; i ++) {
        MemDebugIterm *iterm = _g_mem_debug->bunks[i];
        while (iterm != NULL) {
            // 检测是否存在内存益处
            if ((res = _mem_check(iterm->key, file, line)) < 0) {
                if (res == MEM_ERR_OVER_FLOW)
                    over_flow_count ++;
            }
            
            // 检测是否在start_id之后有内存申请
            if (iterm->id > start_id) {
                MemAppendInfo *append_info = MEM_GET_APPEND_INFO(iterm->key);
                WARN("[check] ptr=%p size=%d malloc in (file=%s line=%d) check in (file=%s line=%d) EXTRA MALLOC!\n",
                    iterm->key, append_info->size, append_info->file,
                    append_info->line, file, line);
                extra_amlloc_count ++;
            }
            iterm = iterm->next;
        }
    }
    INFO("total_malloc_count=%d\ttotal_malloc_size=%d(Byte)\nextra_malloc_count=%d\tover_flow_count=%d\n",
        _g_mem_debug->count, _g_mem_debug->size, extra_amlloc_count, over_flow_count);
    INFO("  =======  End test memery (pthread id: %lu)======\n", pthread_self());
    pthread_mutex_unlock(&_g_mem_debug->lock);
    return 0;
}

void mem_init(int flags)
{
    if (_g_mem_debug != NULL)
        return;
    _g_mem_debug = (MemDebug *)malloc(sizeof(MemDebug));
    assert(_g_mem_debug != NULL);
    bzero(_g_mem_debug, sizeof(MemDebug));

    if ((flags & MEM_TEST) != 0) {
        _g_mem_debug->bunks = (MemDebugIterm **)malloc(sizeof(MemDebugIterm *) * MEM_BUNKS);
        assert(_g_mem_debug->bunks != NULL);
        bzero(_g_mem_debug->bunks, sizeof(MemDebugIterm *) * MEM_BUNKS);
    }

    _g_mem_debug->flags = flags;
    pthread_mutex_init(&_g_mem_debug->lock, NULL);
}

int main()
{
    int i;
    void *ptr_arry[21] = {NULL};
    mem_init(MEM_TEST);
    
    INFO("-----   malloc(10)  -----\n");
    for (i = 0; i < 15; i ++) {
        ptr_arry[i] = mem_malloc(i+1);
    }

    INFO("-----  check -----\n");
    for (i = 0; i < 10; i++) {
        bzero(ptr_arry[i], 5);
        mem_check(ptr_arry[i]);
    }

    INFO("----  last malloc point -----\n");
    int start_id = mem_check_point(0);
    INFO("ID: %d\n", start_id);

    INFO("----  extra malloc(5) ----\n");
    for (i = 15; i < 21; i ++) {
        ptr_arry[i] = mem_malloc(i+1);
    }
    mem_check_point(start_id);

    INFO("---- free  -----\n");
    for (i = 5; i< 21; i ++) {
        mem_free(ptr_arry[i]);
    }
    mem_check_point(start_id);
    return 0;
}

