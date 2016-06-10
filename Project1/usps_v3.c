/*
 * NAME: Kyle Holmberg
 * DUCKID: kmh
 * SIN: 951312729
 * CLASS: CIS415
 *
 * Project 1 - Part 3
 * 
 * This project consists entirely of my original work, with the exception
 * of pseudocode provided by University of Oregon's CIS415's Piazza Forum
 */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

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
int SIGUSR1_received = 0;

/* Creates a new process */
static void addProcess(pid_t p, char *command) {
    if (procSpace < 0) {
        procSpace = DEFAULT_PROCS;
        procs = (Proc*) malloc(sizeof(Proc) * procSpace);
        if (procs == NULL) {
            p1perror(STDERR_FILENO, "ERR: MEMORY ALLOCATION FAILED FOR PROCESS CREATION");
            exit(4);
        }
    } else if (procNum >= procSpace) {
        procSpace += DEFAULT_PROCS;
        procs = (Proc*) realloc(procs, sizeof(Proc) * procSpace);
        if (procs == NULL) {
            p1perror(STDERR_FILENO, "ERR: MEMORY REALLOCATION FAILED FOR PROCESS CREATION");
            exit(5);
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
    for (int i = 0; i < procNum; i++) {
        if (procs[i].pid == pid) {
            procs[i].procEnded = 1; //true
            procs[i].procRunning = 0; //false
            break;
        }
    }
}

/* Free memory blocks for all processes (used to clean) */
static void freeProcs() {
    for (int i = 0; i < procNum; i++) {
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

/* Method to check if signal reception occured */
void signalHandler(int sigNum) {
    if (sigNum == SIGUSR1) {
        SIGUSR1_received++;
    }
}

/* Method to set and reset alarm for scheduling */
void alarmHandler(int sigNum) {
    int i;
    p1putstr(STDOUT_FILENO, "ALARM ACTIVATED\n");
    
    // Suspend a child process
    int runner = 0;
    for (i=0; i < procNum; i++) {
        if (procs[i].procRunning == 1) {
            kill(procs[i].pid, SIGSTOP);
            procs[i].procRunning = 0;
            runner = i;
            break;
        }
    }
    
    // Choose the next child process to run
    runner++;
    if (runner >= procNum) {
        runner = 0;
    }

    int newRunner = -1;
    for (i=0; i < procNum; i++) {
        if (procs[runner].procEnded == 0) {
            newRunner = runner;
            break;
        }
        runner++;
        if (runner >= procNum) {
            runner = 0;
        }
    }
    
    // Resume previous child process
    if (newRunner != -1) {
        if (procs[newRunner].procStarted == 1) {
            kill(procs[newRunner].pid, SIGUSR1);
            procs[newRunner].procStarted = 0;
        } else {
            kill(procs[newRunner].pid, SIGCONT);
        }
        procs[newRunner].procRunning = 1; //true
    }

    alarm(1);
}

int main() {
    int fdin = STDIN_FILENO;

    // Check to ensure signal received
    if (signal(SIGUSR1, signalHandler) == SIG_ERR) {
        p1perror(STDERR_FILENO, "ERR: SIGNAL RECEIVE ISSUE\n");
        //p1putstr(STDERR_FILENO, "ERR: SIGNAL SEND/RECEIVE ISSUE\n");
        exit(3);
    }

    // Parse command lines, prepare them to be checked for arguments
    while (1) {
        char line[200];
        int lineNum = p1getline(fdin, line, 200);
        if (lineNum == 0) { // No process in command line
            break;
        }
        line[p1strlen(line)-1] = '\0'; //Remove new line symbol from cmdline
        addProcess(0, line);
    }

    // Parse arguments for each process (if any)
    for (int i=0; i < procNum; i++) {
        char *argv[50];
        int argc = 0;
        int argument = 0;

        while (1) {
            char word[50];
            argument = p1getword(procs[i].command, argument, word);
            if (argument == -1) { // No arguments exist
                break;
            }

            argv[argc] = p1strdup(word);
            argc++;
        }
        argv[argc] = NULL;

        procs[i].pid = fork(); // Start a new child process
        
        if (procs[i].pid < 0) { // If the process doesn't exist...
            p1perror(STDERR_FILENO, "FORK FAILURE");
            exit(1);
        } else if (procs[i].pid == 0) { // If the process ID# is a child process...
            while (SIGUSR1_received != 1) {
                sleep(1);
            }
            execvp(argv[0], argv);
            p1perror(STDERR_FILENO, "ERR: EXECVP FAILURE");
            exit(2);
        }

        printInfo(procs[i].pid, argv[0]);
        freeArgs(argv);
    }
    
    struct sigaction action;
    action.sa_handler = alarmHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGALRM, &action, NULL);
    alarm(1);

    // Child processes have ended
    int deadCount = 0;
    while (deadCount < procNum) {
        int status;
        int childPID = wait(&status);

        if (WIFEXITED(status) && childPID > 0) { // Child terminated normally
            deadCount++;
            endProcess(childPID);
            printInfo(childPID, "being exited");
            printInfo(WEXITSTATUS(status), "Exit Status (0 if exited)");
        } else if (WIFSIGNALED(status) && childPID > 0) { // Child terminated by a signal
            deadCount++;
            endProcess(childPID);
            printInfo(childPID, "Signaled");
            printInfo(WTERMSIG(status), "Termination from Signal (0 if terminated)");
        }
    }

    freeProcs();
    return 0;
}