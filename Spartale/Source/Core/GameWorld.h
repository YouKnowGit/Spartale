#pragma once
#include <random>
#include <memory>
#include "GameLogic/Units/Player.h"
#include "GameLogic/Map/Field.h"
#include "Utils/ConsoleRenderer.h"

class Monster;

class GameWorld
{
public:
    // 생성자에서 플레이어 객체를 받음
    GameWorld(std::unique_ptr<Player> player);
    ~GameWorld() = default;

    void Run();

    void SetCheat(int id) { m_currentMapID = id; }
private:
    void Initialize();
    void GameLoop();
    void ProcessInput();
    void Render();
    void StartBattle();

    std::unique_ptr<Monster> CreateRandomizedMonster(const std::string& monsterId);

private:
    bool m_bIsRunning;
    int m_currentMapID;

    const float m_encounterChance = 0.15f; // 몬스터 조우 확률

    // GameWorld는 플레이어, 필드, 렌더러를 관리
    std::unique_ptr<Player> m_player;
    Field m_field;
    ConsoleRenderer m_renderer;

    std::mt19937 m_rng;
    std::uniform_real_distribution<float> m_dist;

    const wchar_t* Move_map = L"Sounds/Move_map.wav";
    const wchar_t* Healing = L"Sounds/Healing.wav";
    const wchar_t* Warp = L"Sounds/warp.wav";
};