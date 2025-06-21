#include "Character.h"
#include <iostream>
#include "Equipment.h"

std::string getEquipSlotName(EquipSlotType type) {
	switch (type) {
	case EquipSlotType::RightHand:  return "오른손";
	case EquipSlotType::LeftHand:   return "왼손";
	case EquipSlotType::Chest:      return "가슴";
	case EquipSlotType::Head:       return "머리";
	case EquipSlotType::LeftFoot:   return "왼발";
	case EquipSlotType::RightFoot:  return "오른발";
	case EquipSlotType::Neck:       return "목";
	default:                        return "알 수 없음";
	}
}
// 생성자: 기본 능력치 및 장비 슬롯 초기화
Character::Character(const std::string& name, int level)
	: name(name), level(level), health(100),experience(0), expToLevelUp(100)
{
	// 각 장비 부위별 슬롯 초기화
	equipmentSlots[EquipSlotType::RightHand] = EquipmentSlot(EquipSlotType::RightHand);
	equipmentSlots[EquipSlotType::LeftHand] = EquipmentSlot(EquipSlotType::LeftHand);
	equipmentSlots[EquipSlotType::Chest] = EquipmentSlot(EquipSlotType::Chest);
	equipmentSlots[EquipSlotType::Head] = EquipmentSlot(EquipSlotType::Head);
	equipmentSlots[EquipSlotType::LeftFoot] = EquipmentSlot(EquipSlotType::LeftFoot);
	equipmentSlots[EquipSlotType::RightFoot] = EquipmentSlot(EquipSlotType::RightFoot);
	equipmentSlots[EquipSlotType::Neck] = EquipmentSlot(EquipSlotType::Neck);
}

// 캐릭터 스탯 설정
void Character::setStats(int str, int dex, int intel)
{
	stats.strength = str;
	stats.dexterity = dex;
	stats.intelligence = intel;
}

// 현재 스탯 정보 반환
const CharacterStats& Character::getstats() const {

	return stats;
}
// 캐릭터 상태 출력
void Character::displayStatus()const 
{
	std::cout << "이름: " << name << "\n";
	std::cout << "레벨: " << level << "\n";
	std::cout << "힘: " << stats.strength << "\n";
	std::cout << "민첩: " << stats.dexterity << "\n";
	std::cout << "지능: " << stats.intelligence << "\n";
		
		

}

// 장비 착용 (슬롯 타입에 맞게 자동 장착)
void Character::equipItem(Equipment* item)
{
	if (!item) return;

	EquipSlotType slotType = item->getEquipmentSlot();
	equipmentSlots[slotType].equip(item);


}
// 레벨 업 시 체력 증가 및 경험치 초기화
void Character::levelUp()
{
	level++;
	health += 20;
	experience -= expToLevelUp;
	
}


// 경험치 획득 및 레벨업 체크
void Character::gainExperience(int exp) 
{
	experience += exp;
	while (experience >= expToLevelUp) 
	{
		levelUp();
	}
}

// 장비 상태 출력
void Character::displayEquipment() const
{
	std::cout << "=== 장비 목록 ===\n";
	for (const auto& pair : equipmentSlots)
	{
		std::cout << "- 슬롯: " << getEquipSlotName(pair.first) << "\n";
		pair.second.display();
		
	}
}


