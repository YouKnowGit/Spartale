#include "Core/Actor.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"

// Actor 가 보유한 AbilitySystemComponent 를 생성하여 소유
Actor::Actor()
{
    this->AbilityComponent = std::make_unique<AbilitySystemComponent>(this);

    // 위치 정보 초기화 (X, Y)
    CurrentLocation = { 0, 0 };
}
void Actor::SetPosition(int x, int y)
{
    CurrentLocation.X = x;
    CurrentLocation.Y = y;
}

int Actor::GetX() const
{
    return CurrentLocation.X;
}

int Actor::GetY() const
{
    return CurrentLocation.Y;
}