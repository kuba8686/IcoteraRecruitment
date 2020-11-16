#include <stdio.h>

#include "simpledb_lock.h"

int rw_lock_check(const char *fname)
{
    FILE *f;
    char fname_buff[20];
    
    sprintf(fname_buff, "%s.lock", fname);
    
    if ((f = fopen(fname_buff, "rb"))) {
        fclose(f);
        return 1;
    }
    return 0;
}

int rw_lock_set(const char *fname)
{
    FILE *f;
    char fname_buff[20];
    
    sprintf(fname_buff, "%s.lock", fname);
    
    f = fopen(fname_buff, "wb");
    if (f == NULL) {
        return 1;
    }
    fclose(f);
    return 0;
}

int rw_lock_release(const char *fname)
{
    char fname_buff[20];
    
    sprintf(fname_buff, "%s.lock", fname);
    
    return remove(fname_buff);
} 