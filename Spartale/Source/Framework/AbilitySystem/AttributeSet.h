#pragma once
#include <map>
#include <string>
#include "Core/Global.h"

class AbilitySystemComponent;

// 각 능력치(Attribute)의 현재 값과 기본 값을 저장하는 구조체
struct FAttributeData
{
    float BaseValue = 0.0f; // 기본 값(능력치 아이템이나 스킬에 의해 변경되지 않음)
    float CurrentValue = 0.0f;  // 현재 값(능력치 아이템이나 스킬에 의해 변경될 수 있음)

    // 생성자를 통해 하나의 값으로 2개의 멤버를 초기화할 수 있도록 설정
    FAttributeData(float value = 0.0f) : BaseValue(value), CurrentValue(value) {}
};
class AttributeSet
{
public:
    AttributeSet(AbilitySystemComponent* OwnerComp);
    virtual ~AttributeSet() = default;

    // 속성 이름으로 실제 데이터에 접근할 수 있게 해주는 맵
    // 속성 이름은 문자열로 저장되며, 해당 속성의 FAttributeData 포인터를 참조
    std::map<std::string, FAttributeData*> AttributeMap;

    AbilitySystemComponent* OwningAbilitySystemComponent;

    // --- 기본 스탯 (Primary Stats) ---
    FAttributeData Strength;          // 힘
    FAttributeData Agility;           // 민첩
    FAttributeData Intelligence;      // 지능
    FAttributeData Defence;           // 방어력
    FAttributeData MagicResistance;   // 마법 저항력

    // --- 자원 (Resources) ---
    FAttributeData HP;
    FAttributeData MP;

    // 골드와 경험치, 레벨
    FAttributeData Experience;
    int Gold;
    int Level = 1;

    FAttributeData CriticalHitChance;          // 치명타 확률 (예: 0.05 = 5%)
    FAttributeData CriticalHitDamageMultiplier; // 치명타 피해량 배율 (예: 1.5 = 150%)

    // --- 메타 데이터 (Meta Data) ---
    bool bIsDefending = false; // 현재 방어 상태인지 여부 (방어 시스템 제거 시 삭제)
    int AdditionalStatPoints = 0; // 분배 가능한 추가 스탯 포인트

    // 능력치가 변경되기 직전에 호출될 함수
    virtual void PreAttributeChange(const FAttributeData& Attribute, float& NewValue);

    // 능력치가 변경된 직후에 호출될 함수
    virtual void PostAttributeChange(const FAttributeData& Attribute, float OldValue, float NewValue);

    // 힘, 지능 스탯에 따라 MaxHP, MaxMP를 재계산하는 함수
    void AdjustDependentAttributes();
};