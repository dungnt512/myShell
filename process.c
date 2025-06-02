#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "process.h"
#include "utils.h"
#include "commands.h"

ProcessEntry processList[MAX_PROCESSES];
int processCount = 0;

void initializeProcessList() {
    processCount = 0;
    memset(processList, 0, sizeof(processList));
}

void cleanupProcessList() {
    for (int i = 0; i < processCount; i++) {
        if (processList[i].processInfo.hProcess != NULL) {
            CloseHandle(processList[i].processInfo.hProcess);
            CloseHandle(processList[i].processInfo.hThread);
        }
    }
}

int addProcess(PROCESS_INFORMATION processInfo, const char* commandName, int isBackground) {
    if (processCount >= MAX_PROCESSES) {
        return -1;
    }

    processList[processCount].processInfo = processInfo;
    strncpy(processList[processCount].commandName, commandName, 255);
    processList[processCount].commandName[255] = '\0';
    processList[processCount].isRunning = 1;
    processList[processCount].isPaused = 0;
    processList[processCount].isBackground = isBackground;

    return processCount++;
}

void removeProcess(int index) {
    if (index < 0 || index >= processCount) {
        return;
    }

    CloseHandle(processList[index].processInfo.hProcess);
    CloseHandle(processList[index].processInfo.hThread);

    for (int i = index; i + 1 < processCount; i++) {
        processList[i] = processList[i + 1];
    }

    processCount--;
}

void updateProcessStatus() {
    DWORD exitCode;
    for (int i = 0; i < processCount; i++) {
        if (processList[i].isRunning) {
            if (GetExitCodeProcess(processList[i].processInfo.hProcess, &exitCode)) {
                if (exitCode != STILL_ACTIVE) {
                    processList[i].isRunning = 0;
                }
            }
        }
    }
}

void updateProcessStatusById(DWORD processId) {
    // DWORD exitCode;
    for (int i = 0; i < processCount; i++) {
        if (processList[i].processInfo.dwProcessId == processId) {
            if (processList[i].isRunning) {
                processList[i].isRunning = 0;
            }
            break;
        }
    }
}

void listProcesses() {
    checkBackgroundProcesses();
    char buffer[512];
    // updateProcessStatus();

    print_unicode_line("\n--- Danh sách tiến trình ---");
    sprintf(buffer, "%-5s %-10s %-30s %-20s", "STT", "PID", "Tên lệnh", "Trạng thái");
    print_unicode_line(buffer);
    print_unicode_line("----------------------------------------------------------");

    for (int i = 0; i < processCount; i++) {
        const char* status;
        if (!processList[i].isRunning) {
            status = "Đã kết thúc";
        } else if (processList[i].isPaused) {
            status = "Tạm dừng";
        } else {
            status = "Đang chạy";
        }
        
        sprintf(buffer, "%-5d %-10lu %-30s %-20s", i, 
            processList[i].processInfo.dwProcessId,
            processList[i].commandName,
            status);
        print_unicode_line(buffer);
    }
    print_unicode_line("");
}

int parseCommand(char* input, char** args) {
    int i = 0;
    int background = 0;

    args[i] = strtok(input, " \t");
    while (args[i] != NULL && i + 1 < MAX_ARGS) {
        args[++i] = strtok(NULL, " \t");
    }

    if (i > 0 && args[i - 1] != NULL && strcmp(args[i - 1], "&") == 0) {
        args[i - 1] = NULL;
        background = 1;
    }

    return background;
}

char* findExecutableInShellPaths(const char* command) {
    char buffer[MAX_PATH * 2]; 
    
    if (strchr(command, '\\') || strchr(command, '/')) {
        if (isExecutable(command)) {
            return strdup(command);
        }
        return NULL;
    }
    
    extern char shellPaths[50][512];
    extern int pathCount;
    
    const char* extensions[] = {"", ".exe", ".bat", ".cmd", ".com", NULL};
    
    for (int i = 0; i < pathCount; i++) {
        for (int j = 0; extensions[j] != NULL; j++) {
            snprintf(buffer, sizeof(buffer) - 1, "%s\\%s%s", shellPaths[i], command, extensions[j]);
            buffer[sizeof(buffer) - 1] = '\0'; 
            
            if (isExecutable(buffer)) {
                return strdup(buffer);
            }
        }
    }
    
    return NULL;
}

void executeCommand(char** args, int background) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char command[MAX_PATH * 4] = {0}; 
    char buffer[1024];
    
    char* executablePath = findExecutableInShellPaths(args[0]);
    if (executablePath == NULL) {
        sprintf(buffer, "Lỗi: Không tìm thấy lệnh '%s' trong các đường dẫn của shell", args[0]);
        print_unicode_line(buffer);
        return;
    }
    
    strcpy(command, executablePath);
    free(executablePath);
    
    for (int i = 1; args[i] != NULL; i++) {
        strcat(command, " ");
        if (strchr(args[i], ' ') != NULL) {
            strcat(command, "\"");
            strcat(command, args[i]);
            strcat(command, "\"");
        } else {
            strcat(command, args[i]);
        }
    }

    // if .bat file, add cmd /c to the command
    if (strstr(args[0], ".bat") != NULL) {
        char batchCommand[MAX_PATH * 2 + 10] = "cmd /c ";
        strcat(batchCommand, command);
        strcpy(command, batchCommand);
    }

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    DWORD creationFlags = 0;
    if (background) {
        creationFlags = CREATE_NEW_CONSOLE;
    }

    if (!CreateProcess(NULL, command, NULL, NULL, FALSE, creationFlags, NULL, NULL, &si, &pi)) {
        snprintf(buffer, sizeof(buffer) - 1, "Lỗi: Không thể tạo tiến trình cho lệnh '%s'. Mã lỗi: %lu", command, GetLastError());
        buffer[sizeof(buffer) - 1] = '\0';
        print_unicode_line(buffer);
        return;
    }

    addProcess(pi, args[0], background);

    if (background) {
        sprintf(buffer, "[1] %lu", pi.dwProcessId);
        print_unicode_line(buffer);
    } else {
        currentForegroundProcess = pi;
        isForegroundProcessRunning = 1;
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        isForegroundProcessRunning = 0;
    }
}

int killProcess(DWORD processId) {
    char buffer[512];
    for (int i = 0; i < processCount; i++) {
        if (processList[i].processInfo.dwProcessId == processId) {
            if (TerminateProcess(processList[i].processInfo.hProcess, 1)) {
                snprintf(buffer, sizeof(buffer) - 1, "Đã kết thúc tiến trình PID %lu", processId);
                buffer[sizeof(buffer) - 1] = '\0';
                print_unicode_line(buffer);
                processList[i].isRunning = 0;
                return 1;
            } else {
                snprintf(buffer, sizeof(buffer) - 1, "Không thể kết thúc tiến trình PID %lu. Mã lỗi: %lu", processId, GetLastError());
                buffer[sizeof(buffer) - 1] = '\0';
                print_unicode_line(buffer);
                return 0;
            }
        }
    }
    snprintf(buffer, sizeof(buffer) - 1, "Không tìm thấy tiến trình PID %lu", processId);
    buffer[sizeof(buffer) - 1] = '\0';
    print_unicode_line(buffer);
    return 0;
}

int stopProcess(DWORD processId) {
    char buffer[512];
    for (int i = 0; i < processCount; i++) {
        if (processList[i].processInfo.dwProcessId == processId) {
            if (SuspendThread(processList[i].processInfo.hThread) != (DWORD)-1) {
                processList[i].isPaused = 1;
                snprintf(buffer, sizeof(buffer) - 1, "Đã tạm dừng tiến trình PID %lu", processId);
                buffer[sizeof(buffer) - 1] = '\0';
                print_unicode_line(buffer);
                return 1;
            } else {
                snprintf(buffer, sizeof(buffer) - 1, "Không thể tạm dừng tiến trình PID %lu. Mã lỗi: %lu", processId, GetLastError());
                buffer[sizeof(buffer) - 1] = '\0';
                print_unicode_line(buffer);
                return 0;
            }
        }
    }
    snprintf(buffer, sizeof(buffer) - 1, "Không tìm thấy tiến trình PID %lu", processId);
    buffer[sizeof(buffer) - 1] = '\0';
    print_unicode_line(buffer);
    return 0;
}

int resumeProcess(DWORD processId) {
    char buffer[512];
    for (int i = 0; i < processCount; i++) {
        if (processList[i].processInfo.dwProcessId == processId) {
            if (ResumeThread(processList[i].processInfo.hThread) != (DWORD)-1) {
                processList[i].isPaused = 0;
                snprintf(buffer, sizeof(buffer) - 1, "Đã tiếp tục tiến trình PID %lu", processId);
                buffer[sizeof(buffer) - 1] = '\0';
                print_unicode_line(buffer);
                return 1;
            } else {
                snprintf(buffer, sizeof(buffer) - 1, "Không thể tiếp tục tiến trình PID %lu. Mã lỗi: %lu", processId, GetLastError());
                buffer[sizeof(buffer) - 1] = '\0';
                print_unicode_line(buffer);
                return 0;
            }
        }
    }
    snprintf(buffer, sizeof(buffer) - 1, "Không tìm thấy tiến trình PID %lu", processId);
    buffer[sizeof(buffer) - 1] = '\0';
    print_unicode_line(buffer);
    return 0;
}

void checkBackgroundProcesses() {
    DWORD exitCode;
    char buffer[512];
    int hasCompletedProcess = 0;
    
    for (int i = 0; i < processCount; i++) {
        if (processList[i].isBackground && processList[i].isRunning) {
            if (GetExitCodeProcess(processList[i].processInfo.hProcess, &exitCode)) {
                if (exitCode != STILL_ACTIVE) {
                    processList[i].isRunning = 0;
                    hasCompletedProcess = 1;
                    // snprintf(buffer, sizeof(buffer) - 1, "\n[%d] Tiến trình '%s' (PID: %lu) đã kết thúc với mã: %lu", 
                    //     i + 1, processList[i].commandName, processList[i].processInfo.dwProcessId, exitCode);
                    // buffer[sizeof(buffer) - 1] = '\0';
                    // print_unicode_line(buffer);
                }
            }
        }
    }
    
    if (hasCompletedProcess) {
        print_unicode("\n");
    }
}

int isProcessExist(DWORD processId) {
    for (int i = 0; i < processCount; i++) {
        if (processList[i].processInfo.dwProcessId == processId) {
            return 1;
        }
    }
    return 0;
} 