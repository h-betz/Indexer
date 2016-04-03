#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "sorted-list.h"

int ind = 0;
SortedListPtr words = NULL;

int compareStrings(void *c1, void *c2) {
    
    char *str1 = (char *) c1;
    char *str2 = (char *) c2;
    
    return strcmp(str1, str2);

}


//Parses the string line into separate words, or tokens
void tokenize(char *string, char *fileName) {
    
    char c = string[ind];
    char *token = malloc(strlen(string) + 1);
    token[0] - '\0';
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
        SLInsert(words, token, fileName);
    }

    
}

/*Reads the contents of the file and calls the tokenizer function to tokenize the contents*/
void readFile(char *filePath, char *fileName) {
    
    FILE *f = fopen(filePath, "r");                         //open the file in read mode
    char string[1024];                                      //create a 1024 byte char array to store the string in
    if (f == NULL) {                                        //if failed to open the file, print error and return
        printf("error\n");
        return;
    }
    
    while (fgets(string, sizeof(string), f)) {
        int length = strlen(string);
        while (ind < length) {
            tokenize(string, fileName);
        }
        ind = 0;
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
                readFile(string, dent->d_name);                                                   //read the contents of the file
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
        //readFile(path);                                                                 //call the readFile function
    }
    
}

void writeToFile() {
    
    //Iterate through the words
    while (words->head != NULL) {
        
        printf("%s\n", words->head->data);
        
        //Iterate through the files of the word
        while (words->head->file != NULL) {
            printf("\t%s: %d\n", words->head->file->name, words->head->file->count);
            FileNode *fn = words->head->file;
            words->head->file = words->head->file->next;
            free(fn);
        }
        
        Node *word = words->head;
        words->head = words->head->next;
        free(word);
        
    }
    
}


int main (int argc, char** argv) {
    
    char *path = argv[1];
    
    int (*compare)(void *, void *) = compareStrings;
    
    words = SLCreate(compare);
    readDirect(path);
    writeToFile();
    /*SortedListIteratorPtr iter = SLCreateIterator(words);
    char *str = SLGetItem(iter);
    while (str != NULL) {
        printf("%s\n", str);
        str = SLGetItem(iter);
    }
    SLDestroyIterator(iter);*/
    
    /*while (words->head != NULL) {
        Node *n = words->head;
        free(n);
        words->head = words->head->next;
    }*/
    
    
    return 0;
}