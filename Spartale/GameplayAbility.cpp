#include "GameplayAbility.h"
#include "GameplayEffect.h"

GameplayAbility::GameplayAbility()
{
}


bool GameplayAbility::CanActivateAbility(AbilitySystemComponent* SourceASC) const
{
    return true;
}