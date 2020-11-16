#ifndef simpledb_lock_h__
#define simpledb_lock_h__

/* CHECK if rwlock for fname is set 
(filename without ext) */
int rw_lock_check(const char *fname);

/* SET the rwlock for fname 
(filename without ext) */
int rw_lock_set(const char *fname);

/* RELEASE the rvlock for fname
(filename without ext) */
int rw_lock_release(const char *fname);

#endif  /* simpledb_lock_h__ */