#ifndef PROCESS_H
#define PROCESS_H

#include <windows.h>

#define MAX_PROCESSES 100
#define MAX_ARGS 64

typedef struct {
    PROCESS_INFORMATION processInfo;
    char commandName[256];
    int isRunning;
    int isPaused;
    int isBackground;
} ProcessEntry;

extern PROCESS_INFORMATION currentForegroundProcess;
extern int isForegroundProcessRunning;

extern ProcessEntry processList[MAX_PROCESSES];
extern int processCount;

void initializeProcessList();
void cleanupProcessList();
int addProcess(PROCESS_INFORMATION processInfo, const char* commandName, int isBackground);
void removeProcess(int index);
void updateProcessStatus();
void updateProcessStatusById(DWORD processId);
void listProcesses();
void checkBackgroundProcesses();

int parseCommand(char* input, char** args);
void executeCommand(char** args, int background);

int killProcess(DWORD processId);
int stopProcess(DWORD processId);
int resumeProcess(DWORD processId);
int isProcessExist(DWORD processId);

#endif 