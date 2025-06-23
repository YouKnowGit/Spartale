#pragma once
#include <string>
#include "Global.h"
#include "AttributeSet.h"
enum class EAttributeOperation {
    Add,
    Subtract,
    Multiply,
    Divide
};
class GameplayEffect
{
private:
    
    EAttributeOperation Operation;
public:
    GameplayEffect();
    GameplayEffect(const std::wstring& attributeName, EAttributeOperation operation, float magnitude)
        : TargetAttributeName(attributeName), Operation(operation), Magnitude(magnitude)
    {
    }
    virtual ~GameplayEffect();

    // 이펙트의 이름
    std::wstring EffectName;

    // 이펙트 적용 방식 (즉시, 지속, 영구)
    EEffectApplication ApplicationType;

    // 이펙트가 적용될 속성의 이름 (예: "HP", "Strength")
    //std::string TargetAttributeName;
    std::wstring TargetAttributeName;
    // 지속 시간 (턴 단위)
    int Duration = 0;

    // 데미지 타입
    EDamageType DamageType;

    // 변경 수치 (데미지량, 회복량, 스탯 증가량 등)
    float Magnitude = 0.0f;

    // 이 이펙트를 대상의 AttributeSet에 실제로 적용하는 함수
    virtual void Apply(AttributeSet* TargetAttributeSet) const;
    virtual void Remove(AttributeSet* TargetAttributeSet) const;
    
    //virtual ~GameplayEffect();
    
};