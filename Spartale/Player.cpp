#include "Player.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include <string> 

Player::Player()
{
	// Player 객체 생성 시 바로 초기화 함수 호출
    Initialize();
}

void Player::Initialize()
{
    std::string temp;
    std::cout << "플레이어 이름을 입력하세요: ";
    std::getline(std::cin, temp);
    Name = std::wstring(temp.begin(), temp.end());
    

    // ASC 통해 AttributeSet에 접근
    AttributeSet* MyStats = GetAbilityComponent()->GetAttributeSet();
    if (MyStats)
    {
        // 플레이어 초기 능력치 설정
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
    // TODO: 매 프레임마다 플레이어가 처리해야 할 로직 (예: 입력 처리)
}

void Player::Render()
{
    // TODO: 플레이어의 정보를 화면에 그리는 로직
}
void Player::Equip(Equipment* item)
{
    if (!item)
    {
        std::cout << "장비할 아이템이 없습니다.\n";
        return;
    }

    EquipSlotType slotType = item->GetSlotType();
    AttributeSet* attributes = GetAbilityComponent()->GetAttributeSet();

    // 이미 슬롯이 존재하는 경우
    auto it = equipmentSlots.find(slotType);
    if (it != equipmentSlots.end())
    {
        std::wcout << L"기존 장비를 교체합니다: ";
        if (it->second.GetEquippedItem())
            std::wcout << it->second.GetEquippedItem()->GetName() << L" → ";
        else
            std::wcout << L"(없음) → ";

        std::wcout << item->GetName() << L"\n";

        it->second.Equip(item, attributes);
    }
    else
    {
        EquipmentSlot newSlot(slotType);
        newSlot.Equip(item, attributes);
        equipmentSlots[slotType] = newSlot;

        std::wcout << item->GetName() << L" 을(를) 새 슬롯에 장착했습니다.\n";
    }
    
}
void Player::DisplayStats() const
{
   
    if (AbilityComponent)
    {
        AbilityComponent->GetAttributeSet()->Display();
    }
    
}
