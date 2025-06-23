#pragma once

#include <string>
#include <memory>
#include "Core/Global.h"
#include "Framework/AbilitySystem/AttributeSet.h"

class Actor;

class GameplayEffect
{
public:
    GameplayEffect();
    virtual ~GameplayEffect();

    // 이펙트의 이름
    std::wstring EffectName;

    // 이펙트 적용 방식 (즉시, 지속, 영구)
    EEffectApplication ApplicationType;

    // 이펙트가 적용될 속성의 이름 (예: "HP", "Strength")
    std::string TargetAttributeName;

    EModifierOperation OperationType = EModifierOperation::Add; // 기본은 덧셈

    // 효과의 정반대 효과를 생성해서 반환하는 함수 (지속 효과를 원상 복구하기 위해 사용)
    std::unique_ptr<GameplayEffect> CreateInverseEffect() const;

    // 지속 시간 (턴 단위)
    int Duration = 0;

    // 데미지 타입
    EDamageType DamageType;

    // 변경 수치 (데미지량, 회복량, 스탯 증가량 등)
    float Magnitude = 0.0f;

    bool bExecuteOnTurn = false;

    // 효과 시전자
    Actor* SourceActor = nullptr;

    // 이 이펙트를 대상의 AttributeSet에 실제로 적용하는 함수
    virtual void Apply(AttributeSet* TargetAttributeSet) const;
    virtual void Remove(AttributeSet* TargetAttributeSet) const;
};