#pragma once
#include <string>
#include "EquipSlotType.h"

enum class EquipmentType
{
	Weapon,     // ����
	Armor,      // ����
	Helmet,     // ����
	Boots,      // ����
	Accessory   // ��ű� (��: ����, �����)
};


// �⺻ ��� Ŭ����
// ĳ���Ͱ� ������ �� �ִ� ��� ����� ���� �Ӽ��� �����Ѵ�
class Equipment
{
protected:
	std::string name;          // ��� �̸� (��: ö��, ���װ���)
	EquipmentType type;        // ��� �з� (����/�� ��)
	int bonusStr;              // �� ���ʽ�
	int bonusDex;              // ��ø ���ʽ�
	int bonusInt;              // ���� ���ʽ�

	EquipSlotType slotType;    // ������ ���� ��ġ (������, �Ӹ� ��)

public:
	// ������: ��� ���� �����ؾ� ���� ����
	Equipment(const std::string& name, EquipSlotType slotType, int bonusStr, int bonusDex, int bonusInt, EquipmentType type);

	//Equipment(); 
	// ���� �Ҹ���: �Ļ� Ŭ���� Ȯ���� ���
	virtual ~Equipment() = default;
	// ��� �̸� ��ȯ
	const std::string& getName()const;
	// ��� ���� ��ȯ (����/�� ��)
	EquipmentType getType() const;
	//EquipSlotType getEquipSlot() const;
	// ��� ����� ���� ��ȯ (������/����/�Ӹ� ��)
	EquipSlotType getEquipmentSlot() const;
	// ���� �ɷ�ġ ��ȯ
	int getBonusStr() const;
	int getBonusDex() const;
	int getBonusInt() const;


};