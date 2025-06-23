// Sword.h
#pragma once
#include "EquipItem.h"
#include "GameplayEffect.h"
//#include "Equipment.h"

class Sword : public EquipItem {
public:
    Sword()
        : EquipItem(
            L"°­Ã¶°Ë",
            EquipSlotType::RightHand,
            GameplayEffect(L"Strength", EAttributeOperation::Add, 5.0f))
    {
    }
};
