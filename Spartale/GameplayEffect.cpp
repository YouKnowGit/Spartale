#include "GameplayEffect.h"

GameplayEffect::GameplayEffect()
{
}

GameplayEffect::~GameplayEffect()
{
}

// ���� �ٽ����� �Լ�
// TargetAttributeName�� ������� TargetAttributeSet�� ���� �Ӽ��� ã��
// Magnitude��ŭ ��ġ�� �����ϴ� �Լ�
void GameplayEffect::Apply(AttributeSet* TargetAttributeSet) const
{
    // ����� AttributeSet ��ȿ�� �˻�
    if (!TargetAttributeSet)
    {
        return;
    }

    // 1. �ʿ��� TargetAttributeName�� �ش��ϴ� �Ӽ��� find
    auto it = TargetAttributeSet->AttributeMap.find(TargetAttributeName);

	// 2. �Ӽ��� ���������� ã�Ҵ��� �˻�
    if (it != TargetAttributeSet->AttributeMap.end())
    {
        // 3. �ش� �Ӽ��� ���� ���� Magnitude ��ŭ ����
        // it->first�� Ű("HP"), it->second�� ��(HP �������� �ּ�)
        FAttributeData* AttributeToChange = it->second;

        // ���� ���� ���� ���
        float OldValue = AttributeToChange->CurrentValue;

        AttributeToChange->CurrentValue += Magnitude;

        // AttributeSet���� "���� �ٲ������ ��ó��" ��� �˷���
        TargetAttributeSet->PostAttributeChange(*AttributeToChange, OldValue, AttributeToChange->CurrentValue);

    }
}

std::unique_ptr<GameplayEffect> GameplayEffect::CreateInverseEffect() const
{
    // 1. ���ο� GameplayEffect ��ü ����
    auto InverseEffect = std::make_unique<GameplayEffect>();

	// 2. InverseEffect�� �Ӽ����� ����
    InverseEffect->EffectName = this->EffectName + L" (����)";
    InverseEffect->TargetAttributeName = this->TargetAttributeName;

    // 3. ���� ����� ��� (Instant) �� ó��
    InverseEffect->ApplicationType = EEffectApplication::Instant;

    // 4. ���� ��ġ(Magnitude) �� ���ݴ�� ����
    InverseEffect->Magnitude = -this->Magnitude;

    // 5. �ϼ��� �ݴ� ȿ���� ��ȯ
    return InverseEffect;
}