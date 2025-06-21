#include "Character.h"
#include <iostream>
#include "Equipment.h"

std::string getEquipSlotName(EquipSlotType type) {
	switch (type) {
	case EquipSlotType::RightHand:  return "������";
	case EquipSlotType::LeftHand:   return "�޼�";
	case EquipSlotType::Chest:      return "����";
	case EquipSlotType::Head:       return "�Ӹ�";
	case EquipSlotType::LeftFoot:   return "�޹�";
	case EquipSlotType::RightFoot:  return "������";
	case EquipSlotType::Neck:       return "��";
	default:                        return "�� �� ����";
	}
}
// ������: �⺻ �ɷ�ġ �� ��� ���� �ʱ�ȭ
Character::Character(const std::string& name, int level)
	: name(name), level(level), health(100),experience(0), expToLevelUp(100)
{
	// �� ��� ������ ���� �ʱ�ȭ
	equipmentSlots[EquipSlotType::RightHand] = EquipmentSlot(EquipSlotType::RightHand);
	equipmentSlots[EquipSlotType::LeftHand] = EquipmentSlot(EquipSlotType::LeftHand);
	equipmentSlots[EquipSlotType::Chest] = EquipmentSlot(EquipSlotType::Chest);
	equipmentSlots[EquipSlotType::Head] = EquipmentSlot(EquipSlotType::Head);
	equipmentSlots[EquipSlotType::LeftFoot] = EquipmentSlot(EquipSlotType::LeftFoot);
	equipmentSlots[EquipSlotType::RightFoot] = EquipmentSlot(EquipSlotType::RightFoot);
	equipmentSlots[EquipSlotType::Neck] = EquipmentSlot(EquipSlotType::Neck);
}

// ĳ���� ���� ����
void Character::setStats(int str, int dex, int intel)
{
	stats.strength = str;
	stats.dexterity = dex;
	stats.intelligence = intel;
}

// ���� ���� ���� ��ȯ
const CharacterStats& Character::getstats() const {

	return stats;
}
// ĳ���� ���� ���
void Character::displayStatus()const 
{
	std::cout << "�̸�: " << name << "\n";
	std::cout << "����: " << level << "\n";
	std::cout << "��: " << stats.strength << "\n";
	std::cout << "��ø: " << stats.dexterity << "\n";
	std::cout << "����: " << stats.intelligence << "\n";
		
		

}

// ��� ���� (���� Ÿ�Կ� �°� �ڵ� ����)
void Character::equipItem(Equipment* item)
{
	if (!item) return;

	EquipSlotType slotType = item->getEquipmentSlot();
	equipmentSlots[slotType].equip(item);


}
// ���� �� �� ü�� ���� �� ����ġ �ʱ�ȭ
void Character::levelUp()
{
	level++;
	health += 20;
	experience -= expToLevelUp;
	
}


// ����ġ ȹ�� �� ������ üũ
void Character::gainExperience(int exp) 
{
	experience += exp;
	while (experience >= expToLevelUp) 
	{
		levelUp();
	}
}

// ��� ���� ���
void Character::displayEquipment() const
{
	std::cout << "=== ��� ��� ===\n";
	for (const auto& pair : equipmentSlots)
	{
		std::cout << "- ����: " << getEquipSlotName(pair.first) << "\n";
		pair.second.display();
		
	}
}


