#include "GameLogic/Units/Player.h"
#include "GameLogic/Skills/SkillFactory.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/AttributeSet.h"
#include "Framework/AbilitySystem/GameplayAbility.h"
#include "Utils/StringUtils.h"

#include <sstream>


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
        MyStats->HP.BaseValue = 200.f;
        MyStats->HP.CurrentValue = 200.f;
        MyStats->MP.BaseValue = 75.f;
        MyStats->MP.CurrentValue = 75.f;

        MyStats->Strength.BaseValue = 30.f;
        MyStats->Strength.CurrentValue = 30.f;
        MyStats->Agility.BaseValue = 15.f;
        MyStats->Agility.CurrentValue = 15.f;
        MyStats->Defence.BaseValue = 15.f;
        MyStats->Defence.CurrentValue = 15.f;
        MyStats->Intelligence.CurrentValue = 15.f;
        MyStats->Intelligence.BaseValue = 15.f;
    }

    // 기본 공격만 0번 슬롯에 장착
    GetAbilityComponent()->GrantAbility(SkillFactory::CreateSkill("SK_NormalAttack"));

    // Player 위치 및 방향 관련 데이터
    CurrentLocation.X = 9;
    CurrentLocation.Y = 5;
    m_direction = Direction::DOWN;

	// 시작 아이템으로 HP 포션을 3개 추가합니다.
    GetInventory()->AddItem("consume_hp_potion_01", 3);
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
}
void Player::Load(std::ifstream& file)
{
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
    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        if (line == "[Player]") {
            in_player_section = true;
            continue;
        }

        if (in_player_section && line[0] == '[') {
            break;
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
    }
}