#ifndef COMMANDS_H
#define COMMANDS_H

int executeBuiltinCommand(char** args);
void initializeShellEnvironment();
void showHelp();
void showDate();
void showTime();
void listDir(char** args);
void showPath();
void addPath(char* newPath);
void unPath(int index);
void openNotepad(char** args);
void openCalculator();

#endif 