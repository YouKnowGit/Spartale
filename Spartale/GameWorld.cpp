#include "GameWorld.h"
#include <iostream>
#include <conio.h>

// ������
GameWorld::GameWorld(std::unique_ptr<Player> player)
    : m_player(std::move(player)),
    m_bIsRunning(true),
    m_currentMapID(0)
{
}

void GameWorld::Initialize()
{
    // ������ �ʱ�ȭ
    m_renderer.Initialize();

    // ù �� �ε� �� �÷��̾� �ʱ� ��ġ ���� (�ʿ� ��)
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
        Sleep(16); // �� 60FPS
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
        // . . . ( �ʵ� ������ ������ ��ɵ鿡 ���� Ű �Է� ó���� ���⼭ ex. 'g' �� ��ȣ�ۿ� �� ) . . .

        if (key == 224)
        {
            key = _getch();
            FMapLocation nextLocation = m_player->CurrentLocation;

			// �Է¹��� Key �� 224(����Ű)�� ��� ���⿡ �°� Player �� Position �� Direction �� ����
            switch (key)
            {
            case 72: // �� (Up)
                nextLocation.Y--;
                m_player->SetDirection(Direction::UP);
                break;
            case 80: // �Ʒ� (Down)
                nextLocation.Y++;
                m_player->SetDirection(Direction::DOWN);
                break;
            case 75: // ���� (Left)
                nextLocation.X--;
                m_player->SetDirection(Direction::LEFT);
                break;
            case 77: // ������ (Right)
                nextLocation.X++;
                m_player->SetDirection(Direction::RIGHT);
                break;
            }

			// IsWalkable üũ �� �̵�. NPC �� �ٸ� ������Ʈ�� ���� �浹 ������ ���⼭ �߰�
            if (m_field.IsWalkable(nextLocation.X, nextLocation.Y))
            {
                m_player->CurrentLocation = nextLocation;
            }

            // ��Ż �̵� ����
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