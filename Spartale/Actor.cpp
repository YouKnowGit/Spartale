#include "Actor.h"
#include "AbilitySystemComponent.h"

// ������: �ڽ��� AbilitySystemComponent�� �����Ͽ� ����
Actor::Actor()
{
    this->AbilityComponent = std::make_unique<AbilitySystemComponent>(this);
}