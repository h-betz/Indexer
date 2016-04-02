#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sorted-list.h"

//A function that creates a node struct and returns a pointer to it
Node* createNode(char *data, Node *nxt) {
   
    Node * node = malloc(sizeof(Node));                                 //allocates memory for the node
    if (node == NULL) {                                                 //checks to see if malloc successfully allocated memory
        printf("Error! Failed to allocate memory for a new node.\n");
        return NULL;
    }
    
    node->data = data;                                                  //assign the data to the node
    node->next = nxt;                                                   //set the node's next node to nxt
    
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

//inserts the new data item in sorted order
int SLInsert(SortedListPtr  list, char *newObj) {
    
    int comp = 0;
    
    if (list->head == NULL) {
        list->head = createNode(newObj, NULL);
        return 1;
    } 
    
    Node *ptr = list->head;
    Node *prev = NULL;
    
    while (ptr != NULL) {
        comp = list->cf(ptr->data, newObj);
        if (comp > 0) {                                                     //new string, comes before old
            prev->next = createNode(newObj, ptr);
            return 1;
        } else if (comp == 0) {
            ptr->next = createNode(newObj, ptr->next);
            return 1;
        }
        prev = ptr;
        ptr = ptr->next;
       
    }
    
    prev->next = createNode(newObj, NULL);
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