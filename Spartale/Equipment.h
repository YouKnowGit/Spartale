#pragma once
#include <string>
#include "EquipSlotType.h"

enum class EquipmentType
{
	Weapon,     // 무기
	Armor,      // 갑옷
	Helmet,     // 투구
	Boots,      // 부츠
	Accessory   // 장신구 (예: 반지, 목걸이)
};


// 기본 장비 클래스
// 캐릭터가 착용할 수 있는 모든 장비의 공통 속성을 정의한다
class Equipment
{
protected:
	std::string name;          // 장비 이름 (예: 철검, 가죽갑옷)
	EquipmentType type;        // 장비 분류 (무기/방어구 등)
	int bonusStr;              // 힘 보너스
	int bonusDex;              // 민첩 보너스
	int bonusInt;              // 지능 보너스

	EquipSlotType slotType;    // 장착될 슬롯 위치 (오른손, 머리 등)

public:
	// 생성자: 모든 값을 지정해야 생성 가능
	Equipment(const std::string& name, EquipSlotType slotType, int bonusStr, int bonusDex, int bonusInt, EquipmentType type);

	//Equipment(); 
	// 가상 소멸자: 파생 클래스 확장을 고려
	virtual ~Equipment() = default;
	// 장비 이름 반환
	const std::string& getName()const;
	// 장비 종류 반환 (무기/방어구 등)
	EquipmentType getType() const;
	//EquipSlotType getEquipSlot() const;
	// 장비가 착용될 슬롯 반환 (오른손/가슴/머리 등)
	EquipSlotType getEquipmentSlot() const;
	// 보정 능력치 반환
	int getBonusStr() const;
	int getBonusDex() const;
	int getBonusInt() const;


};