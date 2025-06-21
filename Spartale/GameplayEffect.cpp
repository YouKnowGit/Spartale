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
{// ����� AttributeSet ��ȿ�� �˻�
    if (!TargetAttributeSet)
    {
        return;
    }

    // ����Ʈ ���� ��Ŀ� ���� ������ �б�
    // ����� ��� ����(Instant)�� ����
    switch (ApplicationType)
    {
    case EEffectApplication::Instant:
    {
        // TargetAttributeName ���ڿ��� ���Ͽ� � �Ӽ��� �������� ����
        if (TargetAttributeName == "HP")
        {
            TargetAttributeSet->HP.CurrentValue += Magnitude;
        }
        else if (TargetAttributeName == "MP")
        {
            TargetAttributeSet->MP.CurrentValue += Magnitude;
        }
        else if (TargetAttributeName == "Strength")
        {
            TargetAttributeSet->Strength.CurrentValue += Magnitude;
        }
        else if (TargetAttributeName == "Agility")
        {
            TargetAttributeSet->Agility.CurrentValue += Magnitude;
        }
        // ... ( �߰����� �Ӽ��鿡 ���� ó���Ϸ��� ���⿡ else if ���� �߰��ϼ��� ) ...
        else if (TargetAttributeName == "Gold")
        {
            TargetAttributeSet->Gold.CurrentValue += Magnitude;
        }

        break;
    }

    case EEffectApplication::Duration:
    {
		// ���� ȿ�� (��, ���� ��)
        break;
    }

    case EEffectApplication::Infinite:
    {
        // TODO: ���� ȿ�� ���� ����
        // �������� ��� �������� ���� ���ʽ� � ���
        break;
    }
    }
}