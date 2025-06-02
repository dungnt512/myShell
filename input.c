#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include "utils.h"

#define KEY_UP      72
#define KEY_DOWN    80
#define KEY_LEFT    75
#define KEY_RIGHT   77
#define KEY_BACKSPACE 8
#define KEY_DELETE  83
#define KEY_ENTER   13
#define KEY_ESC     27

char commandHistory[HISTORY_SIZE][MAX_COMMAND_LENGTH];
int historyCount = 0;
int currentHistoryIndex = -1;

void getCommandFromHistory(char *buffer, int *cursorPos, int direction) {
    if (historyCount == 0) {
        return;
    }

    // arrow down 
    if (direction > 0) {
        if (currentHistoryIndex > 0) {
            currentHistoryIndex--;
        } else if (currentHistoryIndex == 0) {
            currentHistoryIndex = -1;
            buffer[0] = '\0';
            *cursorPos = 0;
            return;
        }
    } else {
        if (currentHistoryIndex < historyCount - 1) {
            currentHistoryIndex++;
        } else {
            return;
        }
    }

    if (currentHistoryIndex == -1) {
        buffer[0] = '\0';
    } else {
        strcpy(buffer, commandHistory[currentHistoryIndex]);
    }
    *cursorPos = strlen(buffer);
}

void addToHistory(const char *command) {
    if (strlen(command) == 0) {
        return;
    }
    if (historyCount > 0 && strcmp(commandHistory[0], command) == 0) {
        return;
    }
    for (int i = historyCount; i > 0; i--) {
        if (i < HISTORY_SIZE) {
            strcpy(commandHistory[i], commandHistory[i - 1]);
        }
    }
    if (historyCount < HISTORY_SIZE) {
        historyCount++;
    }
    strcpy(commandHistory[0], command);
}

void clearLine(int startX) {
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hStdout, &csbi);
    
    COORD pos = {startX, csbi.dwCursorPosition.Y};
    SetConsoleCursorPosition(hStdout, pos);
    
    DWORD written;
    FillConsoleOutputCharacter(hStdout, ' ', csbi.dwSize.X - startX, pos, &written);
    SetConsoleCursorPosition(hStdout, pos);
}

int readCommandLine(char *buffer, int bufferSize) {
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hStdout, &csbi);
    int startX = csbi.dwCursorPosition.X;
    
    int cursorPos = 0;
    int bufferLen = 0;
    buffer[0] = '\0';
    currentHistoryIndex = -1;
    
    while (1) {
        int ch = _getch();
        
        if (ch == 3) {
            printf("^C\n");
            exit(0);
        }
        
        if (ch == 0 || ch == 224) { 
            ch = _getch();
            
            switch (ch) {
                case KEY_UP:
                    getCommandFromHistory(buffer, &cursorPos, -1);
                    bufferLen = strlen(buffer);
                    clearLine(startX);
                    printf("%s", buffer);
                    break;
                    
                case KEY_DOWN: 
                    getCommandFromHistory(buffer, &cursorPos, 1);
                    bufferLen = strlen(buffer);
                    clearLine(startX);
                    printf("%s", buffer);
                    break;
                    
                case KEY_LEFT:
                    if (cursorPos > 0) {
                        cursorPos--;
                        printf("\b");
                    }
                    break;
                    
                case KEY_RIGHT:
                    if (cursorPos < bufferLen) {
                        printf("%c", buffer[cursorPos]);
                        cursorPos++;
                    }
                    break;
                    
                case KEY_DELETE:
                    if (cursorPos < bufferLen) {
                        for (int i = cursorPos; i < bufferLen - 1; i++) {
                            buffer[i] = buffer[i + 1];
                        }
                        bufferLen--;
                        buffer[bufferLen] = '\0';
                        
                        clearLine(startX);
                        printf("%s", buffer);
                        
                        COORD pos = {startX + cursorPos, csbi.dwCursorPosition.Y};
                        SetConsoleCursorPosition(hStdout, pos);
                    }
                    break;
            }
        } else {
            switch (ch) {
                case KEY_ENTER:
                    printf("\r\n");
                    buffer[bufferLen] = '\0';
                    if (bufferLen > 0) {
                        addToHistory(buffer);
                    }
                    return bufferLen;
                    
                case KEY_BACKSPACE:
                    if (cursorPos > 0) {
                        for (int i = cursorPos - 1; i < bufferLen - 1; i++) {
                            buffer[i] = buffer[i + 1];
                        }
                        bufferLen--;
                        cursorPos--;
                        buffer[bufferLen] = '\0';
                        
                        printf("\b");
                        clearLine(startX + cursorPos);
                        printf("%s", buffer + cursorPos);
                        
                        COORD pos = {startX + cursorPos, csbi.dwCursorPosition.Y};
                        SetConsoleCursorPosition(hStdout, pos);
                    }
                    break;
                    
                case KEY_ESC:
                    clearLine(startX);
                    buffer[0] = '\0';
                    bufferLen = 0;
                    cursorPos = 0;
                    break;
                    
                default:
                    if (ch >= 32 && ch <= 126 && bufferLen < bufferSize - 1) {
                        for (int i = bufferLen; i > cursorPos; i--) {
                            buffer[i] = buffer[i-1];
                        }
                        buffer[cursorPos] = ch;
                        cursorPos++;
                        bufferLen++;
                        buffer[bufferLen] = '\0';
                        
                        clearLine(startX);
                        printf("%s", buffer);
                        
                        COORD pos = {startX + cursorPos, csbi.dwCursorPosition.Y};
                        SetConsoleCursorPosition(hStdout, pos);
                    }
                    break;
            }
        }
    }
    
    return 0;
}