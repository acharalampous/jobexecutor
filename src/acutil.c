/*******************************/
/* acutils.c */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aclib.h"

/*  All functions implementions that are defined in aclib.h */

int getNumOfPaths(char* fileName){
    size_t buffsize = 256; // will be used for getline
    FILE* fp = fopen(fileName, "r"); // open file

    if(fp == NULL)
        return -1;

    char* line = malloc(sizeof(char) * buffsize); // for getline
    int numOfPaths = 0;
    while(!feof(fp)){
		getline(&line, &buffsize, fp); // read line from file
        if(feof(fp))
            break;

        char* path = discardSpaces(line); // clear all tabs and spaces in front of id
        if(!strcmp(path, "\n")){ // line had only tabs, spaces and \n
            continue;
        }
        numOfPaths++;
    }
    free(line);
    fclose(fp);
    return numOfPaths;
}


char* discardSpaces(char* src){
    char* delim = src;
    while(*delim != '\n'){
        if(*delim == '\t' || *delim == ' ' )
            delim++;
        else
            break;
    }

    return delim;
}

char* myStrCopy(char* src, int srclength){
    char* dest = NULL;
    dest = malloc((srclength + 1) * sizeof(char));
    dest[srclength] = '\0';

    memcpy(dest, src, srclength);
    return dest;
}

int chInStr(char* str, char ch){
    int i = 0;

    while(str[i] != '\0'){
        if(str[i] == ch)
            return 1;
        i++;
    }
    return 0;
}

int isNumber(char* str){
    char* temp = str;
    char ch = *temp;
    while(ch != '\0'){
        if(ch >= 48 && ch <= 57){
            ch = *(++temp);
        }
        else{
            return 0;
        }
    }
    return 1;
}

void exeParameters(){
    printf("[ERR] Invalid Parameters given. Execute again with the following options:\n");
    printf("\t./jobExecutor -d docFile -w W\n");
    printf("\t./jobExecutor -d docFile\n");
    printf("\t./jobExecutor -w W -d docFile\n");
    printf("-docFile Name of text file containing the paths of documents\n");
    printf("-W: Number of workers to create\n");
}

void printOptions(){
    putchar('\n');
    putchar('\n');
    printf("Choose one of the following options:\n");
    printf("-------------------------------------\n");
    printf("(1) /search q1 q2 ... q10 -d deadline");
    printf(" [Make a search for the relative documents]\n");
    printf("(2) /maxcount q");
    printf(" [Find the text file with the most occurences of the word]\n");
    printf("(3) /mincount q");
    printf(" [Find the text file with the least occurences of the word]\n");
    printf("(4) /wc");
    printf(" [Print total number of bytes, words and lines]\n");
    printf("(5) /exit");
    printf(" [Exit the application]\n");
}

