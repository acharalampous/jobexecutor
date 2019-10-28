/*******************************/
/* aclib.h */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once

#include <signal.h>
#include "dirsMap.h"
#include "txInfo.h"
#include "trie.h"

/*  Header file for all variant functions and structs used
 *  to complete the job Executor application.
 */

extern volatile sig_atomic_t activeWorkers;
extern volatile sig_atomic_t endOfTime;

/*  Job Executor process. Takes as argument the number of workers,
 *  all the opened pipes and the name of the file with paths
 */
int jobExecutor(int, int*, char*);

/* Used for the search command, in order the JE can read multiple   */
/* answers from all workers                                         */
void JE_getReady(int , int*, int*, int);

/* Used from JE to make sure all workers are ready for new command  */
void JE_informCmd(int, int*, int*, int*, DMNode**);



/* Worker process. Takes as argument the number of process(i) and
 * the two pipeNames for read and write.
 */
int worker(int, char*, char*, DMNode*);

/* Worker gets path of directory of text files from JE. For each    */
/* path, it opens every text file and creates a Trie                */
void W_getPath(int, int, FMap*, Trie*, int*);


/* Get query command from JE                                        */
void W_getCommand(int, int, char*);



/*  Given its name, it opens the file, finds number of paths*/
/*  and returnes them. Finally it closes the file           */
int getNumOfPaths(char*);


/*  Overpass the spaces( /t/n) in string and return pointer */
/*  to it                                                   */
char* discardSpaces(char* src);

/*  strcpy Alternative. Allocates spaces and returns pointer*/
/*  to the copied string                                    */
char* myStrCopy(char*, int);

/*  Returns 0 if character exists in string, else 1         */
int chInStr(char* str, char ch);

/*  Given a string, it check char-char to see if integer.   */
/*  Is yes, returns 1, else 0.                              */
int isNumber(char* str);

/*  Print options(/search,/df,/tf/exit..)                   */
void printOptions();


/*  Print the valid form of given parameters                */
void exeParameters();
