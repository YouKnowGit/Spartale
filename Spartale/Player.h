#pragma once
#include "Actor.h"
#include <iostream>
#include <map>
#include "Equipment.h"
#include "EquipSlotType.h"
#include "EquipmentSlot.h"
#include "AbilitySystemComponent.h"

class AbilitySystemComponent;
// Actor�� ��ӹ޾� Player Ŭ������ �����մϴ�.
class Player : public Actor
{

private:
    std::map<EquipSlotType, EquipmentSlot> equipmentSlots;
public:
    Player();
    virtual ~Player() = default;

    // Actor�� ���� �Լ����� ������(override)
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Render() override;
    void Equip(Equipment* item);
    void DisplayStats() const;
    void DisplayEquipment() const;
};

