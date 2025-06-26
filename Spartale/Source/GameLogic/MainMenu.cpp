#include "GameLogic/MainMenu.h"
#include "GameLogic/SaveManager.h"
#include "Utils/ConsoleRenderer.h"
#include <conio.h>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

MainMenu::MainMenu(ConsoleRenderer& renderer)
    : m_renderer(renderer),
    m_currentSelection(0),
    m_bIsRunning(true),
    m_nextState(EGameState::MainMenu)
{
    m_menuOptions = { L"새 게임", L"불러오기", L"설정", L"크레딧", L"게임 종료" };

    m_navigateSoundPath = L"Sounds/UI/mainmenu_select.wav"; // 메뉴 이동 효과음
    m_confirmSoundPath = L"Sounds/UI/mainmenu_confirm.wav";   // 메뉴 선택 효과음
}

EGameState MainMenu::Run()
{
    PlaySound(MainTheme, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT); // 메인 테마 재생
    while (m_bIsRunning)
    {
        ProcessInput();
        Render();
        Sleep(16);
    }
    return m_nextState;
}

void MainMenu::ProcessInput()
{
    if (!_kbhit()) return;

    int key = _getch();
    if (key == 224)
    {
        key = _getch();
        if (key == 72) // 위
        {
            m_currentSelection = (m_currentSelection == 0) ? static_cast<int>(m_menuOptions.size()) - 1 : m_currentSelection - 1;
            PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        }
        else if (key == 80) // 아래
        {
            m_currentSelection = (m_currentSelection + 1) % static_cast<int>(m_menuOptions.size());
            PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        }
    }
    else if (key == 13) // 엔터
    {
        OnEnterPressed();
    }
}

void MainMenu::Render()
{
    m_renderer.Clear();

    DrawTitle();
    DrawMenuOptions();

    std::wstring guide = L"[ ↑ / ↓ 방향키로 이동 | Enter: 선택 ]";

    int visualWidth = 0;
    for (wchar_t c : guide) {
        visualWidth += (c >= L'가' && c <= L'힣') ? 2 : 1;
    }

    int guideX = m_renderer.GetWidth() - visualWidth - 2;
    int guideY = m_renderer.GetHeight() - 2;

    m_renderer.DrawString(guideX, guideY, guide);

    m_renderer.Render();
}

void MainMenu::OnEnterPressed()
{
    PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);

    switch (m_currentSelection)
    {
    case 0: // 새 게임
        m_nextState = EGameState::World;
        m_bIsRunning = false;
        break;
    case 1: // 불러오기
        m_nextState = EGameState::LoadGame;
        m_bIsRunning = false;
    case 2: // 설정
        break;
    case 3: // 크레딧
        break;
    case 4: // 게임 종료
        m_nextState = EGameState::Quit;
        m_bIsRunning = false;
        break;
    }
}

void MainMenu::DrawTitle()
{
    int titleX = 8;
    int titleY = 3;
    const wchar_t* title[] = {
    L"██████╗ ███████╗ ███████╗██████╗ ███████╗██████╗  █████╗ ██████╗  ██████╗ ",
    L"██╔══██╗██╔════╝██╔═════╝██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔══██╗██╔═══██╗",
    L"██║  ██║█████╗  ███████╗ ██████╔╝█████╗  ██████╔╝███████║██║  ██║██║   ██║",
    L"██║  ██║██╔══╝  ╚════███╗██╔═══╝ ██╔══╝  ██╔══██╗██╔══██║██║  ██║██║   ██║",
    L"██████╔╝███████╗███████╔╝██║     ███████╗██║  ██║██║  ██║██████╔╝╚██████╔╝",
    L"╚═════╝ ╚══════╝╚══════╝ ╚═╝     ╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝  ╚═════╝ "
    };
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < wcslen(title[i]); ++j)
        {
            m_renderer.Draw(titleX + j, titleY + i, title[i][j]);
        }
    }
}

void MainMenu::DrawMenuOptions()
{
    int boxStartX = 30;
    int startY = 12;
    int boxWidth = 18;

    for (size_t i = 0; i < m_menuOptions.size(); ++i)
    {
        int currentY = startY + i * 3;

        std::wstring textToDraw = L"  " + m_menuOptions[i];

        int visualTextWidth = 0;
        for (wchar_t c : m_menuOptions[i]) {
            visualTextWidth += (c >= L'가' && c <= L'힣') ? 2 : 1;
        }

        int padding = boxWidth - 2 - visualTextWidth - 4;
        if (padding > 0) {
            textToDraw += std::wstring(padding, L' ');
        }

        if (i == m_currentSelection)
        {
            m_renderer.DrawString(boxStartX, currentY - 1, L"┏" + std::wstring(boxWidth, L'━') + L"┓");
            m_renderer.DrawString(boxStartX, currentY, L"┃" + textToDraw + L" ◀ ");
            m_renderer.DrawString(boxStartX, currentY + 1, L"┗" + std::wstring(boxWidth, L'━') + L"┛");
        }
        else
        {
            m_renderer.DrawString(boxStartX, currentY - 1, L"┌" + std::wstring(boxWidth, L'─') + L"┐");
            m_renderer.DrawString(boxStartX, currentY, L"│" + textToDraw + L"   ");
            m_renderer.DrawString(boxStartX, currentY + 1, L"└" + std::wstring(boxWidth, L'─') + L"┘");
        }
    }
}