#include "Player.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"

Player::Player(std::wstring name)
{
    // �÷��̾� �̸� ����
    Name = name;

	// Player ��ü ���� �� �ٷ� �ʱ�ȭ �Լ� ȣ��
    Initialize();
}

void Player::Initialize()
{
    // ASC�� ���� AttributeSet�� ����
    AttributeSet* MyStats = GetAbilityComponent()->GetAttributeSet();
    if (MyStats)
    {
        // �÷��̾��� �ʱ� �ɷ�ġ�� ����
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

    // �ʵ�� ��ġ ����
    m_x = 5;
    m_y = 5;
    m_direction = Direction::DOWN;
}

void Player::Update()
{
    // TODO: �� �����Ӹ��� �÷��̾ ó���ؾ� �� ���� (��: �Է� ó��)
}

void Player::Render()
{
    // TODO: �÷��̾��� ������ ȭ�鿡 �׸��� ����
}

void Player::SetDirection(Direction dir)
{
    m_direction = dir;
}

Direction Player::GetDirection() const
{
    return m_direction;
}