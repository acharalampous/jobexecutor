/*******************************/
/* txInfo.h */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once

/*  Struct that creates a map for all text file managed by worker.
 *  It has a "dynamical" array which keeps info about each text file
 *  and overall details
 */


/*  Info about a specific text file. Full Path, name, size, words and   */
/*  all the lines are kept in it                                        */
typedef struct TFinfo{
    char* fileName; // name of text file
    char* fullPath; // full path of text file
    int numOfBytes; // size of text file
    int numOfWords; // total number of words
    int numOfLines; // total number of lines(containing text)
    char** lines; // all lines
}TFinfo;

/*  Keeps all text Infos    */
typedef struct FMap{
    TFinfo** fileMap;
    int nextCell;
    int tableCapacity;
    int totalWords;
    int totalBytes;
    int totalLines;
}FMap;


/*****************/
/*** TEXT INFO ***/
/*****************/
/* All functions about trie nodes start with TF_ */

/*  Create a new text info, given the name, path, number of lines */
TFinfo* TF_Init(char*, char*, int);

/*  Destroy given text info */
void TF_Destroy(TFinfo*);

/*  Insert new line in given text info  */
int TF_insertLine(TFinfo*, char*, int);

void TF_incBytes(TFinfo*, int);

void TF_incWords(TFinfo*, int);



/*****************/
/*** TEXT INFO ***/
/*****************/
/* All functions about trie nodes start with FM_ */

/*  Create a new fileMap and return pointer to it */
FMap* FM_Init();

/*  Insert new text in fileMap. If not enough space, create */
/*  a bigger table and then store new file info             */
void FM_insertText(FMap*, char*, char*, int);

/*  Insert new line in specific text file info              */
void FM_insertLine(FMap*, char*, int, int);

/*  Increase overall stats, given the index of file info    */
void FM_incAll(FMap*, int);

/*  Destroy given file Info                                 */
void FM_Destroy(FMap*);

