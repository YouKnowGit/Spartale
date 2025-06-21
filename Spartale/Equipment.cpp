#include "Equipment.h"

// ��� ������
// �̸�, ���� ����, ���ʽ� �ɷ�ġ, ��� ������ �����Ͽ� ����
Equipment::Equipment(const std::string& name, EquipSlotType slotType, int bonusStr, int bonusDex, int bonusInt, EquipmentType type)
    : name(name), slotType(slotType), bonusStr(bonusStr), bonusDex(bonusDex), bonusInt(bonusInt), type(type) {
};

// ��� �̸� ��ȯ
const std::string& Equipment::getName() const {
    return name;
}

// ��� ���� ��ȯ (����/�� ��)
EquipmentType Equipment::getType() const {
    return type;
}

// �� ���ʽ� ��ȯ
int Equipment::getBonusStr() const {
    return bonusStr;
}

// ��ø ���ʽ� ��ȯ
int Equipment::getBonusDex() const {
    return bonusDex;
}

// ���� ���ʽ� ��ȯ
int Equipment::getBonusInt() const {
    return bonusInt;
}

// ������ ���� Ÿ�� ��ȯ
EquipSlotType Equipment::getEquipmentSlot() const {
    return slotType;
}
