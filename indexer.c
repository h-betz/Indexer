#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void readDirect(char *path) {
    
    DIR *direct = opendir(path);
    struct dirent *dent;
        
    if (direct != NULL) {
        
        while ((dent = readdir(direct)) != NULL) {
            
            if (!strcmp(dent->d_name, "..") || !strcmp(dent->d_name, ".")) {
            
            } else if (dent->d_type == DT_REG) {
                printf("\t[%s]\n", dent->d_name);              
            } else {
                char *txt = malloc(strlen(path) + strlen(dent->d_name) + 2);
                strcat(txt, path);
                strcat(txt, "/");                
                strcat(txt, dent->d_name);
                readDirect(txt);                  
                free(txt);
            }
            
            
        }
        
    }
    
}

void printFiles(DIR *d, struct dirent *dent) {
    
    printf("\t[%s]\n", dent->d_name);
    while ((dent = readdir(d)) != NULL) {
        if (dent->d_type == DT_REG) {
            printf("\t[%s]\n", dent->d_name); 
        }
    }
    
    
}

int main (int argc, char **argv) {
    
    char *txt = argv[1];
        
    readDirect(txt);
    
    
    return 0;
}

