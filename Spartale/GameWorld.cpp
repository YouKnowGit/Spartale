#include "GameWorld.h"
#include <iostream>
#include <conio.h>

// 생성자
GameWorld::GameWorld(std::unique_ptr<Player> player)
    : m_player(std::move(player)),
    m_bIsRunning(true),
    m_currentMapID(0)
{
}

void GameWorld::Initialize()
{
    // 렌더러 초기화
    m_renderer.Initialize();

    // 첫 맵 로드 및 플레이어 위치 설정
    m_field.LoadMapFromFile("Town.json");
    m_player->CurrentLocation = { 5, 5 };
}

void GameWorld::Run()
{
    Initialize();
    GameLoop();
}

void GameWorld::GameLoop()
{
    while (m_bIsRunning)
    {
        Render();
        ProcessInput();
        Sleep(16); // 60FPS
    }
}

void GameWorld::ProcessInput()
{
    if (_kbhit())
    {
        int key = _getch();

        if (key == 'q' || key == 'Q')
        {
            m_bIsRunning = false;
            return;
        }
        // . . . ( 필드 내에서 구현할 기능들에 대한 키 입력 처리는 여기서 ex. 'g' 로 상호작용 등 ) . . .

        if (key == 224)
        {
            key = _getch();
            FMapLocation nextLocation = m_player->CurrentLocation;

            // 입력받은 Key 가 224(방향키)인 경우 방향에 맞게 Player 의 Position 과 Direction 을 변경
            switch (key)
            {
            case 72: // (Up)
                nextLocation.Y--;
                m_player->SetDirection(Direction::UP);
                break;
            case 80: // (Down)
                nextLocation.Y++;
                m_player->SetDirection(Direction::DOWN);
                break;
            case 75: // (Left)
                nextLocation.X--;
                m_player->SetDirection(Direction::LEFT);
                break;
            case 77: // (Right)
                nextLocation.X++;
                m_player->SetDirection(Direction::RIGHT);
                break;
            }

			// IsWalkable 체크 후 이동. NPC 등 다른 오브젝트에 의해 충돌 로직도 여기서 추가
            if (m_field.IsWalkable(nextLocation.X, nextLocation.Y))
            {
                m_player->CurrentLocation = nextLocation;
            }

            // 포탈 이동 로직
            const Portal* portal = m_field.GetPortalAt(m_player->CurrentLocation.X, m_player->CurrentLocation.Y);
            if (portal != nullptr)
            {
                const int destinationX = portal->destX;
                const int destinationY = portal->destY;
                const int nextMapID = portal->destMapID;

                std::string nextMap;
                if (nextMapID == 0) {
                    nextMap += "Town.json";
                }
                else {
                    nextMap += "Field_" + std::to_string(nextMapID) + ".json";
                }

                if (m_field.LoadMapFromFile(nextMap))
                {
                    m_currentMapID = nextMapID;
                    m_player->CurrentLocation = { destinationX, destinationY };
                }
            }
        }
    }
}

void GameWorld::Render()
{
    m_renderer.Clear();
    m_field.Draw(m_player.get(), m_renderer);
    m_renderer.Render();
}