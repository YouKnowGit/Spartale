#pragma once
#include "Source/Core/Actor.h"
#include "Framework/Inventory/InventoryComponent.h"
#include <string>
#include <memory>
#include <iostream>
#include <fstream>

enum class Direction { UP, DOWN, LEFT, RIGHT };

class EquipmentItem; // 전방 선언

class Player : public Actor
{
public:
    Player(std::wstring name);
    virtual ~Player() = default;

    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Render() override;

    void SetDirection(Direction dir);
    Direction GetDirection() const;

	InventoryComponent* GetInventory() { return &m_inventory; }

    int GetMapID() { return mapId; }
    void SetMapID(int id) { mapId = id; }
    void Save(std::ofstream& file) const;
    void Load(std::ifstream& file);

    void Heal(float amount);
	bool Equip(int slotIndex);
    bool Unequip(int slotIndex);
	bool IsSlotEquipped(int slotIndex) const;

	int GetEquippedWeaponSlot() const { return m_equippedWeaponSlot; }
	int GetEquippedArmorSlot() const { return m_equippedArmorSlot; }
    int GetEquippedAccessorySlot() const { return m_equippedAccessorySlot; }

private:
    int mapId = 0;

    Direction m_direction;

	InventoryComponent m_inventory; // 플레이어의 인벤토리 컴포넌트

    int m_equippedWeaponSlot = -1;
    int m_equippedArmorSlot = -1;
    int m_equippedAccessorySlot = -1;

    void AddStatModifiers(const std::map<EStatType, float>& bonuses);
    void RemoveStatModifiers(const std::map<EStatType, float>& bonuses);
};