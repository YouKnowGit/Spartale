#include "Monster.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"

Monster::Monster(const std::wstring& InName, float InHP, float InStrength, float InDefence)
{
    // �����ڿ��� ���� �̸����� ����
    Name = InName;

    // ASC�� ���� AttributeSet�� ����
    AttributeSet* MyStats = GetAbilityComponent()->GetAttributeSet();
    if (MyStats)
    {
        // �����ڿ��� ���� ������ ������ �ɷ�ġ�� ����
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