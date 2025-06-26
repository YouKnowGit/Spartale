#include "GameLogic/Units/Player.h"
#include "GameLogic/Skills/SkillFactory.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/AttributeSet.h"
#include "Framework/AbilitySystem/GameplayAbility.h"
#include "Utils/StringUtils.h"
#include "GameLogic/Items/EquipmentItem.h"
#include "Core/TypeConverter.h"
#include <sstream>
#include <Windows.h>


Player::Player(std::wstring name)
	: m_inventory(20) // 플레이어의 인벤토리 크기를 20으로 설정
{
    // Player 이름 초기화
    Name = name;

	// Player 초기화 함수 호출
    Initialize();
}

void Player::Initialize()
{
    // ASC가 보유한 AttributeSet 에 접근
    AttributeSet* MyStats = GetAbilityComponent()->GetAttributeSet();
    if (MyStats)
    {
		// Player 초기 Stats 설정
        MyStats->Level = 1;
        
        MyStats->Experience.CurrentValue = 0.f;
        MyStats->Experience.BaseValue = 100.f;

        MyStats->HP = FAttributeData(200.f);
        MyStats->MP = FAttributeData(75.f);

        MyStats->Strength = FAttributeData(30.f);
        MyStats->Agility = FAttributeData(15.f);
        MyStats->Intelligence = FAttributeData(30.f);

        MyStats->Defence = FAttributeData(15.f);
        MyStats->MagicResistance = FAttributeData(15.f);

        MyStats->CriticalHitChance = FAttributeData(5.f);
        MyStats->CriticalHitDamageMultiplier = FAttributeData(1.5f);
    }
    /*
    const std::vector<std::string> allSkillIDs = {
        "SK_NormalAttack"
        //"SK_PoisonCloud",
        //"SK_Fireball",
        //"SK_TripleSlash",
        //"SK_StrengthBuff",
        //"SK_Meditate",
        //"SK_Heal",
        //"SK_ArcaneBlast",
        //"SK_AbyssalCollapse",
        //"SK_Judgment"
    };
    for (const auto& id : allSkillIDs)
    {
        GetAbilityComponent()->GrantAbility(SkillFactory::CreateSkill(id));
    }
    */

    // Player 위치 및 방향 관련 데이터
    CurrentLocation.X = 9;
    CurrentLocation.Y = 5;
    m_direction = Direction::DOWN;
}

void Player::Update()
{
}

void Player::Render()
{
}

void Player::SetDirection(Direction dir)
{
    m_direction = dir;
}

Direction Player::GetDirection() const
{
    return m_direction;
}
void Player::Save(std::ofstream& file) const
{
    file << "[Player]" << '\n';
    file << "Name " << StringUtils::CovertToString(Name) << '\n';
    file << "MapID " << this->mapId << '\n';
    file << "PosX " << this->CurrentLocation.X << '\n';
    file << "PosY " << this->CurrentLocation.Y << '\n';
    file << "Direction " << static_cast<int>(m_direction) << '\n';

    if ( this->AbilityComponent && this->AbilityComponent->GetAttributeSet()) {
        this->AbilityComponent->GetAttributeSet()->Save(file);
        this->AbilityComponent->Save(file);
    }

    file << "[Items]\n";
    for (int i = 0; i < m_inventory.GetCapacity(); ++i)
    {
        const InventorySlot* slot = m_inventory.GetSlotAtIndex(i);
        if (slot && slot->Quantity > 0)
        {
            file << "Slot" << i << ','
                << slot->ItemID << ','
                << slot->Quantity << '\n';
        }
        else
        {
            file << "Slot" << i << ",EMPTY\n";
        }
    }

    file << "[EquippedItems]\n";
    file << "eWeapon " << m_equippedWeaponSlot << '\n';
    file << "eArmor " << m_equippedArmorSlot << '\n';
    file << "eAccessory " << m_equippedAccessorySlot << '\n';
}
void Player::Load(std::ifstream& file)
{
    file.clear();
    file.seekg(0, std::ios::beg);

    if (AbilityComponent && AbilityComponent->GetAttributeSet())
    {
        AbilityComponent->GetAttributeSet()->Load(file);
    }

    if (AbilityComponent)
    {
        AbilityComponent->Load(file);
    }

    file.clear();
    file.seekg(0, std::ios::beg);

    std::string line;
    bool in_player_section = false;
    bool in_items_section = false;
    bool in_equippeditem_section = false;
    while (std::getline(file, line))
    {
        
        if (line.empty()) continue;

        if (line[0] == '[') {
            in_player_section = false;
            in_items_section = false;
            in_equippeditem_section = false;

            if (line == "[Player]") {
                in_player_section = true;
                continue;
            }
            if (line == "[Items]") {
                in_items_section = true;
                continue;
            }
            if (line == "[EquippedItems]") {
                in_equippeditem_section = true;
                continue;
            }
        }


        if (in_player_section)
        {
            std::stringstream ss(line);
            std::string key;
            ss >> key;

            if (key == "Name") {
                std::string utf8Value;
                ss >> utf8Value;
                Name = StringUtils::ConvertToWstring(utf8Value);
            }
            else if (key == "MapID") ss >> this->mapId;
            else if (key == "PosX") ss >> this->CurrentLocation.X;
            else if (key == "PosY") ss >> this->CurrentLocation.Y;
            else if (key == "Direction") {
                int dirValue;
                ss >> dirValue;
                m_direction = static_cast<Direction>(dirValue);
            }
        }

        if (in_items_section)
        {
            std::istringstream iss(line);
            std::string slotLabel, itemID, quantityStr;

            if (std::getline(iss, slotLabel, ',') && std::getline(iss, itemID, ',') && std::getline(iss, quantityStr))
            {
                if (itemID == "EMPTY")  continue;
                int quantity = std::stoi(quantityStr);
                int slotIndex = -1;
                if (slotLabel.rfind("Slot", 0) == 0)    slotIndex = std::stoi(slotLabel.substr(4));
                if (slotIndex != -1)    m_inventory.AddItem(itemID, quantity);
            }
        }

        if (in_equippeditem_section)
        {
            std::stringstream ss(line);
            std::string key;
            ss >> key;
            
            if (key == "eWeapon")    ss >> this->m_equippedWeaponSlot;
            else if (key == "eArmor")    ss >> this->m_equippedArmorSlot;
            else if (key == "eAccessory")   ss >> this->m_equippedAccessorySlot;

            this->Equip(m_equippedWeaponSlot);
            this->Equip(m_equippedArmorSlot);
            this->Equip(m_equippedAccessorySlot);
        }
    }

    this->GetAbilityComponent()->GetAttributeSet()->AdjustDependentAttributes();
}

void Player::AddStatModifiers(const std::map<EStatType, float>& bonuses)
{
    AttributeSet* myStats = GetAbilityComponent()->GetAttributeSet();
    if (!myStats) return;

    for (const auto& pair : bonuses)
    {
        EStatType statTypeEnum = pair.first;
        float bonusValue = pair.second;
        std::string statKeyString = StatTypeToString(statTypeEnum);
        if (statKeyString.empty()) continue;

        auto it = myStats->AttributeMap.find(statKeyString);
        if (it != myStats->AttributeMap.end())
        {
            // 장비로 인한 스탯 변동은 BaseValue와 CurrentValue 모두에 영향을 줄 수 있음
            // (예: 최대 HP 증가) 여기서는 CurrentValue만 수정하는 것으로 단순화
            it->second->CurrentValue += bonusValue;
        }
    }
}

void Player::RemoveStatModifiers(const std::map<EStatType, float>& bonuses)
{
    AttributeSet* myStats = GetAbilityComponent()->GetAttributeSet();
    if (!myStats) return;

    for (const auto& pair : bonuses)
    {
        EStatType statTypeEnum = pair.first;
        float bonusValue = pair.second;
        std::string statKeyString = StatTypeToString(statTypeEnum);
        if (statKeyString.empty()) continue;

        auto it = myStats->AttributeMap.find(statKeyString);
        if (it != myStats->AttributeMap.end())
        {
            // 보너스 값을 다시 빼줌
            it->second->CurrentValue -= bonusValue;
        }
    }
}

void Player::Heal(float amount)
{
	AttributeSet* stats = GetAbilityComponent()->GetAttributeSet();
    if (stats)
    {
        stats->HP.CurrentValue += amount;
		// HP가 최대치를 넘지 않도록 제한
        if (stats->HP.CurrentValue > stats->HP.BaseValue)
        {
            stats->HP.CurrentValue = stats->HP.BaseValue;
		}
    }
}

void Player::IncreaseGold(int amount)
{
    AttributeSet* stats = GetAbilityComponent()->GetAttributeSet();
    if (stats && amount > 0) // stats가 유효하고, 증가량이 0보다 클 때만
    {
        stats->Gold += amount;
    }
}

bool Player::DecreaseGold(int amount)
{
    AttributeSet* stats = GetAbilityComponent()->GetAttributeSet();
    
    // stats가 유효하고, 골드가 충분한지 확인
    if (stats && stats->Gold >= amount)
    {
        stats->Gold -= amount;
        return true;
    }

    // 골드가 부족하거나 stats가 유효하지 않으면 실패
    return false;
}

// 아이템 장착 해제 함수
bool Player::Unequip(int slotIndex)
{
    InventoryComponent* inventory = GetInventory();
    if (!inventory || slotIndex < 0) return false;

    InventorySlot* slot = inventory->GetSlotAtIndex_Mutable(slotIndex);

    if (!slot || !slot->bIsEquipped) return false; // 장착된 아이템이 아니면 해제할 수 없음

    const ItemData* data = slot->pItemData;
    if (!data || data->Type != EItemType::Equipment) return false;

    // 1. 스탯 보너스를 제거
    RemoveStatModifiers(data->EquipmentData.StatBonuses);

    // 2. Player의 장착 슬롯 인덱스를 초기화
    switch (data->EquipmentData.Type)
    {
    case EEquipmentType::WEAPON: m_equippedWeaponSlot = -1; break;
    case EEquipmentType::ARMOR:  m_equippedArmorSlot = -1; break;
    case EEquipmentType::ACCESSORY: m_equippedAccessorySlot = -1; break;
    }

    // 3. 인벤토리 슬롯의 상태를 '장착 해제'로 변경
    slot->bIsEquipped = false;

    return true;
}

// 아이템 장착 함수
bool Player::Equip(int slotIndex)
{
    InventoryComponent* inventory = GetInventory();
    if (!inventory || slotIndex < 0) return false;
    InventorySlot* slotToEquip = inventory->GetSlotAtIndex_Mutable(slotIndex);

    // 슬롯이 비어있거나, 장비 아이템이 아니면 장착 불가
    if (!slotToEquip || slotToEquip->Quantity <= 0 || !slotToEquip->pItemData || slotToEquip->pItemData->Type != EItemType::Equipment)
    {
        return false;
    }

    const ItemData* data = slotToEquip->pItemData;
    EEquipmentType type = data->EquipmentData.Type;

    // 1. 같은 부위에 이미 다른 아이템이 장착되어 있다면, 먼저 해제
    int oldEquippedSlot = -1;
    switch (type)
    {
    case EEquipmentType::WEAPON: oldEquippedSlot = m_equippedWeaponSlot; break;
    case EEquipmentType::ARMOR:  oldEquippedSlot = m_equippedArmorSlot; break;
    case EEquipmentType::ACCESSORY: oldEquippedSlot = m_equippedAccessorySlot; break;
    }
    if (oldEquippedSlot != -1)
    {
        Unequip(oldEquippedSlot);
    }

    // 2. 새로운 아이템의 스탯 보너스를 적용
    AddStatModifiers(data->EquipmentData.StatBonuses);

    // 3. Player의 장착 슬롯에 새 아이템의 인덱스를 기억
    switch (type)
    {
    case EEquipmentType::WEAPON: m_equippedWeaponSlot = slotIndex; break;
    case EEquipmentType::ARMOR:  m_equippedArmorSlot = slotIndex; break;
    case EEquipmentType::ACCESSORY: m_equippedAccessorySlot = slotIndex; break;
    }

    // 4. 인벤토리 슬롯의 상태를 '장착 중'으로 변경
    slotToEquip->bIsEquipped = true;

    return true;
}

bool Player::IsSlotEquipped(int slotIndex) const
{
    if (slotIndex < 0) return false;

    return (slotIndex == m_equippedWeaponSlot ||
            slotIndex == m_equippedArmorSlot || 
		    slotIndex == m_equippedAccessorySlot);
    
}