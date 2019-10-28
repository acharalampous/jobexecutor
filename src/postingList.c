/*******************************/
/*postingList.c */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "postingList.h"

/*  Implementation of all functions of Posting List
 *  (PL_), its nodes (PLN_) and lineNodes(LLN_).
 *  Definitions of them found in PostingList.h
 */



/////////////////
//** linesLN **//
/////////////////
linesLN* LLN_Init(int linenum){
    linesLN* newNode = malloc(sizeof(linesLN));

    if(newNode == NULL) // error while allocating space
        return NULL;

    // Set newNode data //
    newNode->index = linenum;
    newNode->next = NULL;

    return newNode;
}

void LLN_insertLine(linesLN** first, linesLN* newNode){
    if((*first) == NULL) // empty list
        newNode->next = NULL;
    else if(newNode->index != (*first)->index) // line does not exist
        newNode->next = *first; // point to current first
    else{ // line already exists in posting list
        free(newNode);
        return;
    }
    (*first) = newNode; // insert at start
}

void LLN_Destroy(linesLN** first){
    linesLN* temp = *first;
    linesLN* toDel;
    while(temp != NULL){ // until every node is deleted
        toDel = temp;
        temp = temp->next; // point to next to delete current
        free(toDel); // delete node
    }

    *first = NULL;
}



///////////////////
//** LIST NODE **//
///////////////////
PLNode* PLN_Init(int id, int linenum){
    PLNode* newNode = malloc(sizeof(PLNode));

    if(newNode == NULL) // error while allocating space
        return NULL;

    // Set newNode data //
    newNode->id = id;
    newNode->times = 1;
    newNode->next = NULL;

    newNode->lines = LLN_Init(linenum);
    if(newNode->lines == NULL){
        free(newNode);
        return NULL;
    }
    //LLN_insertLine(newNode->lines, LLN_Init(linenum));

    return newNode;
}


void PLN_Print(PLNode* node){
    printf("id: %d | times: %d\n", node->id, node->times);
}


void PLN_increaseTimes(PLNode* node, int lineNum){
    node->times++;
    if(node->lines->index != lineNum){
        LLN_insertLine(&node->lines, LLN_Init(lineNum));
    }
}



//////////////////////
//** POSTING LIST **//
//////////////////////
PostingList* PL_Init(){
    PostingList* newList = malloc(sizeof(PostingList));
    if(newList == NULL)
        return NULL; // error during allocation

    newList->start = NULL;
    newList->numOfPosts = 0;

    return newList;
}


int PL_isEmpty(PostingList* list){
    return (list->start == NULL) ? 1 : 0;
}


int PL_mostOcc(PostingList* ps, int* times){
    int maxOcc = 0; // most occurences
    int index = -1;

    PLNode* temp = ps->start; // point at first node
    while(temp != NULL){ // guide through posting list
        if(temp->times > maxOcc){
            index = temp->id;
            maxOcc = temp->times;
        }
        temp = temp->next;
    }
    *times = maxOcc;
    return index;
}

int PL_leastOcc(PostingList* ps, int* times){
    PLNode* temp = ps->start; // point at first node
    int maxOcc = temp->times; // most occurences
    int index = temp->id;

    temp = temp->next;
    while(temp != NULL){ // guide through posting list
        if(temp->times < maxOcc){
            index = temp->id;
            maxOcc = temp->times;
        }
        temp = temp->next;
    }
    *times = maxOcc;
    return index;
}

int PL_getNumOfPosts(PostingList* ps){
    if(ps == NULL)
        return -1;
    else
        return ps->numOfPosts;
}

void PL_Push(PostingList* list, PLNode* newNode){
    if(PL_isEmpty(list)) // empty list
        newNode->next = NULL;
    else // not empty
        newNode->next = list->start; // point to current first

    list->start = newNode; // insert at start

    // increase number of nodes in list //
    list->numOfPosts++;
}


PLNode* PL_FindNode(PostingList* list, int id){
    PLNode* temp; // guide through nodes of list
    temp = list->start;
    while(temp != NULL){ // scan whole list
        int current = temp->id;
        if(current == id){ // post found
            return temp;
        }
        else if(current < id){ // list is sorted in descended order
            break; // so the id is not in list
        }
        else{ // move to next post
            temp = temp->next;
        }
    }
    return NULL; // 404

}

PLNode* PL_checkStart(PostingList* list, int id){
    PLNode* temp = list->start;
    if(temp->id == id){
        return temp;
    }
    else
        return NULL;
}


int PL_Pop(PostingList* list){
    PLNode* temp = list->start;
    if(temp == NULL) // empty list
        return -1;

    list->start = temp->next; // new list's first node
    LLN_Destroy(&temp->lines);
    free(temp);

    // Decrease number of posts //
    list->numOfPosts--;
    return 1; // success
}


void PL_Destroy(PostingList* list){
    PLNode* temp = list->start;
    while(temp != NULL){ // until every node is deleted
        temp = temp->next; // point to next to delete current
        PL_Pop(list); // delete first node
    }

    free(list);
}


void PL_Print(PostingList* list){
    PLNode* node = list->start;
    int nodeNum = 1;
    printf("Printing list: [number of posts: %d]\n", list->numOfPosts);
    while(node != NULL){
        printf("      +Printing post [%d]: ", nodeNum);
        PLN_Print(node);
        node = node->next;
        nodeNum++;
    }
}
