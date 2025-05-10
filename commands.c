#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "commands.h"
#include "process.h"
#include "utils.h"

#define MAX_PATHS 100
#define MAX_PATH_LENGTH 512

char shellPaths[MAX_PATHS][MAX_PATH_LENGTH];
int pathCount = 0;

void initializeShellEnvironment() {
    pathCount = 0;
    strcpy(shellPaths[pathCount++], ".");

    // add default windows path (if neccessary)
    // char systemRoot[MAX_PATH];
    // if (GetEnvironmentVariable("SystemRoot", systemRoot, MAX_PATH)) {
    //     char systemPath[MAX_PATH * 2];
    //     _snprintf(systemPath, sizeof(systemPath) - 1, "%s\\System32", systemRoot);
    //     systemPath[sizeof(systemPath) - 1] = '\0';
    //     strncpy(shellPaths[pathCount], systemPath, MAX_PATH_LENGTH - 1);
    //     shellPaths[pathCount][MAX_PATH_LENGTH - 1] = '\0';
    //     pathCount++;
    // }
}

int executeBuiltinCommand(char** args) {
    if (args[0] == NULL) {
        return 0;
    }
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
        return 1;
    }
    if (strcmp(args[0], "help") == 0) {
        showHelp();
        return 1;
    }
    if (strcmp(args[0], "date") == 0) {
        showDate();
        return 1;
    }
    if (strcmp(args[0], "time") == 0) {
        showTime();
        return 1;
    }
    if (strcmp(args[0], "dir") == 0) {
        listDir(args);
        return 1;
    }
    if (strcmp(args[0], "path") == 0) {
        showPath();
        return 1;
    }
    if (strcmp(args[0], "addpath") == 0) {
        if (args[1] != NULL) {
            addPath(args[1]);
        } else {
            print_unicode_line("Lỗi: thiếu tham số đường dẫn. Sử dụng: addpath <đường_dẫn>");
        }
        return 1;
    }
    if (strcmp(args[0], "list") == 0) {
        listProcesses();
        return 1;
    }
    if (strcmp(args[0], "kill") == 0) {
        if (args[1] != NULL) {
            killProcess(atoi(args[1]));
        } else {
            print_unicode_line("Lỗi: thiếu ID tiến trình. Sử dụng: kill <PID>");
        }
        return 1;
    }

    if (strcmp(args[0], "stop") == 0) {
        if (args[1] != NULL) {
            stopProcess(atoi(args[1]));
        } else {
            print_unicode_line("Lỗi: thiếu ID tiến trình. Sử dụng: stop <PID>");
        }
        return 1;
    }

    if (strcmp(args[0], "resume") == 0) {
        if (args[1] != NULL) {
            resumeProcess(atoi(args[1]));
        } else {
            print_unicode_line("Lỗi: thiếu ID tiến trình. Sử dụng: resume <PID>");
        }
        return 1;
    }

    return 0;
}

void showHelp() {
    print_unicode_line("\n=== Trợ giúp tinyShell ===");
    print_unicode_line("Các lệnh tích hợp:");
    print_unicode_line("  help                - Hiển thị trợ giúp này");
    print_unicode_line("  exit                - Thoát shell");
    print_unicode_line("  date                - Hiển thị ngày hiện tại");
    print_unicode_line("  time                - Hiển thị giờ hiện tại");
    print_unicode_line("  dir [đường_dẫn]     - Liệt kê nội dung thư mục");
    print_unicode_line("  path                - Hiển thị đường dẫn của shell");
    print_unicode_line("  addpath <đường_dẫn> - Thêm đường dẫn vào quản lý của shell");
    print_unicode_line("\nQuản lý tiến trình:");
    print_unicode_line("  list                - Liệt kê tất cả tiến trình do shell quản lý");
    print_unicode_line("  kill <PID>          - Kết thúc tiến trình");
    print_unicode_line("  stop <PID>          - Tạm dừng tiến trình");
    print_unicode_line("  resume <PID>        - Tiếp tục tiến trình đã tạm dừng");
    print_unicode_line("\nChạy lệnh:");
    print_unicode_line("  <lệnh> [tham_số]    - Chạy lệnh ở chế độ foreground");
    print_unicode_line("  <lệnh> [tham_số] &  - Chạy lệnh ở chế độ background");
    print_unicode_line("\nPhím tắt:");
    print_unicode_line("  Ctrl+C              - Hủy tiến trình foreground đang chạy");
    print_unicode_line("");
}

void showDate() {
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    char date_string[100];
    char buffer[150];
    
    strftime(date_string, sizeof(date_string), "%d/%m/%Y", tm_now);
    snprintf(buffer, sizeof(buffer) - 1, "Ngày hiện tại: %s", date_string);
    buffer[sizeof(buffer) - 1] = '\0';
    print_unicode_line(buffer);
}

void showTime() {
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    char time_string[100];
    char buffer[150];
    
    strftime(time_string, sizeof(time_string), "%H:%M:%S", tm_now);
    snprintf(buffer, sizeof(buffer) - 1, "Giờ hiện tại: %s", time_string);
    buffer[sizeof(buffer) - 1] = '\0';
    print_unicode_line(buffer);
}

void listDir(char** args) {
    HANDLE hFind;
    WIN32_FIND_DATA findData;
    char searchPath[MAX_PATH];
    char buffer[1024];

    if (args[1] == NULL) {
        strcpy(searchPath, ".\\*");
    } else {
        snprintf(searchPath, sizeof(searchPath) - 1, "%s\\*", args[1]);
        searchPath[sizeof(searchPath) - 1] = '\0';
    }

    // start search
    hFind = FindFirstFile(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        snprintf(buffer, sizeof(buffer) - 1, "Lỗi: không thể mở thư mục. Mã lỗi: %lu", GetLastError());
        buffer[sizeof(buffer) - 1] = '\0';
        print_unicode_line(buffer);
        return;
    }

    print_unicode("\nDanh sách file trong thư mục: ");
    if (args[1] == NULL) {
        print_unicode_line("(hiện tại)");
    } else {
        print_unicode_line(args[1]);
    }
    
    print_unicode_line("Tên                            Kích thước    Loại");
    print_unicode_line("-------------------------------------------------------------");

    do {
        char fileType[20] = "";
        ULONGLONG fileSize = (((ULONGLONG)findData.nFileSizeHigh) << sizeof(findData.nFileSizeLow) << 3) + findData.nFileSizeLow;
        
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            strcpy(fileType, "Thư mục");
        } else {
            strcpy(fileType, "Tập tin");
        }
        
        snprintf(buffer, sizeof(buffer) - 1, "%-30s %-14I64u %s", 
                findData.cFileName, 
                fileSize,
                fileType);
        buffer[sizeof(buffer) - 1] = '\0';
        print_unicode_line(buffer);
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
    print_unicode_line("");
}

void showPath() {
    char buffer[1024];
    
    print_unicode_line("\nCác đường dẫn trong PATH của shell:");
    
    if (pathCount == 0) {
        print_unicode_line("Không có đường dẫn nào được cấu hình.");
    } else {
        for (int i = 0; i < pathCount; i++) {
            snprintf(buffer, sizeof(buffer) - 1, "%d. %s", i + 1, shellPaths[i]);
            buffer[sizeof(buffer) - 1] = '\0';
            print_unicode_line(buffer);
        }
    }
    print_unicode_line("");
}

void addPath(char* newPath) {
    char buffer[1024];
    if (pathCount >= MAX_PATHS) {
        print_unicode_line("Lỗi: Đã đạt số lượng đường dẫn tối đa.");
        return;
    }
    
    DWORD attrib = GetFileAttributes(newPath);
    // READONLY     1
    // HIDDEN       2
    // SYSTEM       4
    // DIRECTORY    16
    // ARCHIVE      32
    // NORMAL       128
    // TEMPORARY    256
    // COMPRESSED   2048
    // OFFLINE      4096

    // INVAILID_FILE_ATTRIBUTES 0xFFFFFFFF
    if (attrib == INVALID_FILE_ATTRIBUTES || !(attrib & FILE_ATTRIBUTE_DIRECTORY)) {
        snprintf(buffer, sizeof(buffer) - 1, "Lỗi: Đường dẫn không hợp lệ hoặc không tồn tại: %s", newPath);
        buffer[sizeof(buffer) - 1] = '\0';
        print_unicode_line(buffer);
        return;
    }
    
    for (int i = 0; i < pathCount; i++) {
        if (stricmp(shellPaths[i], newPath) == 0) {
            print_unicode_line("Đường dẫn đã tồn tại trong PATH của shell.");
            return;
        }
    }
    
    strncpy(shellPaths[pathCount], newPath, MAX_PATH_LENGTH - 1);
    shellPaths[pathCount][MAX_PATH_LENGTH - 1] = '\0';
    pathCount++;
    
    snprintf(buffer, sizeof(buffer) - 1, "Đã thêm đường dẫn vào PATH của shell: %s", newPath);
    buffer[sizeof(buffer) - 1] = '\0';
    print_unicode_line(buffer);
    
    // update PATH of system (optional :))
    // SetEnvironmentVariable("PATH", newFullPath);
} 