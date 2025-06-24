#pragma once
#include "Core/Global.h"
#include "Utils/ConsoleRenderer.h"
#include <vector>
#include <string>

class MainMenu
{
public:
    MainMenu(ConsoleRenderer& renderer);
    ~MainMenu() = default;

    EGameState Run();

private:
    void ProcessInput();

    void Render();
    void DrawTitle();
    void DrawMenuOptions();
    void OnEnterPressed();

    ConsoleRenderer& m_renderer;
    std::vector<std::wstring> m_menuOptions;
    int m_currentSelection;
    bool m_bIsRunning;
    EGameState m_nextState;

    const wchar_t* m_navigateSoundPath;
    const wchar_t* m_confirmSoundPath;
};