#pragma once
#include "Actor.h"
#include <iostream>
#include <map>
#include "EquipSlotType.h"
#include "EquipmentSlot.h"
#include "AbilitySystemComponent.h"
#include "EquipItem.h"

class Player : public Actor {
private:
    std::map<EquipSlotType, EquipmentSlot> equipmentSlots;
    AbilitySystemComponent AttributeComponent;

public:
    Player();
    virtual ~Player() = default;

    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Render() override;

    void EquipItems(ItemBase* item);
    void Unequip(EquipSlotType slotType);
    void DisplayStats() const;
    void DisplayEquipment() const;

    AttributeSet* GetAttributeSet(); // Ãß°¡
};