#include <iostream>
#include <memory>
#include <locale>
#include <fcntl.h>
#include <io.h>
#include <conio.h>
#include <Windows.h>

#include "GameLogic/Units/Player.h"
#include "GameLogic/Units/Monster.h"
#include "GameLogic/Skills/AB_NormalAttack.h"
#include "GameLogic/BattleManager.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "GameLogic/Skills/AB_PoisonCloud.h"
#include "Utils/ConsoleUtils.h"
#include "GameLogic/Map/Field.h"
#include "Core/GameWorld.h"


using namespace std;

int main()
{
    /*
	ConsoleUtils::ShowConsoleCursor(false);
    auto player = make_unique<Player>(L"주인공");
    EGameState currentState = EGameState::World; // 게임은 월드 탐험부터 시작

    while (currentState != EGameState::Quit)
    {
        switch (currentState)
        {
        case EGameState::World:
        {
            GameWorld world(std::move(player));
            world.Run(); // 월드 탐험 루프 실행

            currentState = EGameState::Quit; 
            break;
        }
        case EGameState::Battle:
        {
            break;
        }
        // ... 메인 메뉴 등 다른 상태 추가 가능
        }
    }

    ConsoleUtils::gotoxy(0, 26);
    cout << "게임을 종료합니다." << endl;
    ConsoleUtils::ShowConsoleCursor(true);

    return 0;
    */

	// 전투 테스트용 코드 (주석 처리)
    // 1. 플레이어와 몬스터 생성
    auto player = make_unique<Player>(L"주인공");
    auto monster = make_unique<Monster>(L"고블린", 70.f, 10.f, 5.f);
    ConsoleRenderer renderer;
    renderer.Initialize(); 

    // 2. 각자에게 '일반 공격' 스킬 부여 및 0번 슬롯에 장착
    // 현재 AI 는 Monster 클래스의 RunAI() 함수 사용 (0번 스킬만 사용하도록 구현되어 있음)
    
    auto playerAttack = make_unique<AB_NormalAttack>();
    player->GetAbilityComponent()->GrantAbility(std::move(playerAttack));
    player->GetAbilityComponent()->EquipAbility(0, player->GetAbilityComponent()->GetGrantedAbility(0));

    auto poisonCloud = make_unique<AB_PoisonCloud>();
    player->GetAbilityComponent()->GrantAbility(std::move(poisonCloud));
    player->GetAbilityComponent()->EquipAbility(1, player->GetAbilityComponent()->GetGrantedAbility(1));

    auto monsterAttack = make_unique<AB_NormalAttack>();
    monster->GetAbilityComponent()->GrantAbility(std::move(monsterAttack));
    monster->GetAbilityComponent()->EquipAbility(0, monster->GetAbilityComponent()->GetGrantedAbility(0));
    
    // 3. 배틀매니저 생성 및 전투 루프 실행
    BattleManager battleManager(player.get(), monster.get(), renderer);
    battleManager.Run();
    

    return 0;
}