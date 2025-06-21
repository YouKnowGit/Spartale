#include "Monster.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"

Monster::Monster(const std::wstring& InName, float InHP, float InStrength, float InDefence)
{
    // 생성자에서 받은 이름으로 설정
    Name = InName;

    // ASC를 통해 AttributeSet에 접근
    AttributeSet* MyStats = GetAbilityComponent()->GetAttributeSet();
    if (MyStats)
    {
        // 생성자에서 받은 값으로 몬스터의 능력치를 설정
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