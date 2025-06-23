#pragma once
#include <windows.h>
#include <string>

class ConsoleRenderer
{
public:
    ConsoleRenderer();
    ~ConsoleRenderer();

    // ������ ũ��� ���۸� �ʱ�ȭ
    void Initialize(int width, int height);

    // (x, y) ��ġ�� ���� �׸���
    void Draw(int x, int y, wchar_t character, WORD attribute = 0x000F); // 0x000F: ��� �۾�, ���� ���

    // ������ ��� ������ ȭ�鿡 ���
    void Render();

    // ���۸� �����ϰ� �����
    void Clear();

private:
    int m_width;
    int m_height;
    HANDLE m_hConsole;
    CHAR_INFO* m_buffer; // �� ���� ������ �� �޸� ����
};