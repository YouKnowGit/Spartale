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

void ConsoleRenderer::Initialize()
{
    m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // ���� �ܼ� â�� ������ �о�ɴϴ�.
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(m_hConsole, &csbi))
    {
        // ���� ó��
        m_width = 80;
        m_height = 25;
    }
    else
    {
        // �о�� ���� â ũ�⸦ ����մϴ�.
        m_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        m_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }

    // ������ �Ҵ�� ���۰� �ִٸ� �����ϰ� �����մϴ�.
    if (m_buffer)
    {
        delete[] m_buffer;
    }
    // �о�� ���� â ũ�⿡ ���� �� ���۸� �����մϴ�.
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
        m_buffer[i].Attributes = 0x000F; // �⺻ �Ӽ� (��� �۾�, ���� ���)
    }
}