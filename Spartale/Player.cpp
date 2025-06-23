#include "Player.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include <string> 
#include <iostream>
#include <map>
#include "EquipSlotType.h"

Player::Player()
{
	// Player ��ü ���� �� �ٷ� �ʱ�ȭ �Լ� ȣ��
    Initialize();
}

void Player::Initialize()
{
    std::string temp;
    std::cout << "�÷��̾� �̸��� �Է��ϼ���: ";
    std::getline(std::cin, temp);
    Name = std::wstring(temp.begin(), temp.end());
    

    // ASC ���� AttributeSet�� ����
    AttributeSet* MyStats = GetAbilityComponent()->GetAttributeSet();
    if (MyStats)
    {
        // �÷��̾� �ʱ� �ɷ�ġ ����
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

void Player::Update()
{
    // TODO: �� �����Ӹ��� �÷��̾ ó���ؾ� �� ���� (��: �Է� ó��)
}

void Player::Render()
{
    // TODO: �÷��̾��� ������ ȭ�鿡 �׸��� ����
}
void Player::Equip(Equipment* item)
{
    if (!item)
    {
        std::cout << "����� �������� �����ϴ�.\n";
        return;
    }


    EquipSlotType slotType = item->GetSlotType();
    AttributeSet* attributes = GetAbilityComponent()->GetAttributeSet();

    // �̹� ������ �����ϴ� ���
    auto it = equipmentSlots.find(slotType);
    if (it != equipmentSlots.end())
    {
        std::wcout << L"���� ��� ��ü�մϴ�: ";
        if (it->second.GetEquippedItem())
            std::wcout << it->second.GetEquippedItem()->GetName() << L" �� ";
        else
            std::wcout << L"(����) �� ";

        std::wcout << item->GetName() << L"\n";

        it->second.Equip(item, attributes);
    }
    else
    {
        EquipmentSlot newSlot(slotType);
        newSlot.Equip(item, attributes);
        equipmentSlots[slotType] = newSlot;

        std::wcout << item->GetName() << L" ��(��) �� ���Կ� �����߽��ϴ�.\n";
    }
    
}
void Player::DisplayStats() const
{
   
    if (AbilityComponent)
    {
        AbilityComponent->GetAttributeSet()->Display();
    }
    
}

void Player::DisplayEquipment() const {
    std::wcout << L"\n[��� ���� ����]\n";

    // ������ ���� ����
    std::vector<EquipSlotType> slotOrder = {
        EquipSlotType::Head,
        EquipSlotType::Chest,
        EquipSlotType::LeftHand,
        EquipSlotType::RightHand,
        EquipSlotType::boots,
        EquipSlotType::Neck
    };

    for (EquipSlotType slotType : slotOrder) {
        std::wstring slotName;

        // ���� �̸� ��ȯ
        switch (slotType) {
        case EquipSlotType::Head: slotName = L"�Ӹ�"; break;
        case EquipSlotType::Chest: slotName = L"����"; break;
        case EquipSlotType::LeftHand: slotName = L"�޼�"; break;
        case EquipSlotType::RightHand: slotName = L"������"; break;
        case EquipSlotType::boots: slotName = L"�Ź�"; break;
        case EquipSlotType::Neck: slotName = L"��"; break;
        default: slotName = L"�� �� ����"; break;
        }

        auto it = equipmentSlots.find(slotType);
        if (it != equipmentSlots.end() && it->second.GetEquippedItem()) {
            std::wcout << L"- " << slotName << L" ����: "
                << it->second.GetEquippedItem()->GetName() << L"\n";
        }
        else {
            std::wcout << L"- " << slotName << L" ����: �������\n";
        }
    }
}