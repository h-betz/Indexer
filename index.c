#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include "sorted-list.h"

int ind = 0;                    //Index to keep track of where we are at in the string we are tokenizings
SortedListPtr words = NULL;     //our SortedList to keep track of our words

//Compares strings and returns the result
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

    token[0] = '\0';                            //null terminate our token string
    ind++;
    int tokenIndex = 0;                         //initialize our token index
    
    //Our tokens need to start with a letter
    if (isalpha(c)) {
        c = tolower(c);                         //convert character to lower case
        token[tokenIndex] = c;                  //add it to our token
        tokenIndex++;                           //increment our token index
        c = string[ind];                        //get next character from our string
        ind++;                                  //increment the index of our string
        
        //Iterate through the string as long as it's not a null character or as long as its alphanumeric
        while (c != '\0' && isalnum(c)) {
            token[tokenIndex] = tolower(c);         //convert character to lowercase, add it to token
            tokenIndex++;                           //incrment our token's index
            c = string[ind];                        //get next character in the string we are tokenizing
            ind++;                                  //increment the index in our string we are tokenizing
        }

        token[tokenIndex] = '\0';                   //null terminate the token
        SLInsert(words, token, fileName);           //insert the word or increment it's occurrence if necessary
    }

    
}

/*Reads the contents of the file and calls the tokenizer function to tokenize the contents*/
void readFile(char *filePath, char *fileName, char *subdirec) {
    
    FILE *f = fopen(filePath, "r");                         //open the file in read mode
    char string[1024];                                      //create a 1024 byte char array to store the string in
    if (f == NULL) {                                        //if failed to open the file, print error and return
        printf("Error reading file: %s\n", fileName);
        return;
    }
    
    while (fgets(string, sizeof(string), f)) {              //Read contents of the file to the end
        int length = strlen(string);                        //Get length of current string buffer
        while (ind < length) {                              //While the index is less than the length of the string, continue tokenizing
            if (subdirec != NULL) {                         //If subdirec is not null, pass the subdirectory filepath as the file name
                tokenize(string, subdirec);
            } else {
                tokenize(string, fileName);
            }
        }
        ind = 0;                                            //We have a new string, so index goes back to 0
    }

    fclose(f);                                              //Close the file
    
}


/*Moves through the directory until it reaches the file or files
  we are looking for
 */
void readDirect(char *path, char *sub_path) {
   
    DIR *direct = opendir(path);                                                    //open the directory
    struct dirent *dent;                                                            //create a directory struct
        
    if (direct != NULL) {                                                           //if it's not a directory, our path name might be a file name
        
        while ((dent = readdir(direct)) != NULL) {
            if (!strcmp(dent->d_name, "..") || !strcmp(dent->d_name, ".")) {
                //do nothing if we see these symbols which mean previous directory and current directory
            } else if (dent->d_type == DT_REG) {
                
                //The following appends the file name to the end of the file/directory path
                char path_new[255];
                
                //If this file is part of a subdirectory in the file path we chose, do the following
                if (sub_path != NULL) {
                    char subdirec[255];                                                                 //Create a subdirectory array
                    int x = snprintf(subdirec, sizeof(subdirec)-1, "%s/%s", sub_path, dent->d_name);    //Copy previous subdirectory path into our new one with the current file name appended
                    int len = snprintf(path_new, sizeof(path_new)-1, "%s/%s", path, dent->d_name);      //Append file name to our current file path
                    subdirec[254] = 0;                                                                  //Null terminate our subdirec string
                    path_new[len] = 0;                                                                  //Null terminate our new path string
                    readFile(path_new, dent->d_name, subdirec);                                         //Read contents of the file
                    
                } else {                                                                                //Otherwise file is in the directory we provided
                    int len = snprintf(path_new, sizeof(path_new)-1, "%s/%s", path, dent->d_name);      //Append file name to our current file path
                    path_new[len] = 0;                                                                  //Null terminate our string
                    readFile(path_new, dent->d_name, sub_path);                                         //read the file
                }
                
            } else if (dent->d_type == DT_DIR) {                                                        //Otherwise this is a directory
                char path_new[255];                                                                     //Path to store the file path            
                char extra_path[255];                                                                   //Variable to store subdirectory path
                int x = snprintf(extra_path, sizeof(extra_path)-1, "/%s", dent->d_name);                //Append a / and subdirectory name to our subdirectory variable
                int len = snprintf(path_new, sizeof(path_new)-1, "%s/%s", path, dent->d_name);          //Append the subdirectory name to our current file path
                path_new[len] = 0;                                                                      //Null terminate our file path string
                readDirect(path_new, extra_path);                                                       //Read the contents of the subdirectory
            }
        }
        
    } else {
        FILE *f = fopen(path, "r");                                                     //if we are given a file path, try to open the file
        if (f == NULL) {
            printf("Error! Failed to open file: %s\n", path);                                    //if failure, print an error and return
            return;
        }
        char *fileName;
        fileName = basename(path);                                                      //Gets the name of the file without the added parents directories
        readFile(path, fileName, sub_path);                                             //Read contents of the file
    }
    
}

//Writes contents from memory to the file the user specified
void writeToFile(char *new_file) {
    
    FILE *fp = fopen(new_file, "wb");
    fputs("{\"list\":[\n", fp);
    
    //Iterate through the words
    while (words->head != NULL) {
        
        fprintf(fp, "\t{\"%s\":[\n", words->head->data);
        
        //Iterate through the files of the word
        while (words->head->file != NULL) {
            
            //If next file is not null, write with specific format (comma appended)
            if (words->head->file->next != NULL) {
                fprintf(fp, "\t\t{\"%s\": %d},\n", words->head->file->name, words->head->file->count);  
            } else {
                fprintf(fp, "\t\t{\"%s\": %d}\n", words->head->file->name, words->head->file->count);   //comma is ommitted
            } 
             
            FileNode *fn = words->head->file;               //Get current file
            words->head->file = words->head->file->next;    //Move to next file
            free(fn);                                       //Free file node from memory
        }
        
        Node *word = words->head;                           //Get word node
        words->head = words->head->next;                    //Move to next word in the list
        DestroyWord(word);                                  //Free word node 
        
    }
    fprintf(fp, "\t]}\n");
    fprintf(fp, "]}\n");
    fclose(fp);                                             //Close file
    
}


int main (int argc, char** argv) {
    
    char *path = argv[2];                               //Get directory or file name to read
    char *new_file = argv[1];                           //Get name of the file user wants to write to 
    
    int (*compare)(void *, void *) = compareStrings;    //Create our comparator function
    
    words = SLCreate(compare);                          //Create our sorted list
    readDirect(path, NULL);                             //read the files in the directory and in any subdirectories
    writeToFile(new_file);                              //write contents to the file 
    SLDestroy(words);
    
    return 0;
}