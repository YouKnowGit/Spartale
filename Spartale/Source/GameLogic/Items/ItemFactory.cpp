#include "ItemFactory.h"
#include "GameLogic/DataManager.h"
#include "ConsumableItem.h"
#include "EquipmentItem.h"

std::unique_ptr<Item> ItemFactory::CreateItem(const std::string& itemID)
{
	const ItemData* data = DataManager::GetInstance().GetItemData(itemID);
	
	if (!data)
	{
		return nullptr; // 아이템 정보가 없으면 nullptr 반환
	}

	// ItemData의 타입에 따라 다른 클래스의 객체를 생성하여 반환
	switch (data->Type)
	{
		case EItemType::Consumable:
			return std::make_unique<ConsumableItem>(data);
		case EItemType::Equipment:
			return std::make_unique<EquipmentItem>(data);
		default:
			return nullptr; // 지원하지 않는 아이템 타입
	}

	return nullptr;
}