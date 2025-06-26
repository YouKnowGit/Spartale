#define NOMINMAX // std:: 매크로 충돌 방지 [ 사운드 파트의 매크로랑 겹침]

#include "InventoryComponent.h"
#include "GameLogic/Items/ItemData.h"
#include "GameLogic/Items/ItemFactory.h"
#include "GameLogic/Items/Item.h"
#include "GameLogic/DataManager.h"
#include "GameLogic/Units/Player.h"
#include <iostream> 
#include <stdexcept>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

// 생성자: 인벤토리의 칸(capacity) 수를 받아 초기화합니다.
InventoryComponent::InventoryComponent(int capacity)
    : m_capacity(capacity)
{
    // m_slots 벡터의 크기를 capacity 만큼 미리 할당하고, 빈 슬롯으로 초기화합니다.
    m_slots.resize(capacity);
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
        PlaySound(NULL, NULL, 0); // 실패 사운드 추가
        PlaySound(rejectsound, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOSTOP);
        std::cerr << "Inventory is full. Could not add " << quantityToAdd << " " << itemID << std::endl;
        Sleep(400);
    }

    // 최종적으로 추가하지 못한 아이템의 개수를 반환
    return quantityToAdd;
}

// 특정 슬롯의 아이템을 제거하는 함수
void InventoryComponent::RemoveItem(int slotIndex, int quantity)
{
    // 1. 유효성 검사
    if (slotIndex < 0 || slotIndex >= m_capacity)
    {
        return;
    }

    InventorySlot& slot = m_slots[slotIndex];

    if (slot.Quantity <= 0)
    {
        return;
    }

    // 2. 수량을 지정된 개수만큼 줄인다.
    slot.Quantity -= quantity;

    // 3. 만약 수량이 0 이하가 되었다면, 슬롯을 완전히 비움
    if (slot.Quantity <= 0)
    {
        slot.ItemID.clear();
        slot.pItemData = nullptr;
        slot.Quantity = 0;

        Compact();
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

// 특정 인덱스의 슬롯 정보를 수정 가능한 형태로 반환하는 함수
InventorySlot* InventoryComponent::GetSlotAtIndex_Mutable(int index)
{
    // 인덱스가 유효한 범위 내에 있는지 확인합니다.
    if (index >= 0 && index < m_capacity)
    {
        // m_slots는 멤버 변수이므로, 그 주소를 반환합니다.
        return &m_slots[index];
    }

    // 유효하지 않은 인덱스일 경우 nullptr을 반환합니다.
    return nullptr;
}

void InventoryComponent::UseItem(int slotIndex, Player* owner)
{
	const InventorySlot* slot = GetSlotAtIndex(slotIndex);
    if (!slot || slot->Quantity <= 0 || !owner) return;

	auto itemObject = ItemFactory::CreateItem(slot->ItemID);
    if (!itemObject) return;

    if (itemObject->CanUse(owner)) 
    {
        bool bWasConsumed = itemObject->use(owner, slotIndex);
        if (bWasConsumed)
        {
            RemoveItem(slotIndex, 1); // 사용 후 아이템을 1개 제거합니다.
        }
    }
    else
    {
        PlaySound(NULL, NULL, 0); // 사용 불가 효과음 추가
        PlaySound(rejectsound, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOSTOP);
    }
}

// 인벤토리의 아이템들을 정리하는 함수
void InventoryComponent::Compact()
{
    auto firstEmptySlot = std::stable_partition(m_slots.begin(), m_slots.end(),
        [](const InventorySlot& slot) {
            return slot.Quantity > 0; // 아이템이 있으면 true
        }
    );

    for (auto it = firstEmptySlot; it != m_slots.end(); ++it)
    {
        it->ItemID.clear();
        it->pItemData = nullptr;
        it->Quantity = 0;
        it->bIsEquipped = false;
    }
}