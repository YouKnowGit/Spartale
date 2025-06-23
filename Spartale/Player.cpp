#include "Player.h"
#include "EquipItem.h"
#include <codecvt>
#include <locale>
//#include "ItemBase.h"

Player::Player() 
{
    Initialize();


    equipmentSlots.emplace(EquipSlotType::RightHand, EquipmentSlot(EquipSlotType::RightHand));
    equipmentSlots.emplace(EquipSlotType::LeftHand, EquipmentSlot(EquipSlotType::LeftHand));
    equipmentSlots.emplace(EquipSlotType::Chest, EquipmentSlot(EquipSlotType::Chest));
    equipmentSlots.emplace(EquipSlotType::Head, EquipmentSlot(EquipSlotType::Head));
    equipmentSlots.emplace(EquipSlotType::Boots, EquipmentSlot(EquipSlotType::Boots));
    equipmentSlots.emplace(EquipSlotType::Neck, EquipmentSlot(EquipSlotType::Neck));
}

void Player::Initialize() 
{
    std::wstring temp;
    std::wcout << L"플레이어이름을 입력해주세요: ";
    std::getline(std::wcin, temp);
    Name = std::wstring(temp.begin(), temp.end());

    AttributeSet* MyStats = GetAbilityComponent()->GetAttributeSet();
    if (MyStats)
    {
        MyStats->Level = 1;
        MyStats->HP.BaseValue = 150.f;
        MyStats->HP.CurrentValue = 150.f;
        MyStats->MP.BaseValue = 100.f;
        MyStats->MP.CurrentValue = 100.f;

        MyStats->Strength.BaseValue = 15.f;
        MyStats->Strength.CurrentValue = 15.f;
        MyStats->Defence.BaseValue = 10.f;
        MyStats->Defence.CurrentValue = 10.f;
    }
}
void Player::Update() {}
void Player::Render() {}

AttributeSet* Player::GetAttributeSet() {
    return AttributeComponent.GetAttributeSet();
}

void Player::EquipItems(ItemBase* item)
{
    if (!item) return;
   
    auto equipItem = static_cast<EquipItem*>(item);
    if (!equipItem) {
        std::wcout << L"장비할 수 없는 아이템입니다.\n";
        return;
    }

    EquipSlotType slot = equipItem->GetSlotType();
    equipmentSlots[slot].Equip(equipItem, GetAttributeSet());
}

void Player::Unequip(EquipSlotType slotType) {
    if (equipmentSlots.count(slotType)) {
        equipmentSlots[slotType].Unequip(GetAttributeSet());
    }
}

void Player::DisplayStats() const {
    AttributeComponent.GetAttributeSet()->Display();
}

void Player::DisplayEquipment() const {
    for (auto it = equipmentSlots.begin(); it != equipmentSlots.end(); ++it) {
        EquipSlotType slot = it->first;
        const EquipmentSlot& eqSlot = it->second;

        std::wcout << L"[";
        switch (slot) {
        case EquipSlotType::RightHand: std::wcout << L"오른손"; break;
        case EquipSlotType::LeftHand:  std::wcout << L"왼손"; break;
        case EquipSlotType::Chest:     std::wcout << L"가슴"; break;
        case EquipSlotType::Head:      std::wcout << L"머리"; break;
        case EquipSlotType::Boots:     std::wcout << L"신발"; break;
        case EquipSlotType::Neck:      std::wcout << L"목"; break;
        }
        std::wcout << L"] ";

        auto item = eqSlot.GetEquippedItem();
        if (item) {
            std::wcout << item->GetName() << std::endl;
        }
        else {
            std::wcout << L"비어 있음" << std::endl;
        }
    }
}