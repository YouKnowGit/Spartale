// Equipment.h (GAS 구조 기반)
#pragma once
#include <string>
#include "EquipSlotType.h"
#include "GameplayEffect.h"
#include "AttributeSet.h" // 실제 정의가 필요하므로 include


class AttributeSet;
class Equipment {
private:
    std::wstring name;
    EquipSlotType slotType;
    GameplayEffect effect; // 장비 효과 (ex: 힘 +5)

public:
    Equipment(const std::wstring& name, EquipSlotType slotType, const GameplayEffect& effect)
        : name(name), slotType(slotType), effect(effect) {
    }

    const std::wstring& GetName() const { return name; }
    EquipSlotType GetSlotType() const { return slotType; }

    // 장비 효과 적용/제거
    void ApplyEffect(class AttributeSet* attributes) const { effect.Apply(attributes); }
    void RemoveEffect(class AttributeSet* attributes) const { effect.Remove(attributes); }
};
