#pragma once
#include "Item.h"

class EquipmentItem : public Item
{
public:
	EquipmentItem(const ItemData* pData) : Item(pData) {}
	virtual bool use(Player* targetPlayer, int sourcSlotIndex) override;

	// 장비 아이템의 고유 정보 Getter
	EEquipmentType GetEquipmentType() const { return m_pItemData->EquipmentData.Type; }
	const std::map<EStatType, float>& GetStatBonuses() const { return m_pItemData->EquipmentData.StatBonuses; }
};