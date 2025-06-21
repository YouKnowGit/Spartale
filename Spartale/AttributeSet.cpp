#include "AttributeSet.h"
#include <fcntl.h>
#include <io.h>
#include "AbilitySystemComponent.h"
#include <iostream>
AttributeSet::AttributeSet(AbilitySystemComponent* OwnerComp)
	: OwningAbilitySystemComponent(OwnerComp) // ��� �̴ϼȶ������� ����� OwningAbilitySystemComponent �ʱ�ȭ
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
    std::wcout << L"\n===== ĳ���� �ɷ�ġ =====\n";
    std::wcout << L"�� (STR): " << Strength.CurrentValue << L"\n";
    std::wcout << L"��ø (AGI): " << Agility.CurrentValue << L"\n";
    std::wcout << L"���� (INT): " << Intelligence.CurrentValue << L"\n";
    std::wcout << L"���� (DEF): " << Defence.CurrentValue << L"\n";
    std::wcout << L"���� ���׷� (MRES): " << MagicResistance.CurrentValue << L"\n";
    std::wcout << L"HP: " << HP.CurrentValue << L" / " << BaseHP.CurrentValue << L"\n";
    std::wcout << L"MP: " << MP.CurrentValue << L" / " << BaseMP.CurrentValue << L"\n";
    std::wcout << L"Gold: " << Gold.CurrentValue << L"\n";
    std::wcout << L"EXP: " << Experience.CurrentValue << L"\n";
    std::wcout << L"Level: " << Level << L"\n";
    std::wcout << L"ġ��Ÿ Ȯ��: " << CriticalHitChance.CurrentValue * 100 << L"%\n";
    std::wcout << L"ġ��Ÿ ����: " << CriticalHitDamageMultiplier.CurrentValue << L"x\n";
    std::wcout << L"=========================\n";
    
}