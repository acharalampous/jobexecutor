#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>

#include "aclib.h"
#include "txInfo.h"
#include "trie.h"
#include "dirsMap.h"

/* Worker process. Takes as argument the number of process(i) and
 * the two pipeNames for read and write.
 */

int textIndex = 0; // index for mapping text
volatile sig_atomic_t W_readCommand = 0; // worker needs to read command
volatile sig_atomic_t W_exitSig = 0; // worker must exit


void sigHandlerW(int signo){
    if(signo == SIGUSR1)
        W_readCommand = 1;
    else if(signo == SIGUSR2)
        W_exitSig = 1;
}

int worker(int i, char* pNameR, char* pNameW, DMNode* paths){
    static struct sigaction sigsW;
    memset(&sigsW, 0, sizeof(sigsW));

    sigsW.sa_handler = sigHandlerW;

    sigemptyset(&(sigsW.sa_mask));
    sigaddset(&(sigsW.sa_mask), SIGUSR1);
    sigaddset(&(sigsW.sa_mask), SIGUSR2);
    sigaction(SIGUSR1, &sigsW, NULL);
    sigaction(SIGUSR2, &sigsW, NULL);

    char logFile[20]; // name of the logfile

    FMap* fileMap = FM_Init();
    Trie* tr = Trie_Init();

    int pWrite = open(pNameW, O_WRONLY);
    int pRead = open(pNameR, O_RDONLY);
    if(pRead < 0 || pWrite < 0){
        printf("[%d] Error opening pipes.Abort\n", getpid());
        close(pRead);
        close(pWrite);
        return -1;
    }

    if(paths == NULL){ // worker must get his paths from JE
        int donePaths = 0;
        while(!donePaths){
            W_getPath(pRead, pWrite, fileMap, tr, &donePaths);
        }
        sprintf(logFile, "log/Worker_%d",getpid());
    }
    else{ //** (Not ready because of deadline, would do part of the W_getPath and open its files) **//
        int len = -1;
        len = read(pRead, logFile, 20);
        logFile[len] = '\n';

        write(pWrite, logFile, 4);
        /// CREATE PATHS ///

        sprintf(logFile, "log/Worker_%s",logFile);
    }

/////////////////////////////////////////////////////////////////////////////////


    FILE* logF = fopen(logFile, "ab");
    time_t t;
    struct tm* tm_info;

    char command[1024];

    int totalS = 0; // total strings found in files of worker
    while(1){
        W_exitSig = 0; // cancel search command
        int exitSGN = 0; // exit worker
        int option;
        int flag = 0; // 0: current text must print all details, 1: must not write, 2: current text must write only path


        W_getCommand(pRead, pWrite, command);
        time(&t); // set time that command was received
        tm_info = localtime(&t);

        if (!(strncmp(command, "/search ", 8))) option = 1;

		else if (!(strncmp(command, "/maxcount ", 10))) option = 2;

		else if (!(strncmp(command, "/mincount ", 10))) option = 3;

		else if (!(strncmp(command, "/wc", 3))) option = 4;

		else if (!(strncmp(command, "/exit", 5))) option = 5;

		else{continue;} // read command again


        switch(option){
            case 1:{

                char* keyWords = command + 8; // overpass /search and point to parameters
                discardSpaces(keyWords);
                char* token = myStrCopy(keyWords, strlen(keyWords));
                char* toFree = token;
                token = strtok(token, " \t\n");
                while(token != NULL){ // get number of words to check deadline
                    if(W_exitSig == 1){
                        break;
                    }
                    PostingList* ps = Trie_findWord(tr, token); // get posting list of word
                    if(ps == NULL){ //
                        token = strtok(NULL, " \t\n"); // get next keyWord
                        continue;
                    }

                    PLNode* currText = ps->start;
                    while(currText != NULL){ // get results for all texted containing word
                        if(W_exitSig == 1){ // JE deadline reached
                            break;
                        }

                        int textIndex = currText->id;
                        linesLN* tempLine = currText->lines;
                        while(tempLine != NULL){ // get all lines of text
                            if(W_exitSig == 1){
                                break;
                            }

                            int lineIndex = tempLine->index;
                            char fullBuff[8192];
                            strcpy(fullBuff, fileMap->fileMap[textIndex]->fullPath);
                            strcat(fullBuff, "/");
                            strcat(fullBuff, fileMap->fileMap[textIndex]->fileName);
                            strcat(fullBuff, " - ");

                            char lineId[5];
                            sprintf(lineId, "%d", lineIndex);
                            strcat(fullBuff, lineId);
                            strcat(fullBuff, ": ");
                            strcat(fullBuff, fileMap->fileMap[textIndex]->lines[lineIndex]); // create full Answer
                            write(pWrite, fullBuff, strlen(fullBuff));

                            char discard[10];
                            read(pRead, discard, 10); // wait acknowledgment

                            TFinfo* currFile = fileMap->fileMap[textIndex];
                            if(flag == 0){
                                totalS++;
                                char dt[26]; // date and time
                                strftime(dt, 26, "%d-%m-%Y %H:%M:%S", tm_info);
                                fprintf(logF, "%s: /search: %s: %s/%s", dt, token, currFile->fullPath, currFile->fileName);
                                fflush(logF);
                                flag = 1; // next path must not be printed with date and time
                            }
                            else if(flag == 2){ // write only pathn
                                fprintf(logF, ": %s/%s", currFile->fullPath, currFile->fileName);
                                fflush(logF);
                                flag = 1;
                            }

                            tempLine = tempLine->next; // get to next line
                        }
                        currText = currText->next; // get next text

                        if(flag == 1) // changed text
                            flag = 2;

                    }

                    token = strtok(NULL, " \t\n");
                    if(flag != 0){
                        fprintf(logF, "\n");
                        fflush(logF);
                    }
                    flag = 0;

                }
                char msg[10] = "~~!//";
                write(pWrite, msg, strlen(msg));
                char discard[10];
                read(pRead, discard, 3);

                free(toFree);
                break;
            } // end case 1
            case 2:{
                char* word = command + 10;
                discardSpaces(word); // find the word after /maxcount

                if(word != NULL){
                    PostingList* ps = Trie_findWord(tr, word);
                    if(ps == NULL){
                        char msg[4] = "NIL"; // inform JE that word is not found
                        write(pWrite, msg, strlen(msg));
                        break;
                    }
                    totalS++;

                    int maxTimes = 0; // most times word occured in text
                    int maxIndex = PL_mostOcc(ps, &maxTimes); // get text with most occurences of word

                    char timesStr[15];
                    sprintf(timesStr, "%d", maxTimes);

                    char* textName = fileMap->fileMap[maxIndex]->fileName;
                    char* pathName = fileMap->fileMap[maxIndex]->fullPath;
                    int fullSize = strlen(textName) + strlen(pathName) + strlen(timesStr) + 3;
                    char* fullPath = malloc(sizeof(char) * fullSize);

                    strcpy(fullPath, pathName);
                    strcat(fullPath, "/");
                    strcat(fullPath, textName);
                    strcat(fullPath, " ");
                    strcat(fullPath, timesStr); // create full result

                    TFinfo* currFile = fileMap->fileMap[maxIndex];
                    char dt[26]; // date and time
                    strftime(dt, 26, "%d-%m-%Y %H:%M:%S", tm_info);
                    fprintf(logF, "%s: /maxcount: %s: %s/%s: %s\n", dt, word, currFile->fullPath, currFile->fileName, timesStr);
                    fflush(logF);

                    write(pWrite, fullPath, fullSize); // send result
                    free(fullPath);
                }

                break;
            } // end case 2
            case 3:{
                char* word = command + 10;
                discardSpaces(word); // find the word after /df
                if(word != NULL){
                    PostingList* ps = Trie_findWord(tr, word);
                    if(ps == NULL){
                        char msg[4] = "NIL";
                        write(pWrite, msg, strlen(msg));
                        break;
                    }

                    totalS++;
                    int maxTimes = 0;
                    int maxIndex = PL_leastOcc(ps, &maxTimes);

                    char timesStr[15];
                    sprintf(timesStr, "%d", maxTimes);

                    char* textName = fileMap->fileMap[maxIndex]->fileName;
                    char* pathName = fileMap->fileMap[maxIndex]->fullPath;
                    int fullSize = strlen(textName) + strlen(pathName) + strlen(timesStr) + 3;
                    char* fullPath = malloc(sizeof(char) * fullSize);


                    strcpy(fullPath, pathName);
                    strcat(fullPath, "/");
                    strcat(fullPath, textName);
                    strcat(fullPath, " ");
                    strcat(fullPath, timesStr);

                    TFinfo* currFile = fileMap->fileMap[maxIndex];
                    char dt[26]; // date and time
                    strftime(dt, 26, "%d-%m-%Y %H:%M:%S", tm_info);
                    fprintf(logF, "%s: /mincount: %s: %s/%s: %s\n", dt, word, currFile->fullPath, currFile->fileName, timesStr);
                    fflush(logF);

                    write(pWrite, fullPath, fullSize);
                    free(fullPath);
                }
                break;
            } // end case 3
            case 4:{
                char wc[30];
                sprintf(wc, "%d %d %d", fileMap->totalBytes, fileMap->totalWords, fileMap->totalLines);
                write(pWrite, wc, strlen(wc));

                char dt[26]; // date and time
                strftime(dt, 26, "%d-%m-%Y %H:%M:%S", tm_info);

                fprintf(logF, "%s: /wc: %s\n", dt, wc);
                fflush(logF);

                break;
            }
            case 5:{
                exitSGN = 1; // exit signal
                break;
            }
        }// end switch:
        if(exitSGN == 1)
            break;
	}

    FM_Destroy(fileMap);
    Trie_Destroy(tr);
    fclose(logF);
    close(pRead);
    close(pWrite);
    return totalS;

}


void W_getPath(int pRead, int pWrite, FMap* fmap,Trie* tr, int* done){
    char pathName[1024];
    FILE* fd;
    size_t buffsize = 256; // will be used for getline
	char* line = malloc(sizeof(char) * buffsize); // for getline

    int len = read(pRead, pathName, 1024); // read path from JE
    if(len < 0){
        printf("ERR\n");
    }

    pathName[len] = '\0';
    write(pWrite, pathName, strlen(pathName)); // ACK

    if(!strcmp(pathName, "////")){
        *done = 1;
        free(line);
        return;
    }
    /* HERE I GOT A PATHNAME. MUST OPEN ALL OF ITS FILE */

    DIR* dir = opendir(pathName);
    if(dir == NULL){
        printf("ERR dir\n");
    }
    while(1){
        struct dirent* dt = readdir(dir); // get next text file
        if(dt != NULL){
            if(dt->d_ino == 0){ // if not a text file continue
                continue;
            }
        }
        else
            break;

        if((!strcmp(dt->d_name, ".")) || (!strcmp(dt->d_name, "..")))
            continue;
        char* fullName = malloc(sizeof(char) * (strlen(dt->d_name) + strlen(pathName)+ 2));
        strcpy(fullName, pathName);
        strcat(fullName, "/");
        strcat(fullName, dt->d_name); // create full Path/Name

        if((fd = fopen(fullName, "r")) < 0){
            printf("[%d]inv file\n", getpid());
            free(fullName);
            return;
        }
        else{
            /* Get number of lines in file */
            int lines = 0;
            while(!feof(fd)){
                getline(&line, &buffsize, fd); // read line from file
                if(feof(fd))
                    break;

                char* line1 = discardSpaces(line); // clear all tabs and spaces in front of id
                if(!strcmp(line1, "\n")){ // line had only tabs, spaces and \n
                    continue;
                }
                else
                    lines++;
            }

            fseek(fd, 0, SEEK_SET); // found number of lines, point at start

            FM_insertText(fmap, dt->d_name, pathName, lines);

            int numOfLine = 0;
            while(!feof(fd)){
                getline(&line, &buffsize, fd); // read line from file
                TF_incBytes(fmap->fileMap[textIndex], strlen(line));
                if(feof(fd))
                    break;

                char* line1 = discardSpaces(line); // clear all tabs and spaces in front of id
                if(!strcmp(line1, "\n")){ // line had only tabs, spaces and \n
                    continue;
                }
                else
                    FM_insertLine(fmap, line1, numOfLine, textIndex);

               	char* token = myStrCopy(line1, strlen(line1));
               	char* toFree = token; // will be used to free token space
                token = strtok(token, " \t\n");

                int totalWords = 0;
                while(token != NULL) {
                	Trie_Insert(tr, token, textIndex, numOfLine); // insert word in trie
                    totalWords++;
                    token = strtok(NULL, " \t\n");
                }
                TF_incWords(fmap->fileMap[textIndex], totalWords);

                numOfLine++;
                free(toFree);
            }
            FM_incAll(fmap, textIndex);
            textIndex++;
            free(fullName);
            fclose(fd);
        }
    }
    free(dir);
    free(line);
}

void W_getCommand(int pRead, int pWrite, char* command){
    int len = -1;
    int flag = 0;
    while(1){
        len = read(pRead, command, 1023);
        if(len == -1){
            continue;
        }
        command[len] = '\0';
        if (flag == 1){
            W_exitSig = 0; // reset exit(if it was changed by deadline in JE)
            break;
        }

        if(!strcmp(command, "##Get Ready to read command##")){
            char ack[15] = "##READY##";
            write(pWrite, ack, strlen(ack));

            flag = 1; // next reading is the command
        }
    }
}
