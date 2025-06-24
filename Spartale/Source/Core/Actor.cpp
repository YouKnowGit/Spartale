#include "Core/Actor.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"

// Actor 가 보유한 AbilitySystemComponent 를 생성하여 소유
Actor::Actor()
{
    this->AbilityComponent = std::make_unique<AbilitySystemComponent>(this);

    m_x = 0;
    m_y = 0;
}
void Actor::SetPosition(int x, int y)
{
    m_x = x;
    m_y = y;
}

int Actor::GetX() const
{
    return m_x;
}

int Actor::GetY() const
{
    return m_y;
}