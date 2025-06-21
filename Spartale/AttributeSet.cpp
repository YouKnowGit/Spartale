#include "AttributeSet.h"
#include <fcntl.h>
#include <io.h>
#include "AbilitySystemComponent.h"
#include <iostream>
AttributeSet::AttributeSet(AbilitySystemComponent* OwnerComp)
	: OwningAbilitySystemComponent(OwnerComp) // 멤버 이니셜라이저를 사용해 OwningAbilitySystemComponent 초기화
{
}

void AttributeSet::PreAttributeChange(const FAttributeData& Attribute, float& NewValue)
{
}

void AttributeSet::PostAttributeChange(const FAttributeData& Attribute, float OldValue, float NewValue)
{
}

void AttributeSet::AdjustDependentAttributes()
{
}
void AttributeSet::Display() const
{
    std::wcout << L"\n===== 캐릭터 능력치 =====\n";
    std::wcout << L"힘 (STR): " << Strength.CurrentValue << L"\n";
    std::wcout << L"민첩 (AGI): " << Agility.CurrentValue << L"\n";
    std::wcout << L"지능 (INT): " << Intelligence.CurrentValue << L"\n";
    std::wcout << L"방어력 (DEF): " << Defence.CurrentValue << L"\n";
    std::wcout << L"마법 저항력 (MRES): " << MagicResistance.CurrentValue << L"\n";
    std::wcout << L"HP: " << HP.CurrentValue << L" / " << BaseHP.CurrentValue << L"\n";
    std::wcout << L"MP: " << MP.CurrentValue << L" / " << BaseMP.CurrentValue << L"\n";
    std::wcout << L"Gold: " << Gold.CurrentValue << L"\n";
    std::wcout << L"EXP: " << Experience.CurrentValue << L"\n";
    std::wcout << L"Level: " << Level << L"\n";
    std::wcout << L"치명타 확률: " << CriticalHitChance.CurrentValue * 100 << L"%\n";
    std::wcout << L"치명타 배율: " << CriticalHitDamageMultiplier.CurrentValue << L"x\n";
    std::wcout << L"=========================\n";
    
}