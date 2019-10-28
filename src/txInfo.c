#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "txInfo.h"

TFinfo* TF_Init(char* name, char* path, int numOfLines){
    TFinfo* newTF = malloc(sizeof(TFinfo));
    if(newTF == NULL) // error while allocating space
        return NULL;

    newTF->fileName = malloc(sizeof(char) * (strlen(name) + 1));
    if(newTF->fileName == NULL) // error while allocating space
        return NULL;

    newTF->fullPath = malloc(sizeof(char) * (strlen(path) + 1));
    if(newTF->fullPath == NULL) // error while allocating space
        return NULL;

    newTF->lines = malloc(sizeof(char*) * numOfLines);
    if(newTF->lines == NULL) // error while allocating space
        return NULL;

    for(int i = 0; i < numOfLines; i++)
        newTF->lines[i] = NULL;


    // Set newNode data //
    strcpy(newTF->fileName, name);
    strcpy(newTF->fullPath, path);
    newTF->numOfBytes = 0;
    newTF->numOfWords = 0;
    newTF->numOfLines = numOfLines;

    return newTF;
}


int TF_insertLine(TFinfo* tf, char* newLine, int i){
    if(tf->lines[i] != NULL)
        return -1; // something went wrong. Cell is occupied
    else{
        tf->lines[i] = malloc(sizeof(char) * strlen(newLine) + 1);
        if(tf->lines[i] == NULL) // error while allocating space
            return -2;
    }

    strcpy(tf->lines[i], newLine);
    return 0;
}

void TF_incBytes(TFinfo* tf, int num){
    if(tf == NULL){
        return;
    }

    tf->numOfBytes += num;
}

void TF_incWords(TFinfo* tf, int num){
    if(tf == NULL){
        return;
    }

    tf->numOfWords += num;
}

void TF_Destroy(TFinfo* tf){
    free(tf->fileName);
    free(tf->fullPath);

    for(int i = 0; i < tf->numOfLines; i++){
        if(tf->lines[i] != NULL)
            free(tf->lines[i]);
    }
    free(tf->lines);
    free(tf);
}

///////////////////////////////////////////////////////
FMap* FM_Init(){
    FMap* newMap = malloc(sizeof(FMap));
    if(newMap == NULL) // error while allocating space
        return NULL;

    // Set newNode data //
    newMap->tableCapacity = 1;
    newMap->nextCell = 0;
    newMap->totalBytes = 0;
    newMap->totalWords = 0;
    newMap->totalLines = 0;

    newMap->fileMap = malloc(sizeof(TFinfo*) * newMap->tableCapacity);
    if(newMap->fileMap == NULL){
        free(newMap);
        return NULL;
    }

    return newMap;
}

void FM_Destroy(FMap* fmap){
    for(int i = 0; i < (fmap->nextCell); i++)
        TF_Destroy(fmap->fileMap[i]);

    free(fmap->fileMap);
    free(fmap);
}

void FM_insertText(FMap* fmap, char* fileName, char* path, int numOfLines){
    if(fmap == NULL || fileName == NULL || path == NULL) // inv arguments
        return;

    if((fmap->nextCell + 1) > fmap->tableCapacity){ // table is full
        fmap->tableCapacity *= 2; // double its capacity
        TFinfo** newInfo = malloc(sizeof(TFinfo*) * fmap->tableCapacity);
        for(int i = 0; i < fmap->tableCapacity / 2; i++){
            newInfo[i] = fmap->fileMap[i]; // copy from first table
        }
        TFinfo** toDel = fmap->fileMap;
        fmap->fileMap = newInfo; // point to new table
        free(toDel);
    }

    fmap->fileMap[fmap->nextCell] = TF_Init(fileName, path, numOfLines);
    fmap->nextCell++; // show next cell to write
}

void FM_insertLine(FMap* fmap, char* newline, int lineID, int textID){
    if(fmap == NULL || newline == NULL || textID < 0) // inv arguments
        return;

    TF_insertLine(fmap->fileMap[textID], newline, lineID);
}

void FM_incAll(FMap* fmap, int index){
    fmap->totalBytes += fmap->fileMap[index]->numOfBytes;
    fmap->totalWords += fmap->fileMap[index]->numOfWords;
    fmap->totalLines += fmap->fileMap[index]->numOfLines;
}
