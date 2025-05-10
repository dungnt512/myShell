#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>
#include "utils.h"

// setup UTF-8 encode
void setup_console_encoding() {
    // setup UTF-8 code page
    SetConsoleOutputCP(65001); 
    SetConsoleCP(65001);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }

    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hIn != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hIn, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
            SetConsoleMode(hIn, dwMode);
        }
    }

    setlocale(LC_ALL, ".UTF8");
}

void print_unicode(const char* text) {
    // convert from UTF-8 into UTF-16 (necessary for WriteConsoleW)
    int length = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
    if (length <= 0) return;
    
    wchar_t* wtext = (wchar_t*)malloc(length * sizeof(wchar_t));
    if (!wtext) return;
    
    MultiByteToWideChar(CP_UTF8, 0, text, -1, wtext, length);
    
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD charsWritten;
    WriteConsoleW(hConsole, wtext, wcslen(wtext), &charsWritten, NULL);
    
    free(wtext);
}

void print_unicode_line(const char* text) {
    print_unicode(text);
    print_unicode("\n");
}

int isExecutable(const char* path) {
    DWORD fileAttributes = GetFileAttributes(path);
    if (fileAttributes == INVALID_FILE_ATTRIBUTES || 
        (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return 0;
    }

    const char* extension = strrchr(path, '.');
    if (extension != NULL) {
        if (stricmp(extension, ".exe") == 0 ||
            stricmp(extension, ".bat") == 0 ||
            stricmp(extension, ".cmd") == 0 ||
            stricmp(extension, ".com") == 0) {
            return 1;
        }
    }
    
    return 0;
}

char* findExecutable(const char* command) {
    if (strchr(command, '\\') != NULL || strchr(command, '/') != NULL) {
        if (isExecutable(command)) {
            char* result = _strdup(command);
            return result;
        }
        return NULL;
    }
    
    char currentDirPath[MAX_PATH + 1] = {0};
    sprintf(currentDirPath, ".\\%s", command);
    if (isExecutable(currentDirPath)) {
        return strdup(currentDirPath);
    }

    const char* extensions[] = {"", ".exe", ".bat", ".cmd", ".com", NULL};
    
    if (strchr(command, '.') == NULL) {
        for (int i = 0; extensions[i] != NULL; i++) {
            char commandWithExt[MAX_PATH + 1] = {0};
            sprintf(commandWithExt, ".\\%s%s", command, extensions[i]);
            if (isExecutable(commandWithExt)) {
                return strdup(commandWithExt);
            }
        }
    }
    
    char path[32767];
    if (GetEnvironmentVariable("PATH", path, sizeof(path)) > 0) {
        char* context = NULL;
        char* token = strtok_s(path, ";", &context);
        while (token != NULL) {
            for (int i = 0; extensions[i] != NULL; i++) {
                char fullPath[MAX_PATH + 1] = {0};
                sprintf(fullPath, "%s\\%s%s", token, command, extensions[i]);
                if (isExecutable(fullPath)) {
                    char* result = _strdup(fullPath);
                    return result;
                }
            }
            token = strtok_s(NULL, ";", &context);
        }
    }
    
    return NULL;
} 