#ifndef UTILS_H
#define UTILS_H

#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200
#endif

void setup_console_encoding();

void print_unicode(const char* text);
void print_unicode_line(const char* text);

int isExecutable(const char* path);

char* findExecutable(const char* command);

int readCommandLine(char *buffer, int bufferSize);

#ifndef HISTORY_SIZE
#define HISTORY_SIZE 100
#endif
#ifndef MAX_COMMAND_LENGTH
#define MAX_COMMAND_LENGTH 1024
#endif

extern char commandHistory[HISTORY_SIZE][MAX_COMMAND_LENGTH];
extern int historyCount;
extern int currentHistoryIndex;

#endif 