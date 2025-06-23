#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

AttributeSet::AttributeSet(AbilitySystemComponent* OwnerComp)
	: OwningAbilitySystemComponent(OwnerComp) // 멤버 이니셜라이저를 사용해 OwningAbilitySystemComponent 초기화
{
    // 각 속성의 이름과 메모리 주소를 맵에 등록
    AttributeMap["HP"] = &HP;
    AttributeMap["MP"] = &MP;
    AttributeMap["BaseHP"] = &BaseHP;
    AttributeMap["BaseMP"] = &BaseMP;
    AttributeMap["Strength"] = &Strength;
    AttributeMap["Agility"] = &Agility;
    AttributeMap["Intelligence"] = &Intelligence;
    AttributeMap["Defence"] = &Defence;
    AttributeMap["MagicResistance"] = &MagicResistance;
    AttributeMap["Gold"] = &Gold;
    AttributeMap["Experience"] = &Experience;
    AttributeMap["CriticalHitChance"] = &CriticalHitChance;
    AttributeMap["CriticalHitDamageMultiplier"] = &CriticalHitDamageMultiplier;

    // . . . ( 새로운 스탯 추가 시 여기에 추가 ) . . .
}

void AttributeSet::PreAttributeChange(const FAttributeData& Attribute, float& NewValue)
{
}

void AttributeSet::PostAttributeChange(const FAttributeData& Attribute, float OldValue, float NewValue)
{
    // 포인터 비교를 통해 어떤 속성이 변경되었는지 확인
    if (&Attribute == &HP)
    {
        // HP가 변경되었다면, 0과 최대 HP 사이의 값으로 보정합니다.
        HP.CurrentValue = std::max(0.0f, HP.CurrentValue);
        HP.CurrentValue = std::min(HP.BaseValue, HP.CurrentValue); // 최대 체력을 넘지 않도록
    }
    else if (&Attribute == &MP)
    {
        // MP가 변경되었다면, 0과 최대 MP 사이의 값으로 보정합니다.
        MP.CurrentValue = std::max(0.0f, MP.CurrentValue);
        MP.CurrentValue = std::min(MP.BaseValue, MP.CurrentValue); // 최대 마나를 넘지 않도록
    }
}

void AttributeSet::AdjustDependentAttributes()
{
}