/*******************************/
/* dirsMap.h */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once

/*  Struct that implements a list that holds all the directory
 *  paths of a worker process. Each node holds a path to a
 *  directory
 */

/* Standard model of list-nodes
 * Specs:   * Single-Linked List
 *          * Pointer at start
 *          * LIFO Insertion at start
 */


/* Implementation of the Directories Map node */
typedef struct DMNode{
    char* path; // path to specific directory
    struct DMNode* next; // Next node of posting list
}DMNode;



/***************/
/*** DM NODE ***/
/***************/
/* All functions about Directory Map nodes start with DMN_   */

/* Initializes a new DM Node and returns a pointer to it.   */
/* Takes as parameter the path string. Returns NULL if err. */
DMNode* DMN_Init(char*);

/* Prints info of given DMNode */
void DMN_Print(DMNode*);

/* Takes as argument the first node of list, and returns 1  */
/* if empty or 0 if not.                                    */
int DMN_isEmpty(DMNode*);

/* Inserts(pushes) node at start of list */
void DMN_Push(DMNode**, DMNode*);

/* Deletes(pops) the first node. If list is empty, returns -1 */
int DMN_Pop(DMNode**);

/* Destroy given list and all of its nodes. Removes the first */
/* node, until all nodes are deleted                          */
void DMN_Destroy(DMNode*);

/* Prints info of given DM */
void DMN_Print(DMNode*);


