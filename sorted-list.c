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
    if (fn == NULL) {
        printf("Error! Failed to allocate memory for a new file node.\n");
        return NULL;
    }

    fn->count = 1;
    fn->name = name;
    //printf("File: %s\n", fn->name);
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
    //printf("File: %s\n", file);
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

void sortFiles(char *fileName, FileNode *target, FileNode *prev, Node *words) {
    
    if (words->file == target) {
        return;
    }
    if (words->file->next == NULL) {
        return;
    }
    if (target->count > words->file->count) {
        FileNode *temp = target->next;
        target->next = words->file;
        words->file = target;
        if (prev != NULL) {
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
    
    prev->next = target->next;
    target->next = ptr;
    tempPrev->next = target;
    
}

void findFile(Node *word, char* target) {
    
    FileNode *fn = word->file;
    FileNode *prev = NULL;
    int comp = 0;
    
    while (fn != NULL) {
        comp = strcmp(fn->name, target);
        if (comp == 0) {
            //we have a match!            
            fn->count++;
            sortFiles(target, fn, prev, word);            
            return;
        } 
        prev = fn;
        fn = fn->next;
    }
    //File isn't here, so we have to create a node for it
    prev->next = createFile(target, NULL);                  //goes at the end of the list since it has the lowest occurence count
    
}

//inserts the new data item in sorted order
int SLInsert(SortedListPtr  list, char *newObj, char *fileName) {
    //printf("File: %s\n", fileName);
    int comp = 0;
    if (list->head == NULL) {
        list->head = createNode(newObj, NULL, fileName);
        return 1;
    } 
    
    Node *ptr = list->head;
    Node *prev = NULL;
    
    while (ptr != NULL) {
        comp = list->cf(ptr->data, newObj);
        if (comp > 0) {                                                     //new string, comes before old
            if (prev == NULL) {
                prev = createNode(newObj, ptr, fileName);
                prev->next = list->head;
                list->head = prev;
                return;
            }           
            prev->next = createNode(newObj, ptr, fileName);
            return 1;
        } else if (comp == 0) { 
            findFile(ptr, fileName);
            //printf("here\n");
            return 1;
        }
        
        prev = ptr;
        ptr = ptr->next;
       
    }
    prev->next = createNode(newObj, NULL, fileName);
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