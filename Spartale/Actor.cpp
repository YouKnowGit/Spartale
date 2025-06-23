#include "Actor.h"
#include "AbilitySystemComponent.h"

// ������: �ڽ��� AbilitySystemComponent�� �����Ͽ� ����
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