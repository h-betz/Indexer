#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int ind = 0;

/*char * tokenizer(char *string) {
    
    
}*/

void tokenize(char *string) {
    
    char c = string[ind];
    char *token = malloc(strlen(string) + 1);
    ind++;
    int tokenIndex = 0;
    
    if (isalpha(c)) {
        
        token[tokenIndex] = c;
        tokenIndex++;
        c = string[ind];
        ind++;
        
        while (c != '\0' && isalnum(c)) {
            token[tokenIndex] = c;
            tokenIndex++;
            c = string[ind];
            ind++;
        }
        
        token[tokenIndex] = '\0';
        printf("%s\n", token);
    }
    
    free(token);
    
}

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


int main (int argc, char **argv) {
    
    char *txt = argv[1];
    int length = strlen(txt);
    printf("%d\n", length);
    while (ind < length) {
        tokenize(txt);            
    }
    //readDirect(txt);
    
    
    return 0;
}

