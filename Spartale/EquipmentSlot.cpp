#include <iostream>
#include "EquipmentSlot.h"
#include "Equipment.h"

EquipmentSlot::EquipmentSlot() : slotType(EquipSlotType::RightHand) {} // 기본값 설정
// 생성자: 장비 슬롯의 타입을 설정하고, 초기에는 장착 아이템 없음
EquipmentSlot::EquipmentSlot(EquipSlotType slotType)
	: slotType(slotType), equippedItem(nullptr){ }

// 슬롯에 장비를 장착. nullptr인 경우 false 반환
bool EquipmentSlot::equip(Equipment* item)
{
	if (item == nullptr) return false;

	equippedItem = item;

	return true;

}
// 슬롯을 비워 장비를 해제
void EquipmentSlot::unequip()
{
	equippedItem = nullptr;

}
// 현재 장착된 장비 반환 (없으면 nullptr)
Equipment* EquipmentSlot::getEquippedItem() const
{
	return equippedItem;
}

// 이 슬롯이 어떤 부위인지 반환
EquipSlotType EquipmentSlot::getSlotType() const
{
	return slotType;
}

void EquipmentSlot::display() const {
	if (equippedItem)
		std::cout << equippedItem->getName() << "\n";
	else
		std::cout << "(비어있음)\n";
}
