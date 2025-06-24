#include "InventoryComponent.h"
#include "ItemData.h"
#include "GameLogic/DataManager.h"
#include <iostream> 
#include <stdexcept>

// 생성자: 인벤토리의 칸(capacity) 수를 받아 초기화합니다.
InventoryComponent::InventoryComponent(int capacity)
    : m_capacity(capacity)
{
    // m_slots 벡터의 크기를 capacity 만큼 미리 할당하고, 빈 슬롯으로 초기화합니다.
    m_slots.resize(capacity);
    std::cout << "InventoryComponent created with " << capacity << " slots." << std::endl;
}

// 소멸자: 현재는 특별히 동적으로 할당한 메모리가 없으므로 비워둡니다.
InventoryComponent::~InventoryComponent()
{
    std::cout << "InventoryComponent destroyed." << std::endl;
}

// 아이템을 인벤토리에 추가하는 함수
int InventoryComponent::AddItem(const std::string& itemID, int quantity)
{
    const ItemData* itemData = DataManager::GetInstance().GetItemData(itemID);

    if (!itemData)
    {
        std::cerr << "Error: Invalid item ID '" << itemID << "'" << std::endl;
        return quantity; // 아이템 정보가 없으므로 요청된 개수 전체를 추가 실패로 반환
    }

    int quantityToAdd = quantity;

    // --- 1단계: 기존 스택 채우기 ---
    // 겹치기가 가능한 아이템일 경우에만 이 로직 실행
    if (itemData->bIsStackable)
    {
        for (InventorySlot& slot : m_slots)
        {
            if (slot.ItemID == itemID && slot.Quantity < itemData->MaxStackSize)
            {
                int spaceAvailable = itemData->MaxStackSize - slot.Quantity;
                int amountToTransfer = std::min(quantityToAdd, spaceAvailable);

                slot.Quantity += amountToTransfer;
                quantityToAdd -= amountToTransfer;

                if (quantityToAdd <= 0) {
                    return 0; // 모든 아이템을 성공적으로 추가했으므로 0을 반환
                }
            }
        }
    }

    // --- 2단계: 빈 슬롯에 새로 추가하기 ---
    // 아직 추가할 아이템이 남았을 경우
    if (quantityToAdd > 0)
    {
        for (InventorySlot& slot : m_slots)
        {
            if (slot.Quantity == 0) // 비어있는 슬롯 발견
            {
                slot.ItemID = itemID;
                slot.pItemData = itemData; // <<-- 중요: ItemData 포인터를 슬롯에 캐싱!

                int amountToTransfer = std::min(quantityToAdd, itemData->MaxStackSize);
                slot.Quantity = amountToTransfer;
                quantityToAdd -= amountToTransfer;

                if (quantityToAdd <= 0) {
                    return 0; // 모든 아이템을 성공적으로 추가했으므로 0을 반환
                }
            }
        }
    }

    // 모든 슬롯을 다 확인했는데도 아이템이 남았다면, 인벤토리가 가득 찬 것
    if (quantityToAdd > 0)
    {
        std::cerr << "Inventory is full. Could not add " << quantityToAdd << " " << itemID << std::endl;
    }

    // 최종적으로 추가하지 못한 아이템의 개수를 반환
    return quantityToAdd;
}

// 특정 슬롯의 아이템을 제거하는 함수
void InventoryComponent::RemoveItem(int slotIndex, int quantity)
{
    // 1. 유효성 검사: 인덱스가 인벤토리 범위를 벗어나는지 확인합니다.
    if (slotIndex < 0 || slotIndex >= m_capacity)
    {
        std::cerr << "Error: Invalid slot index " << slotIndex << std::endl;
        return;
    }

    // 수정할 슬롯에 대한 참조를 가져옵니다.
    InventorySlot& slot = m_slots[slotIndex];

    // 2. 유효성 검사: 해당 슬롯이 비어있는지 확인합니다.
    if (slot.Quantity <= 0)
    {
        std::cout << "Slot " << slotIndex << " is already empty." << std::endl;
        return;
    }

    std::cout << "Removing " << quantity << " " << slot.ItemID << " from slot " << slotIndex
        << " (Current: " << slot.Quantity << ")" << std::endl;

    // 3. 수량 감소: 슬롯의 수량을 지정된 개수만큼 줄입니다.
    slot.Quantity -= quantity;

    // 4. 슬롯 비우기: 만약 수량이 0 이하가 되었다면, 슬롯을 완전히 비웁니다.
    if (slot.Quantity <= 0)
    {
        std::cout << "Slot " << slotIndex << " is now empty." << std::endl;
        slot.ItemID.clear(); // string을 비웁니다.
        slot.Quantity = 0;   // 수량을 0으로 확실하게 설정합니다.
    }
    else
    {
        std::cout << "Slot " << slotIndex << " now has " << slot.Quantity << " items." << std::endl;
    }
}

// 특정 인덱스의 슬롯 정보를 반환하는 함수
const InventorySlot* InventoryComponent::GetSlotAtIndex(int index) const
{
    // 인덱스가 유효한 범위 내에 있는지 확인합니다.
    if (index >= 0 && index < m_capacity)
    {
        return &m_slots[index];
    }

    // 유효하지 않은 인덱스일 경우 nullptr을 반환합니다.
    return nullptr;
}