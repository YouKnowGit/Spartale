#pragma once
#include "Actor.h"
#include <iostream>
#include <map>
#include "Equipment.h"
#include "EquipSlotType.h"
#include "EquipmentSlot.h"
// Actor를 상속받아 Player 클래스를 정의합니다.
class Player : public Actor
{

private:
    std::map<EquipSlotType, EquipmentSlot> equipmentSlots;
public:
    Player();
    virtual ~Player() = default;

    // Actor의 가상 함수들을 재정의(override)
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Render() override;
    void Equip(Equipment* item);
    void DisplayStats() const;
};

