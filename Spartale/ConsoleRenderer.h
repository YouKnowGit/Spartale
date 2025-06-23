#pragma once
#include <windows.h>
#include <string>

class ConsoleRenderer
{
public:
    ConsoleRenderer();
    ~ConsoleRenderer();

    // 지정된 크기로 버퍼를 초기화
    void Initialize(int width, int height);

    // (x, y) 위치에 문자 그리기
    void Draw(int x, int y, wchar_t character, WORD attribute = 0x000F); // 0x000F: 흰색 글씨, 검은 배경

    // 버퍼의 모든 내용을 화면에 출력
    void Render();

    // 버퍼를 깨끗하게 지우기
    void Clear();

private:
    int m_width;
    int m_height;
    HANDLE m_hConsole;
    CHAR_INFO* m_buffer; // 백 버퍼 역할을 할 메모리 공간
};