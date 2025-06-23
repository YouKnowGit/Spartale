#pragma once

#include <memory>
#include "Player.h"
#include "Field.h"
#include "ConsoleRenderer.h"

class GameWorld
{
public:
    // �����ڿ��� �÷��̾� ��ü�� ����
    GameWorld(std::unique_ptr<Player> player);
    ~GameWorld() = default;

    void Run();

private:
    void Initialize();
    void GameLoop();
    void ProcessInput();
    void Render();

private:
    bool m_bIsRunning;
    int m_currentMapID;

    // GameWorld�� �÷��̾�, �ʵ�, �������� ����
    std::unique_ptr<Player> m_player;
    Field m_field;
    ConsoleRenderer m_renderer;
};