#include "Core/GameWorld.h"
#include "Framework/AbilitySystem/AttributeSet.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "GameLogic/Skills/AB_NormalAttack.h"
#include "GameLogic/Skills/AB_PoisonCloud.h"
#include "GameLogic/MainMenu.h"
#include "GameLogic/PauseMenu.h"
#include "GameLogic/BattleManager.h"
#include "GameLogic/DataManager.h"
#include "GameLogic/Units/Monster.h"

#include <iostream>
#include <conio.h>
#include <memory>
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

    // 첫 맵 로드 및 플레이어 위치 설정
    m_field.LoadMapFromFile("Data/Town.json");
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

                // --- 이동한 타일이 수풀이면 전투 확률 체크 ---
                TileType currentTile = m_field.GetTileType(nextLocation.X, nextLocation.Y);
                if (currentTile == TileType::BUSH)
                {
                    // 0.0 ~ 1.0 사이의 랜덤 실수를 생성
                    if (m_dist(m_rng) < m_encounterChance)
                    {
                        StartBattle();
                        // 전투 후에는 화면을 다시 그려야 하므로 루프를 한번 빠져나감
                        // (Render() 함수가 GameLoop에서 호출되기 때문)
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
                if (nextMapID == 0) {
                    nextMap += "Data/Town.json";
                }
                else {
                    nextMap += "Data/Field_" + std::to_string(nextMapID) + ".json";
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

void GameWorld::StartBattle()
{
    const auto& encounterList = m_field.GetEncounterList();
    if (encounterList.empty()) return;

    // 맵의 조우 목록에서 몬스터 ID를 랜덤으로 하나 뽑습니다.
    std::uniform_int_distribution<int> monsterDist(0, static_cast<int>(encounterList.size()) - 1);
    std::string monsterId = encounterList[monsterDist(m_rng)];

    // 모듈화된 함수를 호출하여 몬스터를 생성합니다.
    auto monster = CreateRandomizedMonster(monsterId);
    if (!monster)
    {
        // 몬스터 생성에 실패하면 (예: 데이터베이스에 없는 ID) 전투를 시작하지 않습니다.
        return;
    }
    // 전투 실행 및 결과 처리
    BattleManager battleManager(m_player.get(), monster.get(), m_renderer);
    EBattleResult result = battleManager.Run(); // 전투 결과를 받음

    // 전투 결과에 따라 플레이어가 패배했는지 여부만 체크
    if (result == EBattleResult::PlayerLost)
    {
        // TODO: 게임오버 처리 로직
        m_bIsRunning = false; // 임시로 월드 루프 종료
    }
}
std::unique_ptr<Monster> GameWorld::CreateRandomizedMonster(const std::string& monsterId)
{
    const MonsterData* data = DataManager::GetInstance().GetMonsterData(monsterId);
    if (!data)
    {
        return nullptr;
    }

    // --- 랜덤화 로직 ---
    std::uniform_real_distribution<float> statMultiplier(0.9f, 1.1f);
    std::uniform_int_distribution<int> levelDist(data->baseLevel - 1, data->baseLevel + 1);

    // 모든 스탯을 랜덤화합니다.
    int finalLevel = (std::max)(1, levelDist(m_rng));
    float finalHp = data->baseHp * statMultiplier(m_rng);
    float finalStrength = data->baseStrength * statMultiplier(m_rng);
    float finalDefence = data->baseDefence * statMultiplier(m_rng);
    float finalAgility = data->baseAgility * statMultiplier(m_rng);
    float finalIntelligence = data->baseIntelligence * statMultiplier(m_rng);
    float finalMagicResistance = data->baseMagicResistance * statMultiplier(m_rng);

    // 몬스터 객체 생성 (기존 생성자 그대로 사용)
    auto monster = std::make_unique<Monster>(data->name, finalHp, finalStrength, finalDefence, finalLevel);

    // --- 생성된 몬스터의 AttributeSet에 새로운 스탯들을 직접 설정합니다 ---
    AttributeSet* monsterStats = monster->GetAbilityComponent()->GetAttributeSet();
    if (monsterStats)
    {
        monsterStats->Agility.BaseValue = finalAgility;
        monsterStats->Agility.CurrentValue = finalAgility;
        monsterStats->Intelligence.BaseValue = finalIntelligence;
        monsterStats->Intelligence.CurrentValue = finalIntelligence;
        monsterStats->MagicResistance.BaseValue = finalMagicResistance;
        monsterStats->MagicResistance.CurrentValue = finalMagicResistance;
    }

    // --- 데이터 기반 스킬 부여 로직 (이전과 동일) ---
    if (!data->skillIds.empty())
    {
        for (const std::string& skillId : data->skillIds)
        {
            if (skillId == "normal_attack")
                monster->GetAbilityComponent()->GrantAbility(std::make_unique<AB_NormalAttack>());
            else if (skillId == "poison_cloud")
                monster->GetAbilityComponent()->GrantAbility(std::make_unique<AB_PoisonCloud>());
        }
        monster->GetAbilityComponent()->EquipAbility(0, monster->GetAbilityComponent()->GetGrantedAbility(0));
    }

    return monster;
}
void GameWorld::Render()
{
    m_renderer.Clear();
    m_field.Draw(m_player.get(), m_renderer);
    m_renderer.Render();
}