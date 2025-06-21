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

// 캐릭터 클래스: 장비, 스탯, 레벨 등을 관리
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
	

	void displayStatus() const; // 캐릭터 정보 출력
	void levelUp();             // 레벨 업
	void gainExperience(int amount); // 경험치 획득 및 자동 레벨업

	void setStats(int str, int dex, int intel); // 스탯 설정
	const CharacterStats& getstats() const;     // 스탯 반환

	void equipItem(Equipment* item); // 장비 착용
	void displayEquipment() const;   // 장착한 장비 목록 출력
};