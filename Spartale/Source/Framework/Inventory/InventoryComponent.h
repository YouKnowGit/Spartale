#pragma once

#include <vector>
#include <string>
#include "GameLogic/Items/ItemData.h"

class Player; // 전방 선언

// 인벤토리의 한 '칸(슬롯)'을 나타내는 구조체
struct InventorySlot
{
    std::string ItemID;
    int Quantity = 0;
    const ItemData* pItemData = nullptr;
    bool bIsEquipped = false;
};

class InventoryComponent
{
public:
    InventoryComponent(int capacity);
    ~InventoryComponent();

    int AddItem(const std::string& itemID, int quantity);
    void RemoveItem(int slotIndex, int quantity);

    void UseItem(int slotIndex, Player* owner);

    const InventorySlot* GetSlotAtIndex(int index) const;
    
    InventorySlot* GetSlotAtIndex_Mutable(int index);
    
    int GetCapacity() const { return m_capacity; }

private:
    int m_capacity; // 인벤토리의 최대 칸 수
    std::vector<InventorySlot> m_slots;

    void Compact();

    const wchar_t* rejectsound = L"Sounds/UI/menu_reject.wav";
    const wchar_t* Warp = L"Sounds/warp.wav";
};