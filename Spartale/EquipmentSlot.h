#pragma once
#include <string>
#include "EquipSlotType.h"

// Equipment 클래스를 전방 선언 (헤더 include 없이 포인터만 사용 가능하게 하기 위함)
class Equipment;


// 장비 슬롯 클래스: 캐릭터의 특정 부위에 장비를 장착하거나 해제하는 기능을 담당
class EquipmentSlot {
private:	
	// 현재 슬롯에 장착된 장비 (없으면 nullptr)
	Equipment* equippedItem;

protected:
	// 이 슬롯이 어떤 부위를 담당하는지
	EquipSlotType slotType;
public:
	// 슬롯 타입을 지정하여 생성
public:
	EquipmentSlot(); // 기본 생성자 
	EquipmentSlot(EquipSlotType slotType);


	// 장비를 슬롯에 장착
	bool equip(Equipment* item);

	// 슬롯을 비우기
	void unequip();

	void display() const;

	// 현재 장착된 장비를 반환
	Equipment* getEquippedItem() const;

	// 슬롯의 타입 (어느 부위인지) 반환
	EquipSlotType getSlotType()const;


	

};