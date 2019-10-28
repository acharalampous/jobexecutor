/*******************************/
/* postingList.h */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once

/*  Struct that implements the posting list of a word. Specifically
 *  it holds info about all documents that the word is found in, as
 *  well as how many times in each document.
 */

/* Standard model of list-nodes
 * Specs:   * Single-Linked List
 *          * Pointer at start
 *          * LIFO Insertion at start
 */


/*  List that each node holds the index of line that    */
/*  word is found                                       */
typedef struct linesLN{
    int index; // number of line index in map
    struct linesLN* next;
}linesLN;

/* Implementation of the Posting List node */
typedef struct PLNode{
    int id; // id of text where word is found
    linesLN* lines; // lines of text where word is found
    int times; // times that the word is repeated in text
    struct PLNode* next; // Next node of posting list
}PLNode;

/* Implementation of Posting List */
typedef struct PostingList{
    PLNode* start; // points at the first node of list
    int numOfPosts; // number of posts that the word exists in
}PostingList;



/***************/
/*** linesLN ***/
/***************/
/* All functions about linesLiN start with LLN_ */
linesLN* LLN_Init(int);

/* Takes the start of list and new node, and inserts it at start */
void LLN_insertLine(linesLN**, linesLN*);

/* Given the start of list, it destroys it */
void LLN_Destroy(linesLN**);



/***************/
/*** PL NODE ***/
/***************/
/* All functions about Posting List nodes start with PLN_   */

/* Initializes a new PL Node and returns a pointer to it.   */
/* Takes as parameter the document id. Returns NULL if err. */
PLNode* PLN_Init(int, int);

/* Prints info of given PLNode */
void PLN_Print(PLNode*);

/* Increases time of occurences of given PLNode by one */
void PLN_increaseTimes(PLNode*, int);



/********************/
/*** POSTING LIST ***/
/********************/
/* All functions about Posting List start with PL_ */

/* Initializes a new Posting List and returns a pointer to  */
/* it. Takes as parameter the document id. Returns NULL in  */
/* case of error.                                           */
PostingList* PL_Init();

/* Takes as argument a posting and returns 1 if empty or 0  */
/* if not.                                                  */
int PL_isEmpty(PostingList*);

/* Given a posting l;ist, it finds and returns the index of */
/* the text with the most occurences                        */
int PL_mostOcc(PostingList*, int*);

/* Opposite of above */
int PL_leastOcc(PostingList*, int*);

/* Returns the number of posts of specific posting list     */
int PL_getNumOfPosts(PostingList*);

/* Inserts(pushes) node at start of list */
void PL_Push(PostingList*, PLNode*);

/* Returns nod e with given id. If not found, returns NULL.  */
PLNode* PL_FindNode(PostingList*, int);

/* Checks if start of posting list equals to id */
PLNode* PL_checkStart(PostingList*, int);

/* Deletes(pops) the first node. If list is empty, returns -1 */
int PL_Pop(PostingList*);

/* Destroy given list and all of its nodes. Removes the first */
/* node, until all nodes are deleted                          */
void PL_Destroy(PostingList*);

/* Prints info of given Posting List */
void PL_Print(PostingList*);


