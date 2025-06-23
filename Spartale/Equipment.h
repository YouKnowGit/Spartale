// Equipment.h (GAS ���� ���)
#pragma once
#include <string>
#include "EquipSlotType.h"
#include "GameplayEffect.h"
#include "AttributeSet.h" // ���� ���ǰ� �ʿ��ϹǷ� include


class AttributeSet;
class Equipment {
private:
    std::wstring name;
    EquipSlotType slotType;
    GameplayEffect effect; // ��� ȿ�� (ex: �� +5)

public:
    Equipment(const std::wstring& name, EquipSlotType slotType, const GameplayEffect& effect)
        : name(name), slotType(slotType), effect(effect) {
    }

    const std::wstring& GetName() const { return name; }
    EquipSlotType GetSlotType() const { return slotType; }

    // ��� ȿ�� ����/����
    void ApplyEffect(class AttributeSet* attributes) const { effect.Apply(attributes); }
    void RemoveEffect(class AttributeSet* attributes) const { effect.Remove(attributes); }
};
