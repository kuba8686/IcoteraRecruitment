#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>

#include "libs/simpledb.h"

#define DB_NOOFRECORDS 15
#define DB_NAME "names"

int printMenu(bool);

int main(void) {
    
    struct Local_db *local_db;
    int n = DB_NOOFRECORDS, menuOpt;
    bool unsChg = false, showMenu = true;
    
    /* alocate memory for local_db structure */
    local_db = (struct Local_db *)malloc(n * sizeof(struct Local_db));
    
    /* welcome message */
    printf("Welcome to SimpleDB v1.0 by Jakub Bartoszewicz\n");
    
    /* initialize the database */
    if (db_init(local_db, n, DB_NAME, (int)getpid()) != 0) {
        printf("Error initialize database. Exiting the program.\n");
        free(local_db);
        return 1;
    } 
    
    /* Main Menu loop */
    while (showMenu) {
        
        menuOpt = printMenu(unsChg);
        /*  check if another program make any changes
            and update local_db if needed */
        if(db_update(local_db, n, DB_NAME) != 0)
            printf("Update database by other process failed.\n");
        switch (menuOpt) {
            /* show database */
            case 1:  
                if (db_show(local_db, n) != 0)
                    printf("Error showing database '%s' content.\n", DB_NAME);
            break;
            /* make changes in database */
            case 2:  
                if (db_change(local_db, n) == 0) 
                    unsChg = true; 
                else 
                    printf("Error change database '%s' content.\n", DB_NAME);
            break;
            /* reload the database (from file) */
            case 3:  
                if (db_read(local_db, n, DB_NAME) == 0) 
                    unsChg = false;
                else 
                    printf("Error read database '%s' content.\n", DB_NAME);
            break;
            /* write local changes and inform other process about changes */
            case 4:  
                switch(db_write(local_db, n, DB_NAME)) {
                    case 0: 
                        unsChg = false;
                    break;
                    case 1:
                        printf("Error write to database '%s' file.\n", DB_NAME);
                    break;
                    case 2: 
                        printf("Error inform other process about changes in '%s'.\n", DB_NAME);
                    break;    
                }
            break;
            /* exit discarding changes */
            case 5:  
                showMenu = false; 
            break;
            /* exit, save and inform about changes */
            case 6:  
                switch(db_write(local_db, n, DB_NAME)) {
                    case 0: 
                        showMenu = false;
                    break;
                    case 1:
                        printf("Error write to database '%s' file.\n", DB_NAME);
                    break;
                    case 2: 
                        printf("Error inform other process about changes in '%s'.\n", DB_NAME);
                    break;    
                }
            break;
            /* wrong choice */
            default: 
                printf("Wrong choice. Try again.\n");
            break;
        }
    }
    

    /* free lib memory, delete <pid>.listener, update all.listener */
    db_close();

    /* free local_db structure */
    free(local_db);
    
    /* exit message */
    printf("Exiting program!\n");
    
    return 0;
}

int printMenu(bool ch)
{
    int o;
    
    printf("\nMAIN MENU: %d\n\n", (int)getpid());
    
    printf("1.\tShow database content \t(db_show)\n");
    printf("2.\tChange the record value (db_change)\n");
    printf("3.\tReload the database \t(db_read)\n");
    printf("4.\tSave db and inform other process about changes \t(db_write)\n");
    printf("5.\tExit and DISCARD local changes\n");
    printf("6.\tExit and SAVE local changes\n");
    
    /* prompt if unsaved changes */
    if(ch)
        printf("\n\033[0;36m --- YOU HAVE UNSAVED CHANGES! ---\033[0m\n\n");
    
    printf("Chose option: ");
    scanf("%d", &o);
    printf("\n");
    
    return o;
}