#ifndef simpledb_file_h__
#define simpledb_file_h__

/* CREATE the empty file 
(filename, extension) */
extern int file_set(const char *name, const char *ext);

/* ERASE the data from file if exists 
(filename with ext) */
extern int file_erase(const char * fname);

/* ADD int value to file called 'name' 
(filename with ext, int val) */
extern int file_add_int(const char *name, int value);

/* DELETE int value to file called 'name' 
(filename with ext, int val) */
extern int file_deleteVal_int(const char *fname, int value);

#endif  /* simpledb_file_h__ */