/*  Welcome to LIB_SIMPLEDB by Jakub Bartoszewicz

    in main program MUST have to include (in such an order): 
        - db_init(...)  ->  to initialize the database
        - db_update(...) ->  to update local_db record changed 
                            by other programs (if any, checking listener)
        - db_close(...) ->  to close the database
        
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "simpledb.h"
#include "simpledb_file.h"
#include "simpledb_lock.h"

/* stop the program allways setting the rwlock (1 - active, 0 - unactive) */
#define LOCK_DEBUG 0

/* name of file holding all runing process pids */
#define ALL_LISTENERS "all.listener" 

/* dynamically alocated tables handling id's of changed records */
int *changedRec;
  
/* number of changed records, during the programm is runing
reset to 0 always when db_file is succesfully updated and 
other runing process are informed. */ 
int noOfChangedRec = 0; 

/* global variable carrying pid number */
int pid; 


int db_init(struct Local_db * local_db, int n, const char * db_name, int p)
{
    char pid_buffer[20];
    #if LOCK_DEBUG==1
    int debug;
    #endif
    
    /* load database from file */
    if (db_read(local_db, n, db_name) != 0) {
        printf("Error reading database file. Exiting the program.\n");
        return 1;
    }
    
    /* assign pid number to global var */
    pid = p;
    
    /* convert pid to array of char */
    sprintf(pid_buffer, "%d", pid);
    
    /* set the listener for the file */
    if(file_set(pid_buffer, "listener") != 0)
        return 1;
    
    /* check if set lock for all.listener */
    if (rw_lock_check(ALL_LISTENERS)) {
        printf("%s in use, please wait...\n", ALL_LISTENERS);
        while(rw_lock_check(ALL_LISTENERS))
            sleep(1);
    }
    
    /* set lock for all.listener */
    if(rw_lock_set(ALL_LISTENERS) != 0)
        return 1;
    
    #if LOCK_DEBUG==1
    printf("\n\033[0;31m LOCK DEBUG -3: (db_init)"
    " lock for %s is set.\033[0m\n", ALL_LISTENERS);
    scanf("%d", &debug);
    #endif
    
    /* update all listener files with current process pid */
    if(file_add_int(ALL_LISTENERS, p) != 0) {
        rw_lock_release(ALL_LISTENERS);
        return 1;
    }
        
    /* release the lock all.listener */
    rw_lock_release(ALL_LISTENERS);
    
    return 0;
}

int db_close()
{
    char pid_buffer[20];
    int attempt = 10;
    #if LOCK_DEBUG==1
    int debug;
    #endif
    
    /* phase 1 - free memory */
        /* release the memory -> table containing changed record id's */
    if(changedRec != NULL)
        free(changedRec);
    
    /* phase 2 - update all.listener */
        /* check if set lock for all.listener */
    if (rw_lock_check(ALL_LISTENERS)) {
        printf("%s in use, please wait...\n", ALL_LISTENERS);
        while(rw_lock_check(ALL_LISTENERS))
            sleep(1);
    }
    
        /* set lock for all.listener */
    if(rw_lock_set(ALL_LISTENERS) == 0) {
        
        #if LOCK_DEBUG==1
        printf("\n\033[0;31m LOCK DEBUG -2: (db_close)"
        " lock for %s is set.\033[0m\n", ALL_LISTENERS);
        scanf("%d", &debug);
        #endif
    
        /* delete pid from all.listener file */
        if(file_deleteVal_int(ALL_LISTENERS, pid) != 0) {
            printf("Problem updating %s. "
            "File can contain out of date PID's information.\n", ALL_LISTENERS);
        }
    
        /* release the lock for all.listener */
        rw_lock_release(ALL_LISTENERS);
        
    } else {
        printf("Problem updating %s. "
            "File can contain out of date PID's information.\n", ALL_LISTENERS);
    }
    
    /* phase 3 - delete listenr*/
        /* prepare listener filename */
    sprintf(pid_buffer, "%d.listener", pid);

        /* try to delete listener, default max 10 attempts */
    while(attempt--)
        if(remove(pid_buffer) == 0)
            attempt = 0;
        /* if listener remove unsuccesful - inform user */
    if(!attempt)
        printf("Cannot remove %d.listener. Please remove it manually.\n", pid);
    
    return 0;
}

int db_signal()
{
    FILE *f;
    int i, tpid;
    char buff[20], buff_lisLock[20];
    #if LOCK_DEBUG==1
    int debug;
    #endif
    
    /* check if set lock for all.listener */
    if (rw_lock_check(ALL_LISTENERS)) {
        printf("%s in use, please wait...\n", ALL_LISTENERS);
        while(rw_lock_check(ALL_LISTENERS))
            sleep(1);
    }
    
    /* set lock for all.listener */
    if(rw_lock_set(ALL_LISTENERS) != 0)
        return 1;
    
    #if LOCK_DEBUG==1
    printf("\n\033[0;31m LOCK DEBUG -1: (update signal to other)"
    " lock for %s is set.\033[0m\n", ALL_LISTENERS);
    scanf("%d", &debug);
    #endif
    
    /* open file with all listeners */
    f = fopen(ALL_LISTENERS, "rb");
    if (f == NULL) 
        return 1;
    
    /* read from all.listeners file every single pid */
    while(fread(&tpid, sizeof(int), 1, f) != 0) {
        
        /* check if pid from file is not current process pid */
        if(tpid != pid) {
            
            sprintf(buff_lisLock, "%d", tpid);
            
            /* check if lock for particular proces pid is set, if yes wait until release */
            if (rw_lock_check(buff_lisLock)) {
                printf("Listener for PID: %d in use, please wait...\n", tpid);
                while(rw_lock_check(buff_lisLock))
                    sleep(1);
            }
            
            /* set lock for listener */
            if(rw_lock_set(buff_lisLock) != 0)
                return 1;
            
            #if LOCK_DEBUG==1
            printf("\n\033[0;31m LOCK DEBUG 0: (update signal to other)"
            " lock for listener %d is set.\033[0m\n", pid);
            scanf("%d", &debug);
            #endif
            
            /* write for listener file every record id that changed */
            for (i = 0; i < noOfChangedRec; i++) {
                sprintf(buff, "%d.listener", tpid);
                file_add_int(buff, changedRec[i]);
            }
            
            /* release the lock for listener */
            rw_lock_release(buff_lisLock);
        }
    }
    
    fclose(f);
    
    /* release the lock all.listener */
    rw_lock_release(ALL_LISTENERS);
    
    return 0;
}
    
int db_show(struct Local_db * local_db, int n) 
{
    int i;
    
    if (!local_db)
        return 1;
    
    printf("\nDATABASE content:\n\n");
    for (i = 0; i < 34; i++) 
        putchar('-');
    printf("\n| %6s | %21s |\n", "ID", "Name");
    for (i = 0; i < 34; i++) 
        putchar('-');
    printf("\n");
    for (i = 0; i < n; i++)
        printf("| %6d | %21s |\n", local_db[i].id, local_db[i].name);
    for (i = 0; i < 34; i++) 
        putchar('-');
    printf("\n");
    
    return 0;
}

int db_write(struct Local_db * local_db, int n, const char * db_name)
{
    FILE * f;
    char fname_buff[20];
    #if LOCK_DEBUG==1
    int debug;
    #endif
        
    printf("Writing data (to file) for database: '%s' in progress ...\n", db_name);
    
    /* check if lock for db_name is set, if yes wait until release */
    if (rw_lock_check(db_name)) {
        printf("Database '%s' in use, please wait...\n", db_name);
        while(rw_lock_check(db_name))
            sleep(1);
    }
    
    /* set lock for db_name */
    if(rw_lock_set(db_name) != 0)
        return 1;
    
    #if LOCK_DEBUG==1
    printf("\n\033[0;31m LOCK DEBUG 1: (db_write) lock for database '%s' is set.\033[0m\n", db_name);
    scanf("%d", &debug);
    #endif
    
    if (!local_db) {
        return 1;
    }

    sprintf(fname_buff, "%s.dat", db_name);
    
    f = fopen(fname_buff, "wb");
    if (f == NULL) {
        return 1;
    }
    
    if (fwrite(local_db, sizeof(struct Local_db), n, f) == 0) {
        fclose(f);
        return 1;
    }

    printf("Succesfully saved local_db to file\n");
    
    fclose(f);
    rw_lock_release(db_name);
    
    /* inform other app and reset the table with changed record id's
        -> write info to listeners files */
    if(db_signal() == 0) {
        free(changedRec);
        noOfChangedRec = 0;
    } else {
        return 2;
    }
  
    return 0;
}

int db_read(struct Local_db * local_db, int n, const char * db_name)
{
    FILE * f;
    char fname_buff[20];
    #if LOCK_DEBUG==1
    int debug;
    #endif
        
    printf("Reading data (from file) for database: '%s' in progress ...\n", db_name);
    
    if (rw_lock_check(db_name)) {
        printf("Database '%s' in use, please wait...\n", db_name);
        while(rw_lock_check(db_name))
            sleep(1);
    }
    
    /* set lock for db_name */
    if(rw_lock_set(db_name) != 0)
        return 1;
    
    #if LOCK_DEBUG==1
    printf("\n\033[0;31m LOCK DEBUG 2: (db_read) lock for database '%s' is set.\033[0m\n", db_name);
    scanf("%d", &debug);
    #endif
    
    if (!local_db) {
        rw_lock_release(db_name);
        return 1;
    }
 
    /* set the filename <db_name>.dat */
    sprintf(fname_buff, "%s.dat", db_name);
    
    /* open the file for read */
    f = fopen(fname_buff, "rb");
    if (f == NULL) {
        rw_lock_release(db_name);
        return 1;
    }
    
    /* read from file to local_db */
    if (fread(local_db, sizeof(struct Local_db), n, f) == 0) {
        rw_lock_release(db_name);
        fclose(f);
        return 1;
    }
    
    printf("Loadded succesfully from file to local_db!\n");
    fclose(f);
    
    /* release lock */
    rw_lock_release(db_name);
    
    return 0;
}

int db_change(struct Local_db * local_db, int n)
{
    int i, id, isRecordExist = 0;
    char buffer[256];
    
    if (!local_db)
        return 1;
    
    /* scan the record id */
    printf("Enter record id: ");
    scanf("%d", &id);
    
    /* check if record with given id exist */
    for (i = 0; i < n; i++)
        if (local_db[i].id == id) {
            isRecordExist = 1;
            break;
        }
  
    if(!isRecordExist) {
        printf("Record NOT exist:\n");
        return 1;
    }
    
    /* if exist scan the new value */
    printf("Enter new value: ");
    scanf("%s", buffer);
    
    /* check if new value is not to long */
    if (strlen(buffer) > ROW_MAX_LENGTH) {
        printf("Too long word! Max length is: %d\n", ROW_MAX_LENGTH);
        return 1;
    }
    
    /* change the value if no error */
    strcpy(local_db[i].name, buffer);
    
    /*  if success record change 
        -> add record id to table handling records with change */
    if(++noOfChangedRec > 0) {
        if(noOfChangedRec == 1)
            changedRec = (int *)malloc(sizeof(int));
        else 
            changedRec = (int *)realloc(changedRec, noOfChangedRec * sizeof(int));
    
        changedRec[noOfChangedRec - 1] = local_db[i].id;
    }

    printf("local_db updated succesfully!\n");
    
    return 0;
}

int db_update(struct Local_db * local_db, int n, const char * db_name)
{
    char db_nbuff[20], lis_nbuff[20], lisLock_nbuff[20];
    struct Local_db tlocal_db[15];
    int t = 0, i, trec;
    FILE *f_rec, *f;
    #if LOCK_DEBUG==1
    int debug;
    #endif
  
    /* check if lock for proces pid or db_name is set, if yes wait until release */
    sprintf(lisLock_nbuff, "%d", pid);
    if ((rw_lock_check(lisLock_nbuff) != 0) || (rw_lock_check(db_name) != 0)) {
        printf("Listener for PID %d or database '%s'in use, please wait...\n", pid, db_name);
        while((rw_lock_check(lisLock_nbuff) != 0) || (rw_lock_check(db_name) != 0))
            sleep(1);
    }
    
    /* set lock for proces pid listener and database */
    if(rw_lock_set(lisLock_nbuff) != 0)
        return 1;
    if(rw_lock_set(db_name) != 0) {
        rw_lock_release(lisLock_nbuff);
        return 1;
    }
        
    if (!local_db) {
        rw_lock_release(lisLock_nbuff);
        rw_lock_release(db_name);
        return 1;
    }
    
    #if LOCK_DEBUG==1
    printf("\n\033[0;31m LOCK DEBUG 3: (update local db for other prog changes)\n"
    "lock for %d listener and db '%s' is set.\033[0m\n", pid, db_name);
    scanf("%d", &debug);
    #endif
    
    /* open file witch changed records id's */
    sprintf(lis_nbuff, "%d.listener", pid);
    f_rec = fopen(lis_nbuff, "rb");
    if (f_rec == NULL) {
        rw_lock_release(lisLock_nbuff);
        rw_lock_release(db_name);
        return 1;
    }

    /* open file witch database */
    sprintf(db_nbuff, "%s.dat", db_name);
    
    f = fopen(db_nbuff, "rb");
    if (f == NULL) {
        rw_lock_release(lisLock_nbuff);
        rw_lock_release(db_name);
        fclose(f_rec);
        return 1;
    }
    
    /* step through all changed records is's (if exists) */
    while (fread(&trec, sizeof(int), 1, f_rec) != 0) {
        i = 0;
        t = 1;
        /* step through all db records in db file */
        while (fread(tlocal_db, sizeof(struct Local_db), 1, f) != 0) {
            /* compare if changed in num match to id in database */
            if(trec == tlocal_db->id) {
                /* update the record values */
                strcpy(local_db[i].name, tlocal_db->name);
                /* move file handler to the file beginning */
                fseek(f, 0, 0);
                break;
            }
            i++;
        }
    }
  
    fclose(f);
    fclose(f_rec);

    if(t > 0) {
        file_erase(lis_nbuff);
        printf("\n\033[0;33m --- Database '%s' updated by other user." 
        "Your local_db is up to date. ---\033[0m\n", db_name);
    }
    
    /* release the lock for proces pid listener and database */
    rw_lock_release(lisLock_nbuff);
    rw_lock_release(db_name);
    
    return 0;
}



