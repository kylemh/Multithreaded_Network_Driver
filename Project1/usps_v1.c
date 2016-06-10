/*
 * NAME: Kyle Holmberg
 * DUCKID: kmh
 * SIN: 951312729
 * CLASS: CIS415
 *
 * Project 1 - Part 1
 * 
 * This project consists entirely of my original work, with the exception
 * of pseudocode provided by University of Oregon's CIS415's Piazza Forum
 */

#include <stdlib.h>
#include <unistd.h>

#include "p1fxns.h"

#define DEFAULT_PROCS 25

typedef struct process {
    pid_t pid;
    char *command;
    int procStarted; // The three remaining variables are booleans (0 for false, 1 for true)
    int procRunning;
    int procEnded;
} Proc;

Proc *procs = NULL;
int procSpace = -1;
int procNum = 0;

/* Creates a new process */
static void addProcess(pid_t p, char *command) {
    if (procSpace < 0) {
        procSpace = DEFAULT_PROCS;
        procs = (Proc*) malloc(sizeof(Proc) * procSpace);
        if (procs == NULL) {
            p1perror(STDERR_FILENO, "ERR: MEMORY ALLOCATION FAILED FOR PROCESS CREATION");
            exit(3);
        }
    } else if (procNum >= procSpace) {
        procSpace += DEFAULT_PROCS;
        procs = (Proc*) realloc(procs, sizeof(Proc) * procSpace);
        if (procs == NULL) {
            p1perror(STDERR_FILENO, "ERR: MEMORY REALLOCATION FAILED FOR PROCESS CREATION");
            exit(4);
        }
    }

    procs[procNum].pid = p;
    procs[procNum].command = p1strdup(command);
    procs[procNum].procStarted = 1; //true
    procs[procNum].procRunning = 0; //false
    procs[procNum].procEnded = 0; //false
    procNum++;
}

/* Change Proc struct status boolean to reflect process termination */
static void endProcess(int pid) {
    for (int i=0; i < procNum; i++) {
        if (procs[i].pid == pid) {
            procs[i].procEnded = 1;
            break;
        }
    }
}

/* Free memory blocks for all processes (used to clean) */
static void freeProcs() {
    for (int i=0; i < procNum; i++) {
        free(procs[i].command);
    }
    procSpace = -1;
    procNum = 0;
    free(procs);
}

/* Free memory surrounding the arguments of a process (used to clean) */
static void freeArgs(char *argv[]) {
    for (int i=0; 1; i++) {
        if (argv[i] == NULL) {
            break;
        }
        free(argv[i]);
    }
}

/* Print statement that can be used to reveal process statuses and show other relevant information */
static void printInfo(int n, char *message) {
    if (n > 1) { // Detailing a process
        p1putstr(STDOUT_FILENO, "Process ");
        p1putstr(STDOUT_FILENO, message);
        p1putstr(STDOUT_FILENO, " has PID number: ");
        p1putint(STDOUT_FILENO, n);
        p1putstr(STDOUT_FILENO, "\n");
    } else { // Detailing process statuses
        p1putstr(STDOUT_FILENO, message);
        p1putstr(STDOUT_FILENO, ": ");
        p1putint(STDOUT_FILENO, n);
        p1putstr(STDOUT_FILENO, "\n");
    }
}

int main() {
    int fdin = STDIN_FILENO;

    // Parse command lines, prepare them to be checked for arguments
    while (1) {
        char line[256];
        int lineNum = p1getline(fdin, line, 200);
        if (lineNum == 0) { // No process in command line
            break;
        }
        line[p1strlen(line)-1] = '\0';
        addProcess(0, line);
    }

    // Parse arguments for each process (if any)
    for (int i=0; i < procNum; i++) {
        char *argv[64];
        int argc = 0;
        int argument = 0;

        while (1) {
            char word[64];
            argument = p1getword(procs[i].command, argument, word);
            if (argument == -1) { // No arguments exist
                break;
            }

            argv[argc] = p1strdup(word);
            argc++;
        }

        argv[argc] = NULL;

        // Start a new child process
        procs[i].pid = fork();
        if (procs[i].pid < 0) { // If the process doesn't exist...
            p1perror(STDERR_FILENO, "FORK FAILURE");
            exit(1);
        } else if (procs[i].pid == 0) { // If the process ID# is a child process...
            execvp(argv[0], argv);
            p1perror(STDERR_FILENO, "ERR: EXECVP FAILURE");
            exit(2);
        }

        printInfo(procs[i].pid, argv[0]);
        freeArgs(argv);
    }

    // Child processes have ended
    int deadCount = 0;
    while (deadCount < procNum) {
        int status;
        int childPID = wait(&status);

        if (childPID > 0 && WIFEXITED(status)) { // Child terminated normally
            deadCount++;
            endProcess(childPID);
            printInfo(childPID, "being exited");
            printInfo(WEXITSTATUS(status), "Exit Status (0 if exited)");
        } else if (childPID > 0 && WIFSIGNALED(status)) { // Child terminated by a signal
            deadCount++;
            endProcess(childPID);
            printInfo(childPID, "Signaled");
            printInfo(WTERMSIG(status), "Termination from Signal (0 if terminated)");
        } else { // Child not terminated
            p1perror(STDERR_FILENO, "ERR: CHILD NOT TERMINATED");
        }
    }

    freeProcs();
    return 0;
}