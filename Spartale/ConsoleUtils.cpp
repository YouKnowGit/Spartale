#include "ConsoleUtils.h"
#include <iostream>
#include <conio.h>
#include <vector>

using namespace std;

namespace ConsoleUtils {
    static int g_nScreenIndex;
    static HANDLE g_hBuffer[2];

    void DrawBox(int x, int y, int width, int height) {
        gotoxy(x, y); wcout << L"┌";
        for (int i = 0; i < width - 2; ++i) wcout << L"─";
        wcout << L"┐";
        for (int i = 1; i < height - 1; ++i) {
            gotoxy(x, y + i); wcout << L"│";
            gotoxy(x + width - 1, y + i); wcout << L"│";
        }
        gotoxy(x, y + height - 1); wcout << L"└";
        for (int i = 0; i < width - 2; ++i) wcout << L"─";
        wcout << L"┘";
    }

    void gotoxy(int x, int y) {
        COORD pos = { (SHORT)x, (SHORT)y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }

    void ShowConsoleCursor(bool showFlag) {
        HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(out, &cursorInfo);
        cursorInfo.bVisible = showFlag;
        SetConsoleCursorInfo(out, &cursorInfo);
    }

    void clearScreen() {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        SMALL_RECT scrollRect;
        COORD scrollTarget;
        CHAR_INFO fill;

        // Get the number of character cells in the current buffer.
        if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
        {
            return;
        }

        // Scroll the rectangle of the entire buffer.
        scrollRect.Left = 0;
        scrollRect.Top = 0;
        scrollRect.Right = csbi.dwSize.X;
        scrollRect.Bottom = csbi.dwSize.Y;

        // Scroll it upwards off the top of the buffer with a destination of 0, -9999.
        scrollTarget.X = 0;
        scrollTarget.Y = (SHORT)(0 - csbi.dwSize.Y);

        // Fill with empty spaces with the buffer's default text attribute.
        fill.Char.UnicodeChar = TEXT(' ');
        fill.Attributes = csbi.wAttributes;

        ScrollConsoleScreenBuffer(hConsole, &scrollRect, NULL, scrollTarget, &fill);

        // Move cursor to home
        csbi.dwCursorPosition.X = 0;
        csbi.dwCursorPosition.Y = 0;

        SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
    }

    int SelectMenuVertical(const vector<wstring>& options, int startX, int startY) {
        ShowConsoleCursor(false);
        int selection = 0;
        int key;
        while (true) {
            for (int i = 0; i < options.size(); ++i) {
                gotoxy(startX, startY + i);
                if (i == selection) wcout << L"▶ " << options[i];
                else wcout << L"  " << options[i] << L"  ";
            }
            key = _getch();
            if (key == 224) {
                key = _getch();
                switch (key) {
                case 72: selection = (selection == 0) ? options.size() - 1 : selection - 1; break;
                case 80: selection = (selection == options.size() - 1) ? 0 : selection + 1; break;
                }
            }
            else if (key == 13) {
                ShowConsoleCursor(true);
                return selection;
            }
        }
    }
}
void ConsoleUtils::SafeWriteUnicodeLine(HANDLE hConsole, const std::wstring& line, SHORT y)
{
    DWORD written;
    COORD pos = { 0, y };
    SetConsoleCursorPosition(hConsole, pos);
    WriteConsoleW(hConsole, line.c_str(), (DWORD)line.length(), &written, nullptr);
}
void ConsoleUtils::ClearInputBuffer()
{
    // _kbhit() : 키보드 입력이 버퍼에 있는지 확인하는 함수. 있으면 true 반환.
    // 버퍼에 입력이 없을 때까지 루프를 돌며 _getch()로 문자를 읽어들여 소진시킴
    while (_kbhit())
    {
        _getch();
    }
}