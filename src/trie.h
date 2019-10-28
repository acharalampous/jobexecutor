/*******************************/
/* trie.h */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once
#include "postingList.h"

/*  Struct that implements the trie structure with lists  */

/* Implementation of Trie Node. Each Nodes has a character as data */
typedef struct TrieNode{
    char data; // the ASCII character that the node is holding
    struct TrieNode* next; // Next node of current list
    struct TrieNode* child; // Pointer to the first node of the child's list
    int LeafNode; // Is it LeafNode(end of word), yes: 1 - no: 0
    PostingList* ps; // keeps a list of all documents that the word is found
}TrieNode;

/* Implementation of Trie head */
typedef struct Trie{
    TrieNode* start; // start of Trie tree
}Trie;




/*****************/
/*** TRIE NODE ***/
/*****************/
/* All functions about trie nodes start with TN_ */

/* Initializes a new Trie Node and returns a pointer to it  */
/* Takes as arguments the char that will hold as data       */
/* Returns NULL if err.                                     */
TrieNode* TN_Init(char);

/* Prints info about given trienode */
void TN_Print(TrieNode*);

/* Sets a new node as next of specific node */
void TN_setNext(TrieNode*, TrieNode*);

/* Sets a node as leaf Node */
void TN_setLeaf(TrieNode*);

/* Returns data stored in node. Returns -1 if err */
char TN_getData(TrieNode*);

/* Searchs for specific char in specific list,  */
/* given the start of it. If found, the node is */
/* returned. If not found it is placed according*/
/* to alphabetical orded(ascended). If it is    */
/* placed at start, result(parameter) returns   */
/* with the value 0, so the pointer of parent   */
/* must be changed.                             */
TrieNode* TN_InsertChar(TrieNode*, char, int*);

/* Insert new posting list entry in given leaf node */
int TN_NewPost(TrieNode*, int, int);

/* Given a pointer to specific node, it gets destroyed */
void TN_Destroy(TrieNode*);



/************/
/*** TRIE ***/
/************/
/* All functions about trie start with Trie_ */

/* Initalizes new trie and returns pointer to it */
Trie* Trie_Init();

/* Inserts new word in trie. Returns -1 if error */
int Trie_Insert(Trie*, char*, int, int);

/* Uses DFS algorithm in order to find all words in trie */
/* In order to avoid recursion, a while loop is used.    */
/* To achieve this, each node has 2 phases. At the first */
/* phase it is checked if it is a leaf node and then if  */
/* there are any children to move to them. At the second */
/* phase it is checked if there are any siblings. This   */
/* way the DFS algorithm is done with repetitive way     */
void Trie_allWords(Trie*);

/* Search specific word in trie and print occurences(/df)*/
/* If it exists, first it gets the posting list and      */
/* prints number of occurences. If not found, it prints  */
/* error message(404)                                    */
void Trie_printWord(Trie*, char*);

/* Search specific word in trie and print occurences(/tf)*/
/* in specific document with given id. First it searches */
/* for specific word in posting list and then for entry  */
/* with given document id and prints number of           */
/* occurences. If not found, it prints error message(404)*/
void Trie_findWordInDoc(Trie*, int, char*);


/* Searches in trie for given word. If founds it returns */
/* its posting list, else returns NULL                   */
PostingList* Trie_findWord(Trie*, char*);

void Trie_Print(Trie*);

/* Destroy the whole trie */
void Trie_Destroy(Trie*);


/* Given K and a number of keywords(max 10), prints the */
/* most relevant documents, according to the keywords   */
//void Trie_searchKeyWords(Trie*, DocMap*, char*, int K);
