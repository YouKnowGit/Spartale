#pragma once
#include "Source/Core/Actor.h"
#include <string>
#include "Source/GameLogic/Items/EquipSlotType.h"
#include "GameLogic/Items/EquipmentSlot.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "GameLogic/Items/EquipItem.h"
#include <map>
#include <iostream>

enum class Direction { UP, DOWN, LEFT, RIGHT };

// Actor를 상속받아 Player 클래스를 정의합니다.
class Player : public Actor
{
private:
    std::map<EquipSlotType, EquipmentSlot> equipmentSlots;
    AbilitySystemComponent AttributeComponent;
public:
    
    Player(std::wstring name);
    virtual ~Player() = default;

    // Actor의 가상 함수들을 재정의(override)
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Render() override;

    void SetDirection(Direction dir);
    Direction GetDirection() const;

    void EquipItems(ItemBase* item);
    void Unequip(EquipSlotType slotType);
    void DisplayStats() const;
    void DisplayEquipment() const;

    AttributeSet* GetAttributeSet(); // 추가

private:
    Direction m_direction;
};