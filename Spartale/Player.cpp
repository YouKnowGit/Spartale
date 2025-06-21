#include "Player.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include <iostream>
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

