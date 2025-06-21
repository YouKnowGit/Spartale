#pragma once
#include <string>
#include "EquipSlotType.h"

// Equipment Ŭ������ ���� ���� (��� include ���� �����͸� ��� �����ϰ� �ϱ� ����)
class Equipment;


// ��� ���� Ŭ����: ĳ������ Ư�� ������ ��� �����ϰų� �����ϴ� ����� ���
class EquipmentSlot {
private:	
	// ���� ���Կ� ������ ��� (������ nullptr)
	Equipment* equippedItem;

protected:
	// �� ������ � ������ ����ϴ���
	EquipSlotType slotType;
public:
	// ���� Ÿ���� �����Ͽ� ����
public:
	EquipmentSlot(); // �⺻ ������ 
	EquipmentSlot(EquipSlotType slotType);


	// ��� ���Կ� ����
	bool equip(Equipment* item);

	// ������ ����
	void unequip();

	void display() const;

	// ���� ������ ��� ��ȯ
	Equipment* getEquippedItem() const;

	// ������ Ÿ�� (��� ��������) ��ȯ
	EquipSlotType getSlotType()const;


	

};