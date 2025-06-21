#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

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