#include "Utils/ConsoleRenderer.h"

ConsoleRenderer::ConsoleRenderer()
    : m_width(0), m_height(0), m_hConsole(nullptr), m_buffer(nullptr)
{
}

ConsoleRenderer::~ConsoleRenderer()
{
    if (m_buffer)
    {
        delete[] m_buffer;
    }
}

void ConsoleRenderer::Initialize()
{
    m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // 현재 콘솔 창의 정보를 읽음
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(m_hConsole, &csbi))
    {

        m_width = 80;
        m_height = 25;
    }
    else
    {
        // 읽어온 실제 창 크기를 사용
        m_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        m_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }

    // 이전에 할당된 버퍼가 있다면 삭제
    if (m_buffer)
    {
        delete[] m_buffer;
    }

    // 읽어온 현재 창 크기에 맞춰 백 버퍼를 생성

    m_buffer = new CHAR_INFO[m_width * m_height];
}

void ConsoleRenderer::Draw(int x, int y, wchar_t character, WORD attribute)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
    {
        return;
    }
    int index = y * m_width + x;
    m_buffer[index].Char.UnicodeChar = character;
    m_buffer[index].Attributes = attribute;
}

void ConsoleRenderer::Render()
{
    SMALL_RECT destRect = { 0, 0, (SHORT)m_width - 1, (SHORT)m_height - 1 };
    COORD bufferSize = { (SHORT)m_width, (SHORT)m_height };
    COORD bufferCoord = { 0, 0 };

    WriteConsoleOutputW(m_hConsole, m_buffer, bufferSize, bufferCoord, &destRect);
}

void ConsoleRenderer::Clear()
{
    for (int i = 0; i < m_width * m_height; ++i)
    {
        m_buffer[i].Char.UnicodeChar = L' ';
        m_buffer[i].Attributes = 0x000F;  // 기본 속성 (흰색 글씨, 검은 배경)
    }
}