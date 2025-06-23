#pragma once

#include <memory>
#include "GameLogic/Units/Player.h"
#include "GameLogic/Map/Field.h"
#include "Utils/ConsoleRenderer.h"

class GameWorld
{
public:
    // 생성자에서 플레이어 객체를 받음
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

    // GameWorld는 플레이어, 필드, 렌더러를 관리
    std::unique_ptr<Player> m_player;
    Field m_field;
    ConsoleRenderer m_renderer;
};