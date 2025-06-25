#pragma once
#include "Item.h"

class ConsumableItem : public Item
{
	public:
	ConsumableItem(const ItemData* pData) : Item(pData) {}
	virtual bool CanUse(Player* targetPlayer) const override;
	virtual bool use(Player* targetPlayer, int sourceSlotIndex) override;
};
