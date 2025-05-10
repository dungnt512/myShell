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
#define PROMPT "tinyShell> "

PROCESS_INFORMATION currentForegroundProcess = {0};
int isForegroundProcessRunning = 0;

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

int main() {
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

    print_unicode_line("=== tinyShell - Shell đơn giản cho Windows ===");
    print_unicode_line("Nhập 'help' để xem các lệnh.");

    while (1) {
        print_unicode(PROMPT);
        fflush(stdout);
        
        if (fgets(input, MAX_COMMAND_LENGTH, stdin) == NULL) {
        // if (readCommandLine(input, MAX_COMMAND_LENGTH) == 0) {
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