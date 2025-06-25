#pragma once
#include "ItemData.h"

class Player;

class Item
{
public:
	Item(const ItemData* pData) : m_pItemData(pData) {}
	virtual ~Item() = default;

	// 아이템 사용 가능 여부를 판단하는 함수
	virtual bool CanUse(Player* targetPlayer) const { return true; }
	// 순수 가상 함수: targetPlayer에게 자신을 사용. 소모되었으면 true 반환
	virtual bool use(Player* targetPlayer, int sourceSlotIndex) = 0;

	const std::string& GetID() const { return m_pItemData->ID; }
	const std::wstring& GetName() const { return m_pItemData->Name; }

protected:
	const ItemData* m_pItemData;
};