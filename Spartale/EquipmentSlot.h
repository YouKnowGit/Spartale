// EquipmentSlot.h (GAS 구조 기반)
#pragma once
#include "Equipment.h"
#include "EquipSlotType.h"

class EquipmentSlot {
private:
    EquipSlotType slotType;
    Equipment* equippedItem = nullptr;

public:
    EquipmentSlot() : slotType(EquipSlotType::RightHand), equippedItem(nullptr) {}
    EquipmentSlot(EquipSlotType slotType)
        : slotType(slotType), equippedItem(nullptr) {
    }

    Equipment* GetEquippedItem() const { return equippedItem; }
    EquipSlotType GetSlotType() const { return slotType; }

    // 장착: 기존 장비 효과 제거 → 새 장비 장착 및 효과 적용
    void Equip(Equipment* newItem, AttributeSet* attributes) {
        if (equippedItem) {
            equippedItem->RemoveEffect(attributes);
        }
        equippedItem = newItem;
        if (equippedItem) {
            equippedItem->ApplyEffect(attributes);
        }
    }

    void Unequip(AttributeSet* attributes) {
        if (equippedItem) {
            equippedItem->RemoveEffect(attributes);
            equippedItem = nullptr;
        }
    }
};
