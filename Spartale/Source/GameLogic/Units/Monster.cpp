#include "GameLogic/Units/Monster.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/AttributeSet.h"

Monster::Monster(const std::wstring& InName, float InHP, float InStrength, float InDefence)
{
	// Monster 의 이름을 설정
    Name = InName;

	// Actor 가 보유한 ASC 를 통해 AttributeSet 에 접근
    AttributeSet* MyStats = GetAbilityComponent()->GetAttributeSet();
    if (MyStats)
    {
		// 몬스터의 기본 능력치를 설정합니다.
        MyStats->Level = 1;
        MyStats->HP.BaseValue = InHP;
        MyStats->HP.CurrentValue = InHP;

        MyStats->Strength.BaseValue = InStrength;
        MyStats->Strength.CurrentValue = InStrength;

        MyStats->Defence.BaseValue = InDefence;
        MyStats->Defence.CurrentValue = InDefence;
    }
}
std::wstring Monster::RunAI(Actor* Target)
{
    if (GetAbilityComponent())
    {
        return GetAbilityComponent()->TryActivateAbility(0, Target);
    }
}