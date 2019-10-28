/*******************************/
/* trie.c */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "trie.h"
#include <termios.h>
#include <sys/ioctl.h>
#include "WordList.h"

/*  Implementation of all functions of Trie(Trie_)
 *  and its nodes (TN_). Definitions of them
 *  found in Trie.h
 */

///////////////////
//** TRIE NODE **//
///////////////////
TrieNode* TN_Init(char newChar){
    TrieNode* newNode = malloc(sizeof(TrieNode));

    if(newNode == NULL) // error while allocating space
        return NULL;

    // Set newNode data //
    newNode->data = newChar;
    newNode->next = NULL;
    newNode->child = NULL;
    newNode->LeafNode = 0; // not a leafnode (at least yet)
    newNode->ps = NULL; // does not have a posting list(at least yet)

    return newNode;
}


void TN_Print(TrieNode* node){
    if(node == NULL){
        printf("NULL\n");
    }
    else{
        printf("%c[%d] -> ",node->data,node->LeafNode);
        TN_Print(node->next);
        TN_Print(node->child);
    }
}


void TN_setNext(TrieNode* node, TrieNode* newNext){
    node->next = newNext;
}


void TN_setLeaf(TrieNode* node){
    node->LeafNode = 1;
}


char TN_getData(TrieNode* node){
    if(node == NULL)
        return -1; // null pointer return err
    else
        return node->data;
}


TrieNode* TN_InsertChar(TrieNode* start, char newChar, int* result){
    TrieNode* temp = start; // guide through list
    TrieNode* next;
    TrieNode* rNode; // the node to be returned
    TrieNode* last; //

    if(temp == NULL){ // empty list
        rNode = TN_Init(newChar); // create new node and return *Must change beginning of the list from NULL//
        *result = 0;
        return rNode; // exit code 0, means that the start of list must be changed
    }
    else if(TN_getData(temp) == newChar){
        rNode = temp;
        *result = 1;
        return rNode; // exit code 1 means that node was found
    }
    else if(TN_getData(temp) > newChar){ // must insert newchar at start
        rNode = TN_Init(newChar);
        TN_setNext(rNode, start);
        *result = 0;

        return rNode; // exit code 0, means that the start of list must be changed
    }
    else{ // guide through list
        last = temp;
        next = temp->next;
        while(next != NULL){
            if(TN_getData(next) == newChar){ // char already exists
                rNode = next;
                *result = 1;
                return rNode; // exit code 1 means that node was found
            }
            else if(TN_getData(next) > newChar){ // char does not exist
                rNode = TN_Init(newChar);
                TN_setNext(temp, rNode);
                TN_setNext(rNode, next);

                *result = 2;
                return rNode; // exit code 2, means that new node was placed(not at start)
            }
            temp = temp->next;
            last = temp;
            next = temp->next;
        }
        rNode = TN_Init(newChar);
        TN_setNext(last, rNode);
        //TN_setNext(rNode, NULL);

        *result = 2;
        return rNode; // node was placed at end
    }
}


// Insert new posting list entry in leaf node //
int TN_NewPost(TrieNode* leafNode, int id, int lineNum){
    if(leafNode->ps == NULL){ // has no posting list yet
        leafNode->ps = PL_Init(); // so create one
        PL_Push(leafNode->ps, PLN_Init(id, lineNum));
        return 0;
    }

    PLNode* result = PL_checkStart(leafNode->ps, id); // check if text already in posting list
    if(result != NULL) // if it exists in posting list
        PLN_increaseTimes(result, lineNum); // increase number of occurences
    else
        PL_Push(leafNode->ps,PLN_Init(id, lineNum)); // create new entry

    return 1;
}


void TN_Destroy(TrieNode* node){
    if(node->ps != NULL)
        PL_Destroy(node->ps);
    free(node);
}



//////////////
//** TRIE **//
//////////////
Trie* Trie_Init(){
    Trie* newTrie = malloc(sizeof(Trie));
    if(newTrie == NULL) // error while allocating space
        return NULL;

    newTrie->start = NULL;

    return newTrie;
}


int Trie_Insert(Trie* trie, char* newWord, int id, int lineNum){
    if(newWord == NULL || trie == NULL) // invalid given parameters
        return -1;
    TrieNode** nextList = &(trie->start); // point at next list to insert.
    //Note: Point at the pointer itself, so we can change its value if the beginning of list changes //
    int rInsertion; // result of insertion
    TrieNode* leafNode;
    char currentChar = *newWord;

    while(currentChar != '\0'){
        leafNode = TN_InsertChar(*nextList, currentChar, &rInsertion); //rNode: returned(result) Node
        if(rInsertion == 0){ // was placed at start
            *nextList = leafNode; // change the start of current level's child list
        }

        currentChar = *(++newWord); // point to next character of string
        nextList = &(leafNode->child); // point to the start of next list
    }
    TN_setLeaf(leafNode);
    TN_NewPost(leafNode, id, lineNum);
    return 1;
}


void Trie_findWordInDoc(Trie* trie, int id, char* word){
    PostingList* pl = Trie_findWord(trie, word);
    if(pl == NULL){
        printf("Word \"%s\" does not exist in index!\n",word);
    }
    else{
        PLNode* plNode = PL_FindNode(pl, id);
        if(plNode == NULL){
            printf("Word \"%s\" does not exist in document with id %d!\n",word, id);
        }
        else{
            printf("%d %s %d\n", id, word, plNode->times);
        }
    }
}

void Trie_printWord(Trie* trie, char* word){
    PostingList* ps = Trie_findWord(trie, word);
    if(ps == NULL){
        printf("Word \"%s\" does not exist in index!\n",word);
    }
    else{
        printf("%s %d\n",word, PL_getNumOfPosts(ps));
    }
}


PostingList* Trie_findWord(Trie* trie, char* word){
    if(word == NULL || trie == NULL) // invalid given parameters
        return NULL;

    TrieNode* currentNode = trie->start;
    char currentChar = *word;

    while(currentChar != '\0' && currentNode != NULL){ // until end of word
        if(currentNode->data < currentChar){
            currentNode = currentNode->next; // point to the next node
        }
        else if(currentNode->data == currentChar){
            currentChar = *(++word); // point to next character of string
            if(currentChar != '\0')
                currentNode = currentNode->child; // point to the next node
        }
        else
            return NULL;
    }
    if(currentNode != NULL)
        return currentNode->ps;
    else
        return NULL;
}

void Trie_allWords(Trie* trie){
    WordList* wordlist = WL_Init();
    WL_Push(wordlist, WLN_Init(trie->start));
    WLNode* temp = wordlist->start;
    int phase = 0;

    while(wordlist->start != NULL){
        if(phase == 0){
            TrieNode* child = temp->data->child;
            if(temp->data->LeafNode == 1){
                PostingList* ps = temp->data->ps;
                int num = ps->numOfPosts;
                char* word = WL_GetWord(wordlist);
                printf("%s %d\n", word,num);
                free(word);
            }
            if(child != NULL){ // has child
                WL_Push(wordlist,WLN_Init(child));
                temp = wordlist->last;
            }
            else{
                phase = 1;
            }
        }
        else{
            TrieNode* sibling = temp->data->next;
            if(sibling != NULL){
                WL_Pop(wordlist);
                WL_Push(wordlist, WLN_Init(sibling));
                temp = wordlist->last;
                phase = 0;
            }
            else{
                WL_Pop(wordlist);
                temp = wordlist->last;
                phase = 1;
            }
        }
    }
    WL_Destroy(wordlist);
}

//void Trie_searchKeyWords(Trie* tr, DocMap* dMap, char* keyWords, int K){
//    int totalDocs = dMap->numOfDocs;
//
//    /* Create a heap table that will hold all heapDocs */
//    heapDoc** hDocs = malloc(sizeof(heapDoc*) * totalDocs);
//    for(int i = 0; i < totalDocs; i++)
//        hDocs[i] = NULL;
//
//
//    char* temp = myStrCopy(keyWords, strlen(keyWords));
//    char* toFree = temp;
//    int numberOfKeys = 0;
//
//    /* Get number of keywords, to store them in a table */
//    temp = strtok(temp, " \t\n");
//    while(temp != NULL){
//        numberOfKeys++;
//        if(numberOfKeys == 10) // reached limit of 10 words
//            break;
//        temp = strtok(NULL, " \t\n");
//    }
//
//    free(toFree);
//
//    /* Create table that will store all keyWords */
//    char** keys = malloc(sizeof(char*) * numberOfKeys);
//
//    temp = myStrCopy(keyWords, strlen(keyWords));
//    toFree = temp;
//
//    /* Store all words and get the scores of all documents that */
//    /* contain the specific words                               */
//    temp = strtok(temp, " \t\n");
//    for(int i = 0; i < numberOfKeys; i++){
//        keys[i] = myStrCopy(temp, strlen(temp));
//
//        PostingList* pl = Trie_findWord(tr, keys[i]); // get posting list of word
//        if (pl != NULL) // if it exists
//            PL_getScores(pl, hDocs, dMap); // update all scores of documents
//
//        temp = strtok(NULL, " \t\n");
//    }
//
//    free(toFree);
//
//    /* Sort the table of scores using heapsort */
//    int HeapLast = dMap->numOfDocs - 1;
//    S_HeapSort(hDocs, HeapLast);
//    for(int i = 1; i <= K; i++){
//
//        /* Get the maximum score which is at hDocs[0] */
//        heapDoc* maxDoc = S_GetMax(hDocs, &HeapLast);
//        if(maxDoc != NULL)
//            S_PrintResult(maxDoc, dMap, keys, numberOfKeys, i);
//        else
//            break;
//    }
//
//    for(int i = 0; i < numberOfKeys; i++){
//        free(keys[i]);
//    }
//    free(keys);
//
//    for(int i = 0; i < totalDocs; i++){
//        free(hDocs[i]);
//    }
//    free(hDocs);
//}


void Trie_Print(Trie* trie){
    printf("Printing Trie:\n");
    TN_Print(trie->start);
}


void Trie_Destroy(Trie* trie){
    if(trie->start == NULL){
        free(trie);
        return;
    }
    WordList* wordlist = WL_Init();
    WL_Push(wordlist, WLN_Init(trie->start));
    WLNode* temp = wordlist->start;
    int phase = 0;

    while(wordlist->start != NULL){
        if(phase == 0){
            TrieNode* child = temp->data->child;
            if(child != NULL){ // has child
                WL_Push(wordlist,WLN_Init(child));
                temp = wordlist->last;
            }
            else{
                phase = 1;
            }
        }
        else{
            TrieNode* sibling = temp->data->next;
            if(sibling != NULL){
                TN_Destroy(temp->data); // destroy trienode
                WL_Pop(wordlist); // destroy wordlist node
                WL_Push(wordlist, WLN_Init(sibling));
                temp = wordlist->last;
                phase = 0;
            }
            else{
                TN_Destroy(temp->data); // destroy trienode
                WL_Pop(wordlist);
                temp = wordlist->last;
                phase = 1;
            }
        }
    }
    WL_Destroy(wordlist);
    free(trie);
}
