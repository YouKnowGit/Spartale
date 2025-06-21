#include "Equipment.h"

// 장비 생성자
// 이름, 장착 슬롯, 보너스 능력치, 장비 종류를 지정하여 생성
Equipment::Equipment(const std::string& name, EquipSlotType slotType, int bonusStr, int bonusDex, int bonusInt, EquipmentType type)
    : name(name), slotType(slotType), bonusStr(bonusStr), bonusDex(bonusDex), bonusInt(bonusInt), type(type) {
};

// 장비 이름 반환
const std::string& Equipment::getName() const {
    return name;
}

// 장비 종류 반환 (무기/방어구 등)
EquipmentType Equipment::getType() const {
    return type;
}

// 힘 보너스 반환
int Equipment::getBonusStr() const {
    return bonusStr;
}

// 민첩 보너스 반환
int Equipment::getBonusDex() const {
    return bonusDex;
}

// 지능 보너스 반환
int Equipment::getBonusInt() const {
    return bonusInt;
}

// 장착될 슬롯 타입 반환
EquipSlotType Equipment::getEquipmentSlot() const {
    return slotType;
}
