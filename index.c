#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
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
    int length = strlen(fileName) + 1;
    char *file = malloc(strlen(fileName) + 1);
    strcpy(file, fileName);
    file[length - 1] = '\0';
    
    token[0] - '\0';
    ind++;
    int tokenIndex = 0;
    
    //Our tokens need to start with a letter
    if (isalpha(c)) {
        c = tolower(c);
        token[tokenIndex] = c;
        tokenIndex++;
        c = string[ind];
        ind++;
        
        while (c != '\0' && isalnum(c)) {
            token[tokenIndex] = tolower(c);
            token[tokenIndex] = c;
            tokenIndex++;
            c = string[ind];
            ind++;
        }

        token[tokenIndex] = '\0';
        SLInsert(words, token, file);
    }

    
}

/*Reads the contents of the file and calls the tokenizer function to tokenize the contents*/
void readFile(char *filePath, char *fileName) {
    
    FILE *f = fopen(filePath, "r");                         //open the file in read mode
    char string[1024];                                      //create a 1024 byte char array to store the string in
    if (f == NULL) {                                        //if failed to open the file, print error and return
        printf("Error reading file: %s\n", fileName);
        return;
    }
    
    while (fgets(string, sizeof(string), f)) {
        int length = strlen(string);
        while (ind < length) {
            tokenize(string, filePath);
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
                //The following appends the file name to the end of the file/directory path
                char path_new[255];
                int len = snprintf(path_new, sizeof(path_new)-1, "%s/%s", path, dent->d_name);
                path_new[len] = 0;
                readFile(path_new, dent->d_name);
                
            } else if (dent->d_type == DT_DIR) {
                char path_new[255];
                int len = snprintf(path_new, sizeof(path_new)-1, "%s/%s", path, dent->d_name);
                path_new[len] = 0;
                readDirect(path_new);
            }
        }
        
    } else {
        FILE *f = fopen(path, "r");                                                     //if we are given a file path, try to open the file
        if (f == NULL) {
            printf("Error! Failed to open file: %s\n", path);                                    //if failure, print an error and return
            return;
        }
        char *fileName;
        fileName = basename(path);
        readFile(path, fileName);
    }
    
}

void writeToFile(char *new_file) {
    
    FILE *fp = fopen(new_file, "wb");
    fputs("{\"list\":[\n", fp);
    
    //Iterate through the words
    while (words->head != NULL) {
        
        fprintf(fp, "\t{\"%s\":[\n", words->head->data);
        
        //Iterate through the files of the word
        while (words->head->file != NULL) {
            
            if (words->head->file->next != NULL) {
                fprintf(fp, "\t\t{\"%s\": %d},\n", words->head->file->name, words->head->file->count);  
            } else {
                fprintf(fp, "\t\t{\"%s\": %d}\n", words->head->file->name, words->head->file->count); 
            } 
             
            FileNode *fn = words->head->file;
            words->head->file = words->head->file->next;
            free(fn);
        }
        
        Node *word = words->head;
        words->head = words->head->next;
        free(word);
        
    }
    fprintf(fp, "\t]}\n");
    fprintf(fp, "]}\n");
    fclose(fp);
    
}


int main (int argc, char** argv) {
    
    char *path = argv[2];
    char *new_file = argv[1];
    
    int (*compare)(void *, void *) = compareStrings;
    
    words = SLCreate(compare);
    readDirect(path);
    writeToFile(new_file);    
    
    return 0;
}