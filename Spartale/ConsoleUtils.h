#pragma once
#include <windows.h>
#include <string>
#include <vector>

namespace ConsoleUtils {
    void gotoxy(int x, int y);
    void ShowConsoleCursor(bool showFlag);
    void clearScreen();
    void DrawBox(int x, int y, int width, int height);
    int SelectMenuVertical(const std::vector<std::wstring>& options, int startX, int startY); 
    void ClearInputBuffer(); 
    void SafeWriteUnicodeLine(HANDLE hConsole, const std::wstring& line, SHORT y);
}