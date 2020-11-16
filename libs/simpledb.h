#ifndef simpledb_h__
#define simpledb_h__

#define ROW_MAX_LENGTH 20 

struct Local_db{ int id; char name[ROW_MAX_LENGTH + 1]; };

/* SHOW DATABASE FROM local_db structure
(ptr to local_db str, no of records */
extern int db_show(struct Local_db * local_db, int n); 

/* WRITE TO FILE FROM local_db structure
(ptr to local_db str, no of records, filename) */
extern int db_write(struct Local_db * local_db, int n, const char * filename);

/* READ FROM FILE TO local_db structure
(ptr to local_db str, no of records, filename) */
extern int db_read(struct Local_db * local_db, int n, const char * filename);

/* CHANGE record in local_db structure
(ptr to local_db str, no of records, record_id) */
extern int db_change(struct Local_db * local_db, int n);

/* CHECK (and UPDATE) if any other program make changes in database */
extern int db_update(struct Local_db * local_db, int n, const char * db_name);

/* INITIALIZE PROGRAMM: read data from file to local_db, 
    assign PID no to lib global var, set the listener file, 
    update all.listener file by current program PID */
extern int db_init(struct Local_db * local_db, int n, const char * db_name, int p);

/* CLOSING PROGRAMM: 
    - release the memory after table containing changed records id's,
    - delete listener file
    - delete current program PID from all.listeners file */
extern int db_close();



#endif  /* simpledb_h__ */