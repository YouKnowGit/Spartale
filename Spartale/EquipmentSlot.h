// EquipmentSlot.h (GAS ���� ���)
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

    // ����: ���� ��� ȿ�� ���� �� �� ��� ���� �� ȿ�� ����
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
