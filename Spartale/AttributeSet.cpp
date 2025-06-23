#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

AttributeSet::AttributeSet(AbilitySystemComponent* OwnerComp)
	: OwningAbilitySystemComponent(OwnerComp) // ��� �̴ϼȶ������� ����� OwningAbilitySystemComponent �ʱ�ȭ
{
    // �� �Ӽ��� �̸��� �޸� �ּҸ� �ʿ� ���
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

    // . . . ( ���ο� ���� �߰� �� ���⿡ �߰� ) . . .
}

void AttributeSet::PreAttributeChange(const FAttributeData& Attribute, float& NewValue)
{
}

void AttributeSet::PostAttributeChange(const FAttributeData& Attribute, float OldValue, float NewValue)
{
    // ������ �񱳸� ���� � �Ӽ��� ����Ǿ����� Ȯ��
    if (&Attribute == &HP)
    {
        // HP�� ����Ǿ��ٸ�, 0�� �ִ� HP ������ ������ �����մϴ�.
        HP.CurrentValue = std::max(0.0f, HP.CurrentValue);
        HP.CurrentValue = std::min(HP.BaseValue, HP.CurrentValue); // �ִ� ü���� ���� �ʵ���
    }
    else if (&Attribute == &MP)
    {
        // MP�� ����Ǿ��ٸ�, 0�� �ִ� MP ������ ������ �����մϴ�.
        MP.CurrentValue = std::max(0.0f, MP.CurrentValue);
        MP.CurrentValue = std::min(MP.BaseValue, MP.CurrentValue); // �ִ� ������ ���� �ʵ���
    }
}

void AttributeSet::AdjustDependentAttributes()
{
}