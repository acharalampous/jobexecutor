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
#include "aclib.h"

volatile sig_atomic_t activeWorkers = 0;
volatile sig_atomic_t endOfTime = 0;

void sigHandlerJE(int signo){
    if(signo == SIGCHLD) // child exited
        activeWorkers--;
    else if(signo == SIGALRM)
        endOfTime = 1;
}

int main(int argc, char* argv[]){
    char* docFile; // File that the paths of text files are included
    int w; // number of Workers

    if(argc == 3){ // ./jobExecutor -d docfile
        if(!strcmp(argv[1], "-d")){
            docFile = argv[2];
            w = 10;
        }
        else{ // invalid parameter given
            exeParameters();
            printf("Abort.\n");
            return -1;
        }
    }
    else if(argc >= 5){ // ./jobExecutor -d docFile -w W
        if((!strcmp(argv[1], "-d") && (!strcmp(argv[3], "-w")))){
            if(!isNumber(argv[4])){
                exeParameters();
                printf("*error* Parameter *argv[4]* after -w was not a number.Abort\n");
                return -1;
            }
            docFile = argv[2];
            w = atoi(argv[4]);
        }
        else if((!strcmp(argv[1], "-w") && (!strcmp(argv[3], "-d")))){
            if(!isNumber(argv[2])){
                exeParameters();
                printf("*error* Parameter *argv[2]* after -w was not a number. Abort\n");
                return -1;
            }
            docFile = argv[4];
            w = atoi(argv[2]);
        }
        else{ // invalid parameter given
            exeParameters();
            printf("Abort.\n");
            return -1;
        }
    }
    else{
        exeParameters();
        printf("Abort.\n");
        return -1;
    }

    /*  Must create directory for the log files */
    struct stat st = {0};
    if (stat("log", &st) == -1) {
        mkdir("log", 0700);
    }

    /* Set sigHandler for Job Executor  */
    static struct sigaction sigsJE;
    memset(&sigsJE, 0, sizeof(sigsJE));

    sigsJE.sa_handler = sigHandlerJE;

    sigemptyset(&(sigsJE.sa_mask));
    sigaddset(&(sigsJE.sa_mask), SIGUSR1);
    sigaddset(&(sigsJE.sa_mask), SIGUSR2);
    sigaddset(&(sigsJE.sa_mask), SIGCHLD);
    sigaction(SIGUSR1, &sigsJE, NULL);
    sigaction(SIGUSR2, &sigsJE, NULL);
    sigaction(SIGCHLD, &sigsJE, NULL);
    sigaction(SIGALRM, &sigsJE, NULL);

    system("rm -f Pipe*"); // remove pipes from before

    int i, pid;
    char pI[5];
    char pNameW[10]; // used to create the name of pipes for W
    char pNameR[10]; // used for pipes for R

    int* chPid; // array that will hold all children pids

    int numOfPaths = getNumOfPaths(docFile);
    if(numOfPaths == -1){
        printf("ERROR while opening file\n");
        return -1;
    }
    else if(numOfPaths == 0){
        printf("No paths in given file\n");
        return -1;
    }

    if(numOfPaths < w) // create only as much workers as paths
        w = numOfPaths;

    activeWorkers = w;
    chPid = malloc(sizeof(int) * w); // keep all children ids <-> map pid to i

    for(i = 0; i < w; i++){

        sprintf(pI, "%d", i);
        strcpy(pNameW, "Pipe");
        strcpy(pNameR, "Pipe");

        strcat(pNameR, pI);
        strcat(pNameW, pI);

        strcat(pNameR, "[R]");
        strcat(pNameW, "[W]"); // create name for pipes W and R

        mkfifo(pNameR, 0666);
        mkfifo(pNameW, 0666); // create pipes using names

        pid = fork(); // create worker
        if(pid == 0)
            break;
        else
            chPid[i] = pid;
    }

    if(pid == 0){
        free(chPid);
        int totalS = worker(i, pNameW, pNameR, NULL); // execute worker process
        exit(totalS);
    }
    else{
        jobExecutor(w, chPid, docFile); // execute job executor
        free(chPid);
    }
}
