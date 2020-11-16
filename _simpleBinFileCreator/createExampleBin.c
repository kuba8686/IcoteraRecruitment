#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOOFRECORDS 15
#define FILENAME "names.dat"

struct Local_db{ int id; char name[21]; };

int main(int argc, char const *argv[]) {
    
    FILE * f;
    struct Local_db local[NOOFRECORDS];
    int i, n = NOOFRECORDS;
    char *names[] = {   "Boleslaw", 
                        "Wladyslaw", 
                        "Stanislaw", 
                        "Alojzy", 
                        "Kunegunda",
                        "Adrianna",
                        "Matylda", 
                        "Agnieszka", 
                        "Wladyslawa", 
                        "Dominika", 
                        "Edyta",
                        "Jakub",
                        "Michal",
                        "Janusz",
                        "Grzegorz"};
    
    for (i = 0; i < n; i++) {
        local[i].id = i + 1;
        strcpy(local[i].name, names[i]);
    }
    
    /* WRITE */
     
    f = fopen(FILENAME, "wb");
    if (f == NULL)
        return 1;
        
    if (fwrite(local, sizeof(struct Local_db), n, f) == 0)
        return 1;
    
    fclose(f);
    
    return 0;
}