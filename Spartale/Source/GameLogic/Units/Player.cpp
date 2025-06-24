#include "GameLogic/Units/Player.h"
#include "GameLogic/Skills/AB_NormalAttack.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/AttributeSet.h"

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
        MyStats->HP.BaseValue = 100.f;
        MyStats->HP.CurrentValue = 100.f;
        MyStats->MP.BaseValue = 75.f;
        MyStats->MP.CurrentValue = 75.f;

        MyStats->Strength.BaseValue = 15.f;
        MyStats->Strength.CurrentValue = 15.f;
        MyStats->Agility.BaseValue = 10.f;
        MyStats->Agility.CurrentValue = 10.f;
        MyStats->Defence.BaseValue = 10.f;
        MyStats->Defence.CurrentValue = 10.f;
        MyStats->Intelligence.CurrentValue = 10.f;
        MyStats->Intelligence.BaseValue = 10.f;


    }
    GetAbilityComponent()->GrantAbility(std::make_unique<AB_NormalAttack>());
    GetAbilityComponent()->EquipAbility(0, this->GetAbilityComponent()->GetGrantedAbility(0));

    // Player 위치 및 방향 관련 데이터
    m_x = 5;
    m_y = 5;
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