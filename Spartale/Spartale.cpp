#include <iostream>
#include <memory>
#include <locale>

#include "Player.h"
#include "Monster.h"
#include "AB_NormalAttack.h"
#include "BattleManager.h"
#include "AbilitySystemComponent.h"

using namespace std;

int main()
{
	// 한글 지원을 위한 로케일 설정
    wcout.imbue(locale(""));

    // 1. 플레이어와 몬스터 생성
    auto player = make_unique<Player>();
    auto monster = make_unique<Monster>(L"고블린", 70.f, 10.f, 5.f);

    // 2. 각자에게 '일반 공격' 스킬 부여 및 0번 슬롯에 장착
    // 현재 AI 는 Monster 클래스의 RunAI() 함수 사용 (0번 스킬만 사용하도록 구현되어 있음)
    auto playerAttack = make_unique<AB_NormalAttack>();
    player->GetAbilityComponent()->GrantAbility(std::move(playerAttack));
    player->GetAbilityComponent()->EquipAbility(0, player->GetAbilityComponent()->GetGrantedAbility(0));

    auto monsterAttack = make_unique<AB_NormalAttack>();
    monster->GetAbilityComponent()->GrantAbility(std::move(monsterAttack));
    monster->GetAbilityComponent()->EquipAbility(0, monster->GetAbilityComponent()->GetGrantedAbility(0));

    // 3. 배틀매니저 생성 및 전투 루프 실행
    BattleManager battleManager(player.get(), monster.get());
    battleManager.Run();

    return 0;
}