#pragma once
#include "Framework/AbilitySystem/GameplayAbility.h"

class SkillData;

class GenericAbility : public GameplayAbility
{
public:
    GenericAbility() = default;
    virtual ~GenericAbility() = default;

    virtual std::wstring ActivateAbility(AbilitySystemComponent* SourceASC, Actor* Target) override;
    virtual void SetupEffects() override {}

    virtual void InitializeFromData(const SkillData* data) override;

private:
    // 이 스킬이 실행할 효과들의 데이터 목록
    const SkillData* m_SkillData = nullptr;
};