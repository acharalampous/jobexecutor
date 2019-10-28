/*******************************/
/*dirsMap.c */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dirsMap.h"

/*  Implementation of all functions of Directory Map
 *  nodes (DMN_). Definitions of them found in dirsMap.h
 */



/////////////////
//** DM NODE **//
/////////////////
DMNode* DMN_Init(char* path){
    DMNode* newNode = malloc(sizeof(DMNode));

    if(newNode == NULL) // error while allocating space
        return NULL;

    newNode->path = malloc(sizeof(char) * (strlen(path) + 1));

    // Set newNode data //
    strcpy(newNode->path, path);
    newNode->next = NULL;

    return newNode;
}

void DMN_Print(DMNode* node){
    printf("path: %s\n", node->path);
}


int DMN_isEmpty(DMNode* first){
    return (first == NULL) ? 1 : 0;
}

void DMN_Push(DMNode** first, DMNode* newNode){
    if(DMN_isEmpty(*first)) // empty list
        newNode->next = NULL;
    else // not empty
        newNode->next = *first; // point to current first

    *first = newNode; // insert at start
}

int DMN_Pop(DMNode** first){
    DMNode* temp = *first;
    if(temp == NULL) // empty list
        return -1;

    *first = temp->next; // new list's first node

    free(temp->path); // delete allocated string
    free(temp); // delete node

    return 1; // success
}

void DMN_Destroy(DMNode* first){
    int res = 1;
    while(res == 1){ // until every node is deleted
        res = DMN_Pop(&first); // delete first node
    }
}
