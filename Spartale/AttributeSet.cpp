#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

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