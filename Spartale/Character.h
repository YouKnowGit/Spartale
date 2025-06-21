#pragma once
#include <string>
#include <map>
#include "EquipSlotType.h"
#include "EquipmentSlot.h"
using namespace std;

struct CharacterStats {
	int strength = 0;
	int dexterity = 0;
	int intelligence = 0;

};

// ĳ���� Ŭ����: ���, ����, ���� ���� ����
class Character
{
private:
	std::map<EquipSlotType, EquipmentSlot> equipmentSlots;
	std::string name;
	int level;
	int health;
	int experience;
	int expToLevelUp;

	CharacterStats stats;

public:
	
	//Character();
	Character(const std::string& name, int lelvel);
	

	void displayStatus() const; // ĳ���� ���� ���
	void levelUp();             // ���� ��
	void gainExperience(int amount); // ����ġ ȹ�� �� �ڵ� ������

	void setStats(int str, int dex, int intel); // ���� ����
	const CharacterStats& getstats() const;     // ���� ��ȯ

	void equipItem(Equipment* item); // ��� ����
	void displayEquipment() const;   // ������ ��� ��� ���
};