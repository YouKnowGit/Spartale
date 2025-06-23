#include "GameLogic/Units/Player.h"
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
        MyStats->HP.BaseValue = 150.f;
        MyStats->HP.CurrentValue = 150.f;
        MyStats->MP.BaseValue = 100.f;
        MyStats->MP.CurrentValue = 100.f;

        MyStats->Strength.BaseValue = 15.f;
        MyStats->Strength.CurrentValue = 15.f;
        MyStats->Defence.BaseValue = 10.f;
        MyStats->Defence.CurrentValue = 10.f;
        MyStats->Intelligence.CurrentValue = 10.f;
        MyStats->Intelligence.BaseValue = 10.f;

    }

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