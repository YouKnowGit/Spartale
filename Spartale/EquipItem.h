#pragma once
#ifndef EQUIPITEM_H
#define EQUIPITEM_H
#include "ItemBase.h"
#include "EquipSlotType.h"
#include "GameplayEffect.h"

class EquipItem : public ItemBase {
private:
    
    EquipSlotType SlotType;
    GameplayEffect Effect;

public:
    EquipItem(const std::wstring& name, EquipSlotType slotType, const GameplayEffect& effect)
        : ItemBase(name), SlotType(slotType), Effect(effect) {
    }
                                                                
    EquipSlotType GetSlotType() const { return SlotType; }

    void ApplyEffect(AttributeSet* attrSet) const { Effect.Apply(attrSet); }
    void RemoveEffect(AttributeSet* attrSet) const { Effect.Remove(attrSet); }

    void Use(Actor* user) override;  // ÀåÂø
    bool IsEquippable() const override { return true; }
}; 
#endif