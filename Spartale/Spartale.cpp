#include <iostream>
#include <memory>
#include <locale>
#include <fcntl.h>
#include <io.h>
#include <conio.h>
#include <Windows.h>

#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Utils/ConsoleUtils.h"
#include "Core/GameWorld.h"
#include "GameLogic/Units/Player.h"
#include "GameLogic/Units/Monster.h"
#include "GameLogic/Map/Field.h"
#include "GameLogic/BattleManager.h"
#include "GameLogic/DataManager.h"
#include "GameLogic/MainMenu.h"


using namespace std;

int main()
{

    setlocale(LC_ALL, "");
    // 커서 숨기기
    ConsoleUtils::ShowConsoleCursor(false);

    DataManager::GetInstance().LoadMonsterData("Data/Monsters.json");
    DataManager::GetInstance().LoadSkillData("Data/Skills.json");
	DataManager::GetInstance().LoadItemData("Data/Item.json");
    
    EGameState currentState = EGameState::MainMenu;
    ConsoleRenderer renderer;
    renderer.Initialize();

    while (currentState != EGameState::Quit)
    {
        switch (currentState)
        {
        case EGameState::MainMenu:
        {
            // MainMenu 상태일 때
            MainMenu menu(renderer);
            currentState = menu.Run(); // 메뉴 실행 후, 그 결과(다음 상태)를 받음
            break;
        }


        case EGameState::World:
        {
            // World 상태일 때 (새 게임 시작)
            auto player = make_unique<Player>(L"주인공");

            GameWorld world(std::move(player));
            world.Run();

            // GameWorld 루프가 끝나면(ESC -> 메인메뉴로 나가기 선택 시)
            // 게임을 종료하는 대신, MainMenu 상태로 전환합니다.
            currentState = EGameState::MainMenu;
            break;
        }

        case EGameState::Battle:
        {
            // TODO: 전투 상태 로직 구현
            // 전투가 끝나면 World나 MainMenu로 돌아가야 함
            cout << "전투 시스템 미구현" << endl;
            Sleep(1000);
            currentState = EGameState::MainMenu; // 임시로 메뉴로 복귀
            break;
        }
        }
    }

    // --- 게임 종료 ---
    renderer.Clear();
    std::wstring exitMessage = L"게임을 종료합니다.";
    for (int i = 0; i < exitMessage.length(); ++i)
    {
        renderer.Draw(30 + i * 2, 12, exitMessage[i]);
    }
    renderer.Render();
    Sleep(2000);

    ConsoleUtils::ShowConsoleCursor(true); // 커서 다시 표시

    return 0;

    return 0;
}