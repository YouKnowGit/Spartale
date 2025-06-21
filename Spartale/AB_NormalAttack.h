#pragma once
#include "GameplayAbility.h"

// '일반 공격' 어빌리티 클래스를 정의
class AB_NormalAttack : public GameplayAbility
{
public:
    AB_NormalAttack();
    virtual ~AB_NormalAttack() = default;

    // GameplayAbility의 순수 가상 함수를 재정의
    virtual std::wstring ActivateAbility(AbilitySystemComponent* SourceASC, Actor* Target) override;
    virtual void SetupEffects() override;

private:
    // 이 스킬의 데미지 타입
    EDamageType MyDamageType;
};