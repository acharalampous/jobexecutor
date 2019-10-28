/*******************************/
/* WordList.h */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once
#include "trie.h"

/*  Struct that implements a list that each node of it
 *  holds a trieNode. All the nodes in order will form
 *  a word. Plays the role of a "stack" to help us find
 *  all words in a trie and also to help us destroy the
 *  the trie. Create use for DFS throught trie
 */

/* Standard model of list-nodes
 * Specs:   * Double-Linked List
 *          * Pointer at start and end
 *          * FIFO Insertion at end
 */

/* Implementation of the WordList node */
typedef struct WLNode{
    TrieNode* data; // pointer to trieNode
    struct WLNode* previous; // previous node of word list
    struct WLNode* next; // Next node of word list
}WLNode;

/* Implementation of WordList */
typedef struct WordList{
    WLNode* start; // points at the first node
    WLNode* last; // points at the last node
    int capacity; // number of characters in "word"
}WordList;



/***************/
/*** WL NODE ***/
/***************/
/* All functions about WordList nodes start with WLN_   */

/* Initializes a new WL Node and returns a pointer to it.   */
/* Takes as an existing TrieNode. Returns NULL if err.      */                         //
WLNode* WLN_Init(TrieNode*);

/* Prints info of given WLNode */
void WLN_Print(WLNode*);



/*****************/
/*** WORD LIST ***/
/*****************/
/* All functions about Word List start with WL_ */

/* Initializes a new Word List and returns a pointer to */
/* it. Returns NULL in case of error.                   */
WordList* WL_Init();

/* Takes as argument a word list and returns 1 if empty and */
/* 0 if not.                                                */
int WL_isEmpty(WordList*);

/* Inserts(pushes) node at end of list */
void WL_Push(WordList*, WLNode*);

/* Deletes(pops) the last node. If list is empty, returns -1 */
int WL_Pop(WordList*);

/* Read all nodes, form and return the word stored in list. */
char* WL_GetWord(WordList*);

void WL_Print(WordList*);

// Destroy given list //
void WL_Destroy(WordList*);
