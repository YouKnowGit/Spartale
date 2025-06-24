#include "GameLogic/Units/Player.h"
#include "GameLogic/Skills/SkillFactory.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/AttributeSet.h"
#include "Framework/AbilitySystem/GameplayAbility.h"

Player::Player(std::wstring name)
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
    const std::vector<std::string> allSkillIDs = {
        "SK_NormalAttack",
        "SK_PoisonCloud",
        "SK_Fireball",
        "SK_TripleSlash",
        "SK_StrengthBuff",
        "SK_AgilityDown",
        "SK_Meditate",
        "SK_Heal"
    };
    for (const auto& id : allSkillIDs)
    {
        GetAbilityComponent()->GrantAbility(SkillFactory::CreateSkill(id));
    }

    // 기본 공격만 0번 슬롯에 장착
    GetAbilityComponent()->EquipAbility(0, this->GetAbilityComponent()->GetGrantedAbility(0));

    // Player 위치 및 방향 관련 데이터
    CurrentLocation.X = 5;
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