#pragma once

#include <vector>
#include <string>
#include "ItemData.h" // 위에서 정의한 ItemData 구조체

// 인벤토리의 한 '칸(슬롯)'을 나타내는 구조체
struct InventorySlot
{
    std::string ItemID;
    int Quantity = 0;
    const ItemData* pItemData = nullptr;
};

class InventoryComponent
{
public:
    InventoryComponent(int capacity);
    ~InventoryComponent();

    int AddItem(const std::string& itemID, int quantity);
    void RemoveItem(int slotIndex, int quantity);

    const InventorySlot* GetSlotAtIndex(int index) const;
    int GetCapacity() const { return m_capacity; }

private:
    int m_capacity; // 인벤토리의 최대 칸 수
    std::vector<InventorySlot> m_slots;
};