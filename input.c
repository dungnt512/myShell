#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "utils.h"

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

int readCommandLine(char *buffer, int bufferSize) {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD originalMode;
    
    // Lưu chế độ console ban đầu
    GetConsoleMode(hStdin, &originalMode);
    
    // Thiết lập chế độ phù hợp để đọc phím
    // Tắt ENABLE_LINE_INPUT để đọc từng ký tự một
    // Tắt ENABLE_ECHO_INPUT để tự quản lý hiển thị
    SetConsoleMode(hStdin, ENABLE_WINDOW_INPUT | ENABLE_VIRTUAL_TERMINAL_INPUT | ENABLE_PROCESSED_INPUT);

    int cursorPos = 0;
    int bufferLen = 0;
    buffer[0] = '\0';
    currentHistoryIndex = -1;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hStdout, &csbi);
    COORD startPos = csbi.dwCursorPosition;

    while (1) {
        INPUT_RECORD ir;
        DWORD numRead;
        
        // Đọc sự kiện từ bàn phím
        ReadConsoleInput(hStdin, &ir, 1, &numRead);
        
        if (numRead == 0 || ir.EventType != KEY_EVENT || !ir.Event.KeyEvent.bKeyDown) {
            continue;
        }

        KEY_EVENT_RECORD ker = ir.Event.KeyEvent;
        CHAR ch = ker.uChar.AsciiChar;
        WORD vk = ker.wVirtualKeyCode;

        // Xử lý phím Enter
        if (vk == VK_RETURN) {
            WriteConsole(hStdout, "\r\n", 2, NULL, NULL);
            buffer[bufferLen] = '\0';
            if (bufferLen > 0) {
                addToHistory(buffer);
            }
            break;
        }
        
        // Xử lý phím mũi tên
        else if (vk == VK_UP || vk == VK_DOWN) {
            // Lưu trạng thái hiện tại nếu chưa đi vào lịch sử
            if (currentHistoryIndex == -1 && bufferLen > 0) {
                // Lưu trữ command hiện tại tạm thời
                static char tempBuffer[MAX_COMMAND_LENGTH];
                strcpy(tempBuffer, buffer);
            }
            
            // Xóa dòng hiện tại
            SetConsoleCursorPosition(hStdout, startPos);
            for (int i = 0; i < bufferLen + 5; i++) { // Thêm khoảng trống để chắc chắn xóa hết
                WriteConsole(hStdout, " ", 1, NULL, NULL);
            }
            SetConsoleCursorPosition(hStdout, startPos);
            
            // Lấy command từ lịch sử
            if (vk == VK_UP) {
                // Đi lên trong lịch sử (cũ hơn)
                if (currentHistoryIndex < historyCount - 1) {
                    currentHistoryIndex++;
                    strcpy(buffer, commandHistory[currentHistoryIndex]);
                }
            } else { // VK_DOWN
                // Đi xuống trong lịch sử (mới hơn)
                if (currentHistoryIndex > 0) {
                    currentHistoryIndex--;
                    strcpy(buffer, commandHistory[currentHistoryIndex]);
                } else if (currentHistoryIndex == 0) {
                    // Quay lại dòng trống hoặc command đang soạn
                    currentHistoryIndex = -1;
                    buffer[0] = '\0';
                }
            }
            
            bufferLen = strlen(buffer);
            cursorPos = bufferLen;
            
            // Hiển thị command mới
            WriteConsole(hStdout, buffer, bufferLen, NULL, NULL);
        }
        
        // Xử lý phím mũi tên trái
        else if (vk == VK_LEFT && cursorPos > 0) {
            cursorPos--;
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(hStdout, &csbi);
            csbi.dwCursorPosition.X--;
            SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);
        }
        
        // Xử lý phím mũi tên phải
        else if (vk == VK_RIGHT && cursorPos < bufferLen) {
            cursorPos++;
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(hStdout, &csbi);
            csbi.dwCursorPosition.X++;
            SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);
        }
        
        // Xử lý phím Backspace
        else if (vk == VK_BACK && cursorPos > 0) {
            // Di chuyển tất cả ký tự về phía trái
            for (int i = cursorPos - 1; i < bufferLen - 1; i++) {
                buffer[i] = buffer[i + 1];
            }
            bufferLen--;
            cursorPos--;
            buffer[bufferLen] = '\0';
            
            // Cập nhật hiển thị
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(hStdout, &csbi);
            SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);
            csbi.dwCursorPosition.X--;
            SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);
            
            // In lại phần còn lại của dòng và thêm khoảng trắng ở cuối
            WriteConsole(hStdout, buffer + cursorPos, bufferLen - cursorPos, NULL, NULL);
            WriteConsole(hStdout, " ", 1, NULL, NULL);
            
            // Đặt con trỏ về vị trí đúng
            GetConsoleScreenBufferInfo(hStdout, &csbi);
            csbi.dwCursorPosition.X = startPos.X + cursorPos;
            SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);
        }
        
        // Xử lý phím Delete
        else if (vk == VK_DELETE && cursorPos < bufferLen) {
            // Di chuyển tất cả ký tự về phía trái từ vị trí con trỏ
            for (int i = cursorPos; i < bufferLen - 1; i++) {
                buffer[i] = buffer[i + 1];
            }
            bufferLen--;
            buffer[bufferLen] = '\0';
            
            // Cập nhật hiển thị
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(hStdout, &csbi);
            
            // In lại phần còn lại của dòng và thêm khoảng trắng ở cuối
            WriteConsole(hStdout, buffer + cursorPos, bufferLen - cursorPos, NULL, NULL);
            WriteConsole(hStdout, " ", 1, NULL, NULL);
            
            // Đặt con trỏ về vị trí đúng
            csbi.dwCursorPosition.X = startPos.X + cursorPos;
            SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);
        }
        
        // Xử lý các ký tự thông thường
        else if (ch >= 32 && ch <= 126 && bufferLen < bufferSize - 1) {
            // Tạo khoảng trống cho ký tự mới
            for (int i = bufferLen; i > cursorPos; i--) {
                buffer[i] = buffer[i - 1];
            }
            buffer[cursorPos] = ch;
            cursorPos++;
            bufferLen++;
            buffer[bufferLen] = '\0';
            
            // Hiển thị ký tự mới
            WriteConsole(hStdout, &ch, 1, NULL, NULL);
            
            // Nếu không phải nhập ở cuối, cần điều chỉnh hiển thị
            if (cursorPos < bufferLen) {
                // In phần còn lại của chuỗi
                WriteConsole(hStdout, buffer + cursorPos, bufferLen - cursorPos, NULL, NULL);
                
                // Đặt lại con trỏ
                CONSOLE_SCREEN_BUFFER_INFO csbi;
                GetConsoleScreenBufferInfo(hStdout, &csbi);
                COORD newPos = {startPos.X + cursorPos, startPos.Y};
                SetConsoleCursorPosition(hStdout, newPos);
            }
        }
    }

    // Khôi phục chế độ console ban đầu
    SetConsoleMode(hStdin, originalMode);
    return bufferLen;
}