#pragma once
#include "GameplayAbility.h"

// '독안개' 어빌리티 클래스를 정의
class AB_PoisonCloud : public GameplayAbility
{
public:
    AB_PoisonCloud();
    virtual ~AB_PoisonCloud() = default;

    // GameplayAbility의 순수 가상 함수 재정의
    virtual std::wstring ActivateAbility(AbilitySystemComponent* SourceASC, Actor* Target) override;
    virtual void SetupEffects() override {}

private:
    // 이 스킬의 데미지 타입
    EDamageType MyDamageType;

    // 이 스킬의 세부 설정값
    int PoisonDuration;       // 독 지속시간 (턴)
};