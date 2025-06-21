#include "Actor.h"
#include "AbilitySystemComponent.h"

// 持失切: 切重税 AbilitySystemComponent研 持失馬食 社政
Actor::Actor()
{
    this->AbilityComponent = std::make_unique<AbilitySystemComponent>(this);
}