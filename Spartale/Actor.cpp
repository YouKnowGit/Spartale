#include "Actor.h"
#include "AbilitySystemComponent.h"

// 持失切: 切重税 AbilitySystemComponent研 持失馬食 社政
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