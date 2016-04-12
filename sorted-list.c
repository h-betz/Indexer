#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sorted-list.h"

//Compares word count for each file
int compareCount(void *c1, void *c2) {
    
    int count1 = *((int *)c1);
    int count2 = *((int *)c2);
    
    if (c1 < c2) {
        return -1;
    } else if (c2 > c1) {
        return 1;
    }
    
    return 0;
}

//Creates a file node that keeps track of a specific word count in that file
FileNode* createFile(char *name, FileNode *nxt) {
    
    FileNode *fn = malloc(sizeof(FileNode));
    
    //Check if allocation of memory was a success
    if (fn == NULL) {
        printf("Error! Failed to allocate memory for a new file node.\n");
        return NULL;
    }

    fn->count = 1;              //Initialize count of word appearing in this file
    fn->name = name;
    fn->next = nxt;
    return fn;
    
}


//A function that creates a node struct and returns a pointer to it
Node* createNode(char *data, Node *nxt, char* file) {
   
    Node * node = malloc(sizeof(Node));                                 //allocates memory for the node
    if (node == NULL) {                                                 //checks to see if malloc successfully allocated memory
        printf("Error! Failed to allocate memory for a new node.\n");
        return NULL;
    }
    
    node->data = data;                                                  //assign the data to the node
    node->next = nxt;                                                   //set the node's next node to nxt
    node->file = createFile(file, NULL);                                //node to point to list of files that contains this word
    
    return node;                                                        //return the node
}


//Creates a sorted list and returns a pointer to it
SortedListPtr SLCreate(CompareFuncT cf) {
    
    SortedListPtr sl = malloc(sizeof(struct SortedList));               //allocate memory for the sorted list pointer struct
    
    //check to see if malloc was a success
    if (sl == NULL) {
        printf("Error! Failed to allocate memory for sorted list pointer.\n");
        return NULL;
    }
    
    sl->cf = cf;                                                        //assign the sorted list's compare function to the one passed
    sl->head = NULL;                                                    //set the head of the list to null
    
    return sl;
}

void DestroyWord(Node *word) {
    
    free(word->data);                       //Free the memory allocated for the word string
    free(word);                             //Free the word node itself
    
}

//Destroys a sorted list pointer
void SLDestroy(SortedListPtr list) {
    
    free(list);
    
    return;
    
}

//allocates memory for a new sorted list iterator
SortedListIteratorPtr SLCreateIterator(SortedListPtr list) {
    
    SortedListIteratorPtr iterator = malloc(sizeof(SortedListIteratorPtr));         //allocates memory for a sorted list iterator
    
    //check to see if the memory was allocated successfully, if not, print an error and return null
    if (iterator == NULL) {
        printf("Failed to create SortedListIteratorPtr.\n");
        return NULL;
    }
    
    iterator->node = list->head;                                                    //sets the iterator node to the head of the sorted list
    
    return iterator;
}

//frees up the memory allocated for a sorted lister iterator pointer
void SLDestroyIterator(SortedListIteratorPtr iter) {
    
    free(iter);
    
    return;
}

//Sorts file nodes in decreasing order of their count
void sortFiles(char *fileName, FileNode *target, FileNode *prev, Node *words) {
    
    //Return if the first file in the list is our target or if its the only file in the list
    if (words->file == target) {
        return;
    }
    if (words->file->next == NULL) {
        return;
    }

    //Switch the head of the file list with our target
    if (target->count >= words->file->count) {
        FileNode *temp = target->next;              //temporary file node
        target->next = words->file;                 //target's next value is the head of the list
        words->file = target;                       //head of the list points to the target
        if (prev != NULL) {                         //If previous node isn't NULL, point to target's old next
            prev->next = temp;
        }
        return;
    }
    
    FileNode *ptr = words->file;
    FileNode *tempPrev = NULL;
        
    //Loop until our target count is greater than our temporary count
    while (target->count < ptr->count) {
        tempPrev = ptr;
        ptr = ptr->next;
    }

    prev->next = target->next;              //Previous node points to next, next file
    target->next = ptr;                     //Target next points to the first file count lower than it
    tempPrev->next = target;                //The temporary previous file points to our target
    
}

//Finds the file with the same name as target for the node word (creates a file node if necessary)
void findFile(Node *word, char* target) {
    
    FileNode *fn = word->file;                      //file node pointing to head of the file list for this word
    FileNode *prev = NULL;
    int comp = 0;                                   //Initialize comparator integer
    
    while (fn != NULL) {                            //Loop through list until the end or until we have a match
        comp = strcmp(fn->name, target);            //Compare our target name with current file node name
        if (comp == 0) {                            //If 0, we have a match
            //we have a match!
            fn->count++;                            //Increment count for this file
            sortFiles(target, fn, prev, word);      //Sort files if necessary 
            //printf("Sorted\n");        
            return;
        } 
        prev = fn;                                  //Have previous file node point to our current one
        fn = fn->next;                              //Have our file node pointer point to the next one
    }
    //File isn't here, so we have to create a node for it
    prev->next = createFile(target, NULL);                  //goes at the end of the list since it has the lowest occurence count
    
}

//inserts the new word item in sorted order
int SLInsert(SortedListPtr  list, char *newObj, char *fileName) {

    int comp = 0;
    //Word list hasn't been created, so we create it with this word as the head
    if (list->head == NULL) {
        list->head = createNode(newObj, NULL, fileName);
        return 1;
    } 
    
    Node *ptr = list->head;
    Node *prev = NULL;
    
    //Loop until the end of the list or until we return
    while (ptr != NULL) {
        comp = list->cf(ptr->data, newObj);
        if (comp > 0) {                                                     //new word, comes before old
            if (prev == NULL) {                                             //New word becomes the head of the list
                prev = createNode(newObj, ptr, fileName);                   //Create a new word node
                prev->next = list->head;                                    //Have it point to the head of the list
                list->head = prev;                                          //Have list head point to our new word node
                return 1;
            }           
            prev->next = createNode(newObj, ptr, fileName);                 //Otherwise, gets inserted in the middle of the list between previous node and our current node
            return 1;
        } else if (comp == 0) {                                             //Word already exists, so we need to find its file
            //printf("Pointer: %s, Word: %s, file: %s\n", ptr->data, newObj, fileName);
            findFile(ptr, fileName);                                        //findn file for this word and increment its count
            return 1;
        }
        
        prev = ptr;
        ptr = ptr->next;
       
    }
    prev->next = createNode(newObj, NULL, fileName);                        //Word doesn't exist in our list, and goes at the end of the list
    return 1;
    
}


//Gets the next data item in our sorted list
char * SLNextItem(SortedListIteratorPtr iter) {
    
    char * item = NULL;                                 //create a void pointer variable to store our target data
    
    if (iter->node == NULL) {                           //if the iterator's node points to null, then we reached the end of the list
        
        return item;
        
    }
    
    item = iter->node->data;                            //our data item is the iterator's current node's data
    iter->node = iter->node->next;                      //have our iterator point to the next node in the list
    return item;
}

char * SLGetItem(SortedListIteratorPtr iter) {
    
    char * item = SLNextItem(iter);                     //call the next item in the sorted list and return that value
    
    return item;
}