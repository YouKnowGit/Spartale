#include <iostream>
#include "EquipmentSlot.h"
#include "Equipment.h"

EquipmentSlot::EquipmentSlot() : slotType(EquipSlotType::RightHand) {} // �⺻�� ����
// ������: ��� ������ Ÿ���� �����ϰ�, �ʱ⿡�� ���� ������ ����
EquipmentSlot::EquipmentSlot(EquipSlotType slotType)
	: slotType(slotType), equippedItem(nullptr){ }

// ���Կ� ��� ����. nullptr�� ��� false ��ȯ
bool EquipmentSlot::equip(Equipment* item)
{
	if (item == nullptr) return false;

	equippedItem = item;

	return true;

}
// ������ ��� ��� ����
void EquipmentSlot::unequip()
{
	equippedItem = nullptr;

}
// ���� ������ ��� ��ȯ (������ nullptr)
Equipment* EquipmentSlot::getEquippedItem() const
{
	return equippedItem;
}

// �� ������ � �������� ��ȯ
EquipSlotType EquipmentSlot::getSlotType() const
{
	return slotType;
}

void EquipmentSlot::display() const {
	if (equippedItem)
		std::cout << equippedItem->getName() << "\n";
	else
		std::cout << "(�������)\n";
}
