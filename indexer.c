#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int ind = 0;

void tokenize(char *string) {
    
    char c = string[ind];
    char *token = malloc(strlen(string) + 1);
    ind++;
    int tokenIndex = 0;
    int end = strlen(string);
    
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

/*Reads the contents of the file and calls the tokenizer function to tokenize the contents*/
void readFile(char *fileName) {
    
    FILE *f = fopen(fileName, "r");                         //open the file in read mode
    char string[256];                                       //create a 256 byte char array to store the string in
    if (f == NULL) {                                        //if failed to open the file, print error and return
        printf("error\n");
        return;
    }
    fgets(string, 255, (FILE*)f); 
    int length = strlen(string);
    while (ind < length) {   
        tokenize(string);
    }
    fclose(f);
    
}


/*Moves through the directory until it reaches the file or files
  we are looking for
 */
void readDirect(char *path) {
    
    DIR *direct = opendir(path);                                                    //open the directory
    struct dirent *dent;                                                            //create a directory struct
        
    if (direct != NULL) {                                                           //if it's not a directory, our path name might be a file name
        
        while ((dent = readdir(direct)) != NULL) {
            
            if (!strcmp(dent->d_name, "..") || !strcmp(dent->d_name, ".")) {
                //do nothing if we see these symbols which mean previous directory and current directory
            } else if (dent->d_type == DT_REG) {
                
                /*The following appends the file name to the end of the file/directory path*/
                
                char *string = malloc(strlen(dent->d_name) + strlen(path) + 2);
                strcat(string, path);
                strcat(string, "/");
                strcat(string, dent->d_name);
                readFile(string);                                                   //read the contents of the file
                free(string);                                                       //free the memory allocated for the string
                
            } else {
                char *txt = malloc(strlen(path) + strlen(dent->d_name) + 2);
                strcat(txt, path);
                strcat(txt, "/");                
                strcat(txt, dent->d_name);
                readDirect(txt);                  
                free(txt);
            }
        }
        
    } else {
        FILE *f = fopen(path, "r");                                                     //if we are given a file path, try to open the file
        if (f == NULL) {
            printf("Error! Failed to open file!\n");                                    //if failure, print an error and return
            return;
        }
        readFile(path);                                                                 //call the readFile function
    }
    
}


int main (int argc, char **argv) {
    
    char *txt = argv[1];
    int length = strlen(txt);
    readDirect(txt);
    
    
    return 0;
}

