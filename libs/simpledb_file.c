#include <stdio.h>

#include "simpledb_file.h"

int file_set(const char *name, const char *ext)
{
    FILE *f;
    char fname_buff[20];
    
    sprintf(fname_buff, "%s.%s", name, ext);
    
    f = fopen(fname_buff, "wb");
    if (f == NULL) {
        return 1;
    }
    fclose(f);
    return 0;
}

int file_erase(const char * fname)
{
    FILE *f;
    
    /* check if file exist by opening it attemption */
    f = fopen(fname, "rb");
    if (f == NULL)
        return 1;
    
    /* if exist close */
    fclose(f);
    
    /* remve the file */
    if (remove(fname) != 0)
        return 1;
    
    /* create the file with the same name */
    f = fopen(fname, "wb");
    if (f == NULL)
        return 1;
    
    fclose(f);
    
    return 0;
}

/* ADD int value to file called 'name' */
int file_add_int(const char *name, int value)
{
    FILE *f;
    
    f = fopen(name, "ab");
    if (f == NULL) {
        return 1;
    }
    if (fwrite(&value, sizeof(int), 1, f) == 0) {
        fclose(f);
        return 1;
    }
        

    fclose(f);
    return 0;
}

int file_deleteVal_int(const char *fname, int value)
{
    FILE *f, *f_tmp;
    int val_inFile;
    char tfname[20];
    
    f = fopen(fname, "rb");
        if (f == NULL) {
            return 1;
    }
    
    sprintf(tfname, "%s.bin", fname);
    
    f_tmp = fopen(tfname, "ab");
        if (f == NULL) {
            fclose(f);
            return 1;
        }   
    
    while (fread(&val_inFile, sizeof(int), 1, f) != 0) {
        if(val_inFile != value) 
            fwrite(&val_inFile, sizeof(int), 1, f_tmp);
    }
    
    fclose(f);
	fclose(f_tmp);
    
    remove(fname);
	rename(tfname, fname);

	return 0;
}