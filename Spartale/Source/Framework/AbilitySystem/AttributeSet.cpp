#include "Framework/AbilitySystem/AttributeSet.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"

AttributeSet::AttributeSet(AbilitySystemComponent* OwnerComp)
	: OwningAbilitySystemComponent(OwnerComp) // 멤버 이니셜라이저를 사용해 OwningAbilitySystemComponent 초기화
{
    // 각 송성의 이름과 메모리 주소를 맵에 등록
    AttributeMap["HP"] = &HP;
    AttributeMap["MP"] = &MP;
    // Base 능력치 제거
    //AttributeMap["BaseHP"] = &BaseHP;
    //AttributeMap["BaseMP"] = &BaseMP;
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
		// HP 가 변경되었다면, 0 과 BaseHP 사이의 값으로 보정 (음수 방지 및 최대값 제한)
        HP.CurrentValue = std::max(0.0f, HP.CurrentValue);
        HP.CurrentValue = std::min(HP.BaseValue, HP.CurrentValue);
    }
    else if (&Attribute == &MP)
    {
		// MP 가 변경되엇다면, 0 과 BaseMP 사이의 값으로 보정 (음수 방지 및 최대값 제한)
        MP.CurrentValue = std::max(0.0f, MP.CurrentValue);
        MP.CurrentValue = std::min(MP.BaseValue, MP.CurrentValue);
    }
}

void AttributeSet::AdjustDependentAttributes()
{
    // --- 재계산에 사용할 계수 정의 --
    const float BaseHPConstant = 100.0f;       // 캐릭터의 순수 기본 최대 HP
    const float HPPerStrength = 4.0f;        // 힘 1포인트당 증가하는 최대 HP

    const float MPPerIntelligence = 6.0f;     // 지능 1포인트당 증가하는 최대 MP
    const float BaseMPConstant = 75.0f;       // 캐릭터의 순수 기본 최대 MP

    // --- 최대 HP 재계산 ---

    // 재계산 전의 최대 HP 값을 기억
    float oldMaxHP = HP.BaseValue;

    // 새로운 최대 HP를 계산: (기본 HP) + ((힘 - 초기 힘 15) * 힘당HP)
    HP.BaseValue = BaseHPConstant + ((this->Strength.CurrentValue - 15) * HPPerStrength);

    // 최대치가 증가했다면, 증가한 만큼 현재 HP도 회복
    float hpIncrease = HP.BaseValue - oldMaxHP;
    if (hpIncrease > 0)
    {
        HP.CurrentValue += hpIncrease;
    }
    // 현재 HP가 새로운 최대 HP를 넘지 않도록 보정
    HP.CurrentValue = std::min(HP.CurrentValue, HP.BaseValue);


    // --- 최대 MP 재계산 ---

    // 재계산 전의 최대 MP 값을 기억
    float oldMaxMP = MP.BaseValue;

    // 새로운 최대 MP를 계산: (기본 MP) + ((지능 - 초기 지능 10) * 지능당MP) 
    MP.BaseValue = BaseMPConstant + ((this->Intelligence.CurrentValue - 10) * MPPerIntelligence);


    // 최대치가 증가했다면, 증가한 만큼 현재 MP도 회복
    float mpIncrease = MP.BaseValue - oldMaxMP;
    if (mpIncrease > 0)
    {
        MP.CurrentValue += mpIncrease;
    }
    MP.CurrentValue = std::min(MP.CurrentValue, MP.BaseValue);
}