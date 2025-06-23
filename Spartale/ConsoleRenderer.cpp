#include "ConsoleRenderer.h"

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

void ConsoleRenderer::Initialize(int width, int height)
{
    m_width = width;
    m_height = height;
    m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // 버퍼 메모리 할당
    m_buffer = new CHAR_INFO[width * height];

    // 콘솔 창 크기 및 버퍼 크기 조정
    SMALL_RECT consoleSize = { 0, 0, (SHORT)width - 1, (SHORT)height - 1 };
    COORD bufferSize = { (SHORT)width, (SHORT)height };

    SetConsoleWindowInfo(m_hConsole, TRUE, &consoleSize);
    SetConsoleScreenBufferSize(m_hConsole, bufferSize);
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
        m_buffer[i].Attributes = 0x000F; // 기본 속성 (흰색 글씨, 검은 배경)
    }
}