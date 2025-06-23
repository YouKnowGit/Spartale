#pragma once
#include "EquipSlotType.h"
#include "EquipItem.h"
#include "AttributeSet.h"

class EquipmentSlot {
private:
    EquipSlotType slotType;
    EquipItem* EquippedItem;

public:
    EquipmentSlot() {}
    EquipmentSlot(EquipSlotType slotType)
        : slotType(slotType), EquippedItem(nullptr) {
    }

    EquipItem* GetEquippedItem() const {
        return EquippedItem;
    }

    EquipSlotType GetSlotType() const {
        return slotType;
    }

    void Equip(EquipItem* newItem, AttributeSet* attributes) {
        if (EquippedItem) {
            EquippedItem->RemoveEffect(attributes);
        }
        EquippedItem = newItem;
        if (EquippedItem) {
            EquippedItem->ApplyEffect(attributes);
        }
    }

    void Unequip(AttributeSet* attributes) {
        if (EquippedItem) {
            EquippedItem->RemoveEffect(attributes);
            EquippedItem = nullptr;
        }
    }
};