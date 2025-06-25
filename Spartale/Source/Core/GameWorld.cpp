#include "Core/GameWorld.h"
#include "Framework/AbilitySystem/AttributeSet.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "GameLogic/MainMenu.h"
#include "GameLogic/PauseMenu.h"
#include "GameLogic/NPCMenu.h"
#include "GameLogic/BattleManager.h"
#include "GameLogic/DataManager.h"
#include "GameLogic/Units/Monster.h"
#include "GameLogic/Skills/SkillFactory.h"
#include "Utils/StringUtils.h"

#include <iostream>
#include <conio.h>
#include <memory>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")
// 생성자
GameWorld::GameWorld(std::unique_ptr<Player> player)
    : m_player(std::move(player)),
    m_bIsRunning(true),
    m_currentMapID(0),
    m_dist(0.0f, 1.0f) // 0.0과 1.0 사이의 실수를 균등하게 생성
{
    std::random_device rd;
    m_rng.seed(rd());
}

void GameWorld::Initialize()
{
    // 렌더러 초기화
    m_renderer.Initialize();

    if (!m_field.LoadMapFromFile(m_player->GetMapID())) {
        std::wcout << L"맵 불러오기 실패" << std::endl;
    }
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

        if (key == 27) // ESC
        {
            PauseMenu pauseMenu(m_renderer, *m_player);
            EPauseMenuResult result = pauseMenu.Run();

            if (result == EPauseMenuResult::GoToMainMenu)
            {
                m_bIsRunning = false; // GameWorld 루프를 종료하여 메인 메뉴로 돌아가게 함
            }
            // EPauseMenuResult::Resume의 경우, 아무것도 하지 않으면
            // 이 함수가 끝나고 GameLoop가 계속되므로 자연스럽게 게임으로 복귀됩니다.
            return; // PauseMenu가 끝난 후 다른 키 입력이 처리되지 않도록 return
        }

        if (key == 'g' || key == 'G' || key == 'z' || key == 'Z' || key == 13) // 상호작용 'G' 'Z' 'Enter'
        {
            FMapLocation playerPos = m_player->CurrentLocation;
            Direction dir = m_player->GetDirection();

            int targetX = playerPos.X;
            int targetY = playerPos.Y;

            switch (dir)
            {
            case Direction::UP:    targetY -= 1; break;
            case Direction::DOWN:  targetY += 1; break;
            case Direction::LEFT:  targetX -= 1; break;
            case Direction::RIGHT: targetX += 1; break;
            }

            TileType tile = m_field.GetTileType(targetX, targetY);
            if (tile == TileType::NPC_Heal)
            {
                // 여기 힐 사운드
                AttributeSet* stats = m_player->GetAbilityComponent()->GetAttributeSet();
                stats->HP.CurrentValue = stats->HP.BaseValue;
                stats->MP.CurrentValue = stats->MP.BaseValue;
            }
            else if (tile == TileType::NPC_Shop) 
            {
                PauseMenu pauseMenu(m_renderer, *m_player);
                EPauseMenuResult result = pauseMenu.Shop();
                if (result == EPauseMenuResult::GoToMainMenu)
                {
                    m_bIsRunning = false; // GameWorld 루프를 종료하여 메인 메뉴로 돌아가게 함
                }
                // EPauseMenuResult::Resume의 경우, 아무것도 하지 않으면
                // 이 함수가 끝나고 GameLoop가 계속되므로 자연스럽게 게임으로 복귀됩니다.
                return; // PauseMenu가 끝난 후 다른 키 입력이 처리되지 않도록 return
            }

        }
        // . . . ( 필드 내에서 구현할 기능들에 대한 키 입력 처리는 여기서 ex. 'g' 로 상호작용 등 ) . . .

        if (key == 224)
        {
            key = _getch();
            FMapLocation nextLocation = m_player->CurrentLocation;
            PlaySound(Move_map, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOSTOP);

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

                // --- 이동한 타일이 수풀이면 전투 확률 체크 ---
                TileType currentTile = m_field.GetTileType(nextLocation.X, nextLocation.Y);
                if (currentTile == TileType::BUSH)
                {
                    // 0.0 ~ 1.0 사이의 랜덤 실수를 생성
                    if (m_dist(m_rng) < m_encounterChance)
                    {
                        StartBattle();
                        return;
                    }
                }
            }

            // 포탈 이동 로직
            const Portal* portal = m_field.GetPortalAt(m_player->CurrentLocation.X, m_player->CurrentLocation.Y);
            if (portal != nullptr)
            {
                const int destinationX = portal->destX;
                const int destinationY = portal->destY;
                const int nextMapID = portal->destMapID;

                std::string nextMap;

                if (m_field.LoadMapFromFile(nextMapID))
                {
                    m_currentMapID = nextMapID;
                    m_player->SetMapID(m_currentMapID);
                    m_player->CurrentLocation = { destinationX, destinationY };
                }
            }
        }
    }
}

void GameWorld::StartBattle()
{
    const auto& encounterList = m_field.GetEncounterList();
    if (encounterList.empty()) return;

    // 맵의 조우 목록에서 몬스터 ID를 랜덤으로 결정
    std::uniform_int_distribution<int> monsterDist(0, static_cast<int>(encounterList.size()) - 1);
    std::string monsterId = encounterList[monsterDist(m_rng)];

    // 모듈화된 함수를 호출하여 몬스터를 생성
    auto monster = CreateRandomizedMonster(monsterId);
    if (!monster)
    {
        return;
    }

    // 전투 실행 및 결과 처리
    BattleManager battleManager(m_player.get(), monster.get(), m_renderer);
    EBattleResult result = battleManager.Run(); // 전투 결과를 받음

    // 전투 결과에 따라 플레이어가 패배했는지 여부만 체크
    if (result == EBattleResult::PlayerLost)
    {
        m_bIsRunning = false;
    }
}
std::unique_ptr<Monster> GameWorld::CreateRandomizedMonster(const std::string& monsterId)
{
    const MonsterData* data = DataManager::GetInstance().GetMonsterData(monsterId);
    if (!data)
    {
        return nullptr;
    }

    // 레벨은 기본 레벨 ( +- 1 )
    std::uniform_real_distribution<float> statMultiplier(0.9f, 1.1f);
    std::uniform_int_distribution<int> levelDist(data->baseLevel - 1, data->baseLevel + 1);

    // 모든 스탯을 랜덤화
    int finalLevel = (std::max)(1, levelDist(m_rng));
    float finalHp = data->baseHp * statMultiplier(m_rng);
    float finalStrength = data->baseStrength * statMultiplier(m_rng);
    float finalDefence = data->baseDefence * statMultiplier(m_rng);
    float finalAgility = data->baseAgility * statMultiplier(m_rng);
    float finalIntelligence = data->baseIntelligence * statMultiplier(m_rng);
    float finalMagicResistance = data->baseMagicResistance * statMultiplier(m_rng);

    auto monster = std::make_unique<Monster>(data->name, finalHp, finalStrength, finalDefence, finalLevel);
    
    AttributeSet* monsterStats = monster->GetAbilityComponent()->GetAttributeSet();
    if (monsterStats)
    {
        monsterStats->Agility = FAttributeData(finalAgility);
        monsterStats->Intelligence = FAttributeData(finalIntelligence);
        monsterStats->MagicResistance = FAttributeData(finalMagicResistance);
    }

    if (!data->skillIds.empty())
    {
        for (const std::string& skillId : data->skillIds)
        {
            monster->GetAbilityComponent()->GrantAbility(SkillFactory::CreateSkill(skillId));
        }

        const auto& grantedAbilities = monster->GetAbilityComponent()->GetGrantedAbilities();
        for (size_t i = 0; i < grantedAbilities.size(); ++i)
        {
            // 최대 4개의 슬롯
            if (i >= 4) break;

            // i번 슬롯에 i번째로 부여된 스킬을 장착
            monster->GetAbilityComponent()->EquipAbility(static_cast<int32_t>(i), grantedAbilities[i].get());
        }
    }

    return monster;
}
void GameWorld::Render()
{
    m_renderer.Clear();
    m_field.Draw(m_player.get(), m_renderer);
    m_renderer.Render();
}