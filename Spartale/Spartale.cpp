#include <iostream>
#include <memory>
#include <locale>
#include <fcntl.h>
#include <io.h>

#include "Player.h"
#include "Monster.h"
#include "AB_NormalAttack.h"
#include "BattleManager.h"
#include "AbilitySystemComponent.h"


using namespace std;

int main()
{
    _setmode(_fileno(stdout), _O_U16TEXT);
	
    Player player;

    player.DisplayStats();
    // 장비 효과 생성
    GameplayEffect effect;
    effect.ApplicationType = EEffectApplication::Infinite;
    effect.TargetAttributeName = "Strength";
    effect.Magnitude = 5.0f;

    // 장비 생성
    Equipment sword(L"강철 검", EquipSlotType::RightHand, effect);

    // 장착
    player.Equip(&sword);

    // 출력
    player.DisplayStats();
    
    return 0;
    
}