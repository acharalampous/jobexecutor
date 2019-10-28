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

#include "aclib.h"
#include "dirsMap.h"

int jobExecutor(int w, int* chPid, char* fileName){

    char pNameW[15];
    char pNameR[15];

    DMNode** dirMaps = malloc(sizeof(DMNode*) * w); // mapping pids <-> paths

    /*  Read and open all pipes for R and W */
    int* pipesR = malloc(sizeof(int) * w); // used to store all pipes for R
    int* pipesW = malloc(sizeof(int) * w); // used to store all pipes for W
    for(int i = 0; i < w; i++){
        char pI[5];
        sprintf(pI, "%d", i);
        strcpy(pNameW, "Pipe");
        strcpy(pNameR, "Pipe");

        strcat(pNameR, pI);
        strcat(pNameW, pI);

        strcat(pNameR, "[R]");
        strcat(pNameW, "[W]"); // create name of pipes to open

        pipesR[i] = open(pNameR, O_RDONLY);
        pipesW[i] = open(pNameW, O_WRONLY); // open both pipes for R and W
        dirMaps[i] = NULL;
    }

    FILE* fp = fopen(fileName, "r");
    if(fp == NULL){
        printf("[ERR] Error while opening file. File might not exist. Abort.\n");
        free(pipesR);
        free(pipesW);
        for(int i = 0; i < w; i++){
            DMN_Destroy(dirMaps[i]);
        }
        free(dirMaps);
        return -1;
    }

    size_t buffsize = 256; // will be used for getline
	char* line = malloc(sizeof(char) * buffsize); // for getline
	int i = 0;

	/* Send to workers their paths */
	while(!feof(fp)){
		getline(&line, &buffsize, fp); // read line from file
        if(feof(fp))
            break;

        char* path = discardSpaces(line); // clear all tabs and spaces in front of id
        if(!strcmp(path, "\n")){ // line had only tabs, spaces and \n
            continue;
        }

        path = strtok(path, "\n");
        write(pipesW[i], path, strlen(path) + 1);

        char ansBuff[1024];

        read(pipesR[i], ansBuff, 1024); // get confirmation from worker
        DMN_Push(&dirMaps[i], DMN_Init(path)); // keep path in map

        i = (i + 1) % w; // find next worker to be contacted
    }

    fclose(fp);

    char buff[10] = "////"; // specific string will inform worker that all paths were given

    /* Inform workers that all paths are given */
    for(int i = 0; i < w; i++){
        write(pipesW[i], buff, strlen(buff));
        read(pipesR[i], buff, 1024);
    }

    //////////////////////
    // GETTING COMMANDS //
    //////////////////////

    int succCmd = 1; // previous command was succesfull
    while(1){

        if(succCmd == 1){ // avoid checking if workers are ready every time
            JE_informCmd(w, chPid, pipesR, pipesW, dirMaps);
            succCmd = 0;
        }

        int exitSGN = 0;

        int option;

        printOptions();


        printf("jobExecutor>");
        fflush(stdout);
        getline(&line, &buffsize, stdin);
        line = strtok(line, "\n");

        while(line == NULL){
            printf("jobExecutor>");
            fflush(stdout);
            getline(&line, &buffsize, stdin);
            line = strtok(line, "\n");
        }


        if(line[0] != '/'){
            printf("Invalid option. Command must begin with \"/\"\n");
            continue;
        }

        if (!(strncmp(line, "/search ", 8))) option = 1;

		else if (!(strncmp(line, "/maxcount ", 10))) option = 2;

		else if (!(strncmp(line, "/mincount ", 10))) option = 3;

		else if (!(strncmp(line, "/wc", 3))) option = 4;

		else if (!(strncmp(line, "/exit", 5))) option = 5;

		else{printf("Invalid command given.\n"); continue;}


        switch(option){
            case 1:{
                char* keyWords = line + 8; // overpass /search and point to parameters
                discardSpaces(keyWords);
                if(keyWords == NULL){
                    printf("Invalid option given.\n");
                    break;
                }

                int wordslen = strlen(keyWords);
                char* command = malloc(sizeof(char) * strlen(line) + 1);
                strcpy(command, "/search");

                char* token = myStrCopy(keyWords, wordslen);
                char* toFree = token; // will be used to free string, after tokenize

                int numOfKeyWords = 0;
                token = strtok(token, " \n");
                while(token != NULL){ // get number of words to check deadline
                    numOfKeyWords++;
                    token = strtok(NULL, " \n");
                }
                free(toFree);

                int i = numOfKeyWords;
                token = myStrCopy(keyWords, strlen(keyWords));
                toFree = token;
                token = strtok(token, " \n");

                /* Check for command validity */
                int deadline = -1;
                while(i--){
                    if(i == 1){ // check for -d parameter
                        if(strcmp(token, "-d"))
                            deadline = -1;
                    }
                    else if(i == 0){ // last word must be a number
                        if(!isNumber(token)){
                            deadline = -1;
                        }
                        else
                           deadline = atoi(token);

                    }
                    else{
                        strcat(command, " ");
                        strcat(command, token);
                    }
                    token = strtok(NULL, " \n");
                }
                if(deadline <= 0){
                    printf("Invalid deadline given\n");
                }

                else{

                    for(int i = 0; i < w; i++){  // send the command to every worker
                        write(pipesW[i], command, strlen(command));
                    }

                    succCmd = 1;
                    JE_getReady(w, pipesR, pipesW, deadline); // get results from workers
                }

                free(toFree);
                free(command);
                break;
            } // end case 1
            case 2:{
                    char* word = line + 10;
                    discardSpaces(word); // find the word after /maxcount
                    if(word == NULL){
                        break;
                    }
                    else{
                        for(int i = 0; i < w; i++){
                            write(pipesW[i], line, strlen(line));
                        }
                        succCmd = 1;

                        int maxOcc = -1; // max occurences of word
                        char* maxText = NULL; // text with max occurences
                        char resBuff[1024] = "";
                        int len = 0;

                        for(int i = 0; i < w; i++){
                            len = read(pipesR[i], resBuff, 1024);
                            resBuff[len] = '\0';
                            if(!strcmp(resBuff, "NIL")) // worker did not found word
                                continue;

                            char* resToken = strtok(resBuff, " ");
                            char* currText = myStrCopy(resToken, strlen(resToken)); // get text
                            resToken = strtok(NULL, " ");
                            int currOcc = atoi(resToken); // get number of occurences

                            if(currOcc >= maxOcc){
                                if(currOcc > maxOcc){ // if current text has more occurences of word
                                    if(maxText != NULL){
                                        free(maxText);
                                    }
                                    maxText = myStrCopy(currText, strlen(currText));
                                    maxOcc = currOcc;

                                }
                                else{ // same occurences
                                    if(maxText != NULL){
                                        if(strcmp(currText, maxText) > 0){ // check alphabetically
                                            free(maxText);
                                            maxText = myStrCopy(currText, strlen(currText));

                                            maxOcc = currOcc;
                                        }
                                    }
                                    else{
                                        maxText = myStrCopy(currText, strlen(currText));
                                        maxOcc = currOcc;
                                    }
                                }
                            }
                            free(currText);

                        }
                        if(maxOcc != -1){
                            printf("\n%s %d\n", maxText, maxOcc);
                            free(maxText);
                        }
                        else
                            printf("\nWord was not found\n");
                }
                break;
            } // end case 2
            case 3:{
                    char* word = line + 10;
                    discardSpaces(word); // find the word after /df
                    if(word == NULL){
                        break;
                    }
                    else{
                        for(int i = 0; i < w; i++){
                            write(pipesW[i], line, strlen(line));
                        }
                        succCmd = 1;

                        int leastOcc = -1;
                        char* maxText = NULL;
                        char resBuff[1024] = "";
                        int len = 0;

                        for(int i = 0; i < w; i++){
                            len = read(pipesR[i], resBuff, 1024);
                            resBuff[len] = '\0';
                            if(!strcmp(resBuff, "NIL"))
                                continue;

                            char* resToken = strtok(resBuff, " ");
                            char* currText = myStrCopy(resToken, strlen(resToken));
                            resToken = strtok(NULL, " ");
                            int currOcc = atoi(resToken);
                            if(i == 0){
                                leastOcc = currOcc;
                                maxText = myStrCopy(currText, strlen(currText));
                            }
                            else if(currOcc <= leastOcc){
                                if(currOcc < leastOcc){
                                    if(maxText != NULL){
                                        free(maxText);
                                    }
                                    maxText = myStrCopy(currText, strlen(currText));
                                    leastOcc = currOcc;

                                }
                                else{ // same occurences
                                    if(maxText != NULL){
                                        if((strcmp(currText, maxText)) < 0){
                                            free(maxText);
                                            maxText = myStrCopy(currText, strlen(currText));

                                            leastOcc = currOcc;
                                        }
                                    }
                                    else{
                                        maxText = myStrCopy(currText, strlen(currText));
                                        leastOcc = currOcc;
                                    }
                                }
                            }
                            free(currText);
                        }
                        if(leastOcc != -1){
                            printf("\n%s %d\n", maxText, leastOcc);
                            free(maxText);
                        }
                        else
                            printf("\nWord was not found\n");
                }
                break;

            } // end case 3
            case 4:{
                for(int i = 0; i < w; i++){
                    write(pipesW[i], line, strlen(line));
                }
                succCmd = 1;

                int tBytes = 0;
                int tWords = 0;
                int tLines = 0;

                int len = 0;
                char resBuff[1024] = "";
                for(int i = 0; i < w; i++){
                    len = read(pipesR[i], resBuff, 1024);
                    resBuff[len] = '\0';

                    char* resToken = strtok(resBuff, " ");
                    tBytes += atoi(resToken);

                    resToken = strtok(NULL, " ");
                    tWords += atoi(resToken);

                    resToken = strtok(NULL, " ");
                    tLines += atoi(resToken);
                }
                printf("\nTotal Bytes: %d\nTotal Words: %d\nTotal Lines: %d\n",tBytes, tWords, tLines);
                break;
            } // end case 4
            case 5:{
                exitSGN = 1; // exit signal
                for(int i = 0; i < w; i++){
                    write(pipesW[i], line, strlen(line));
                }

                break;
            }
        }// end switch:
        if(exitSGN == 1)
            break;
	}


	pid_t wpid;
    int status = 0;

    while ((wpid = wait(&status)) > 0); // wait for all workers to exit

    free(line);
    free(pipesR);
    free(pipesW);
    for(int i = 0; i < w; i++){
        DMN_Destroy(dirMaps[i]);
    }
    free(dirMaps);



    return 1;
}

/*  Used for search command */
void JE_getReady(int w, int* pipesR, int* pipesW, int deadline){
    int* pReady = malloc(sizeof(int) * w); // shows which processes finished search
    char buffer[8192] = "";

    /*  Keep track of what pipes are ready to be read   */
    fd_set wrSet;
    fd_set rdSet;

    FD_ZERO(&wrSet);
    FD_ZERO(&rdSet);

    int maxRD = -1;
    int maxWR = -1;

    for(int i = 0; i < w; i++){
        FD_SET(pipesR[i], &rdSet);
        FD_SET(pipesW[i], &wrSet);
        pReady[i] = 0;

        maxRD = (maxRD > pipesR[i]) ? maxRD : pipesR[i];
        maxWR = (maxWR > pipesW[i]) ? maxWR : pipesW[i];
    }

    /*  Get answers from workers */
    int doneW = 0; // number of workers that finished

    alarm(deadline); // jobExecutor will keep printing results for deadline seconds
    while(doneW != w){ // until all workers finised

        select(maxRD + 1, &rdSet, NULL, NULL, NULL);
        if(endOfTime == 1) // if deadline passed
            break;

        for(int k = 0; k < w; k++){

            if(pReady[k] == 1){ // if worker already finished, dont check
                FD_CLR(pipesR[k], &rdSet);
            }

            else if(FD_ISSET(pipesR[k], &rdSet)){ // worker wrote in pipe
                int len;

                if(endOfTime == 1){
                    doneW = w;
                    break;
                }

                len = read(pipesR[k], buffer, 8192);
                buffer[len] = '\0';
                write(pipesW[k], buffer, 3);

                if(!strcmp(buffer, "~~!//")){ // if read this string, current worker finished
                    if(pReady[k] != 1){
                        doneW++;
                        pReady[k] = 1;
                    }

                }
                else{
                    printf("%s\n",buffer); // print result
                }

                FD_SET(pipesR[k], &rdSet);
            }
            else
                FD_SET(pipesR[k], &rdSet);
        }
    }

    alarm(0); // reset alarm
    endOfTime = 0;
    free(pReady);
}



/*  JE waits from all workers to be ready to recieve a new command  */
/*  If any worker exited, a new one is recreated                    */
void JE_informCmd(int w, int* chPid, int* pipesR, int* pipesW, DMNode** dirMaps){
    char buff[30] = "##Get Ready to read command##";
    int len = -1;

    for(int i = 0; i < w; i++){
        int status;
        pid_t result = waitpid(chPid[i], &status, WNOHANG);

        if(result == 0){ // child is alive
            while(1){ // until worker is ready
                kill(chPid[i], SIGUSR1); // send signal to child (if it is still executing search)

                write(pipesW[i], buff, strlen(buff));
                alarm(3);
                char ack[50];
                len = read(pipesR[i], ack, 50);

                alarm(0);
                if(len == -1){
                    continue;
                }
                ack[len] = '\0';

                if(!strcmp(ack, "##READY##"))
                    break;

            }

        }
        else{ // RIP worker
            int pid = fork();

            if(pid == 0){
                char pNameR[15];
                char pNameW[15];
                sprintf(pNameR, "Pipe%d[R]",i);
                sprintf(pNameW, "Pipe%d[W]",i); // get pipe Names

                worker(i, pNameR, pNameW, dirMaps[i]);
                exit(0);

            }
            else{
                char logFile[20];
                sprintf(logFile, "log/Worker_%d",chPid[i]);

                write(pipesW[i], logFile, strlen(logFile)); // send former worker's id(for log file)
                char discard[10];

                read(pipesR[i], discard, 10);
                while(1){

                    write(pipesW[i], buff, strlen(buff));
                    alarm(3);
                    char ack[50];
                    len = read(pipesR[i], ack, 50);

                    alarm(0);
                    if(len == -1){
                        continue;
                    }
                    ack[len] = '\0';

                    if(!strcmp(ack, "##READY##"))
                        break;
                }
            }
        }
    }
}


