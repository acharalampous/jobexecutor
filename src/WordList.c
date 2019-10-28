/*******************************/
/* WordList.c */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <stdio.h>
#include <stdlib.h>
#include "WordList.h"
#include "trie.h"


//** CHAR NODE **//
WLNode* WLN_Init(TrieNode* data){
    WLNode* newNode = malloc(sizeof(WLNode));

    if(newNode == NULL) // error while allocating space
        return NULL;

    // Set newNode data //
    newNode->data = data;
    newNode->next = NULL;
    newNode->previous = NULL;

    return newNode;
}

void WLN_Print(WLNode* node){
    if(node == NULL)
        printf("NULL\n");
    else
        printf("%c -> ",node->data->data);
}



///////////////////
//** WORD LIST **//
///////////////////
WordList* WL_Init(){
    WordList* newList = malloc(sizeof(WordList));
    if(newList == NULL)
        return NULL;

    newList->start = NULL;
    newList->last = NULL;
    newList->capacity = 0;

    return newList;
}


int WL_isEmpty(WordList* list){
    return (list->start == NULL) ? 1 : 0;
}


void WL_Push(WordList* list, WLNode* newNode){
    if(WL_isEmpty(list)){ // empty list
        newNode->next = NULL;
        newNode->previous = NULL;
        list->start = newNode;
        list->last = newNode;
    }
    else{
        newNode->previous = list->last;
        list->last->next = newNode;
        list->last = newNode;
    }

    // increase number of nodes in list //
    list->capacity++;
}


int WL_Pop(WordList* list){
    if(WL_isEmpty(list)) // empty list
        return -1;

    WLNode* temp = list->last;
    if(list->capacity > 1){
        WLNode* newLast = list->last->previous;
        list->last = newLast;
        newLast->next = NULL;
    }
    else{
        list->start = NULL;
        list->last = NULL;
    }

    free(temp);
    list->capacity--;
    return 1;
}


char* WL_GetWord(WordList* list){
    char* word = malloc(sizeof(char) * list->capacity + 1);
    if(word == NULL)
        return NULL;

    word[list->capacity] = '\0';

    WLNode* temp = list->start; // guide through list
    int i = 0;
    while(temp != NULL){
        word[i] = temp->data->data;
        i++;
        temp = temp->next;
    }

    return word;
}


void WL_Print(WordList* list){
    WLNode* node = list->last;
    int nodeNum = 1;
    printf("Printing list: [number of posts: %d]\n", list->capacity);
    while(node != NULL){
        printf("      +Printing post [%d]: ", nodeNum);
        WLN_Print(node);
        node = node->next;
        nodeNum++;
    }
}


void WL_Destroy(WordList* list){
    WLNode* temp = list->last;
    while(temp != NULL){ // until every node is deleted
        temp = temp->previous; // point to next to delete current
        WL_Pop(list); // delete first node
    }
    free(list);
}
