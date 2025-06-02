#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <signal.h>
#include <locale.h>
#include "process.h"
#include "commands.h"
#include "utils.h"

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 64
#define PROMPT "myShell> "

PROCESS_INFORMATION currentForegroundProcess = {0};
int isForegroundProcessRunning = 0;
int isMonitorMode = 0;
DWORD monitoredPid = 0;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
        case CTRL_C_EVENT:
            if (isForegroundProcessRunning) {
                TerminateProcess(currentForegroundProcess.hProcess, 1);
                updateProcessStatusById(currentForegroundProcess.dwProcessId);
                CloseHandle(currentForegroundProcess.hProcess);
                CloseHandle(currentForegroundProcess.hThread);
                isForegroundProcessRunning = 0;
                print_unicode_line("\nĐã dừng tiến trình foreground");
                return TRUE;
            }
            return FALSE;
        default:
            return FALSE;
    }
}

int main(int argc, char* argv[]) {
    char input[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS];
    int background;

    // setup UTF-8 encode
    setup_console_encoding();

    initializeProcessList();
    initializeShellEnvironment();
    
    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
        print_unicode_line("Không set handler Ctrl+C.");
        return 1;
    }

    if (argc > 1) {
        monitoredPid = atoi(argv[1]);
        if (monitoredPid > 0) {
            isMonitorMode = 1;
            char buffer[512];
            snprintf(buffer, sizeof(buffer) - 1, "=== myShell - Chế độ giám sát tiến trình %lu ===", monitoredPid);
            buffer[sizeof(buffer) - 1] = '\0';
            print_unicode_line(buffer);
        }
    } else {
        print_unicode_line("=== myShell - Shell đơn giản cho Windows ===");
        print_unicode_line("Nhập 'help' để xem các lệnh.");
    }

    while (1) {
        // checkBackgroundProcesses();
        
        if (isMonitorMode) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, monitoredPid);
            if (hProcess != NULL) {
                DWORD exitCode;
                if (GetExitCodeProcess(hProcess, &exitCode)) {
                    if (exitCode != STILL_ACTIVE) {
                        char buffer[512];
                        snprintf(buffer, sizeof(buffer) - 1, "\nTiến trình %lu đã kết thúc với mã: %lu", monitoredPid, exitCode);
                        buffer[sizeof(buffer) - 1] = '\0';
                        print_unicode_line(buffer);
                        CloseHandle(hProcess);
                        break;
                    }
                }
                CloseHandle(hProcess);
            } else {
                print_unicode_line("\nKhông thể mở tiến trình để giám sát. Tiến trình có thể đã kết thúc.");
                break;
            }
        }
        
        print_unicode(PROMPT);
        fflush(stdout);
        
        // if (fgets(input, MAX_COMMAND_LENGTH, stdin) == NULL) {
        if (readCommandLine(input, MAX_COMMAND_LENGTH) == 0) {
            break;
        }
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0) {
            continue;
        }

        background = parseCommand(input, args);
        if (args[0] == NULL) {
            continue;
        }

        if (executeBuiltinCommand(args)) {
            continue;
        }
        executeCommand(args, background);
    }

    cleanupProcessList();
    return 0;
} 