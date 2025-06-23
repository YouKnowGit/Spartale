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
        if (TargetAttributeName == L"HP")
        {
            TargetAttributeSet->HP.CurrentValue += Magnitude;
        }
        else if (TargetAttributeName == L"MP")
        {
            TargetAttributeSet->MP.CurrentValue += Magnitude;
        }
        else if (TargetAttributeName == L"Strength")
        {
            TargetAttributeSet->Strength.CurrentValue += Magnitude;
        }
        else if (TargetAttributeName == L"Agility")
        {
            TargetAttributeSet->Agility.CurrentValue += Magnitude;
        }
        // ... ( �߰����� �Ӽ��鿡 ���� ó���Ϸ��� ���⿡ else if ���� �߰��ϼ��� ) ...
        else if (TargetAttributeName == L"Gold")
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
void GameplayEffect::Remove(AttributeSet* TargetAttributeSet) const
{
    if (!TargetAttributeSet) return;

    // ����Ʈ ���� ��Ŀ� ���� ������ �б�
    switch (ApplicationType)
    {
    case EEffectApplication::Instant:
    case EEffectApplication::Infinite:
    {
        if (TargetAttributeName == L"HP")
        {
            TargetAttributeSet->HP.CurrentValue -= Magnitude;
        }
        else if (TargetAttributeName == L"MP")
        {
            TargetAttributeSet->MP.CurrentValue -= Magnitude;
        }
        else if (TargetAttributeName == L"Strength")
        {
            TargetAttributeSet->Strength.CurrentValue -= Magnitude;
        }
        else if (TargetAttributeName == L"Agility")
        {
            TargetAttributeSet->Agility.CurrentValue -= Magnitude;
        }
        else if (TargetAttributeName == L"Gold")
        {
            TargetAttributeSet->Gold.CurrentValue -= Magnitude;
        }
        break;
    }

    case EEffectApplication::Duration:
        // TODO: ���� ȿ�� ���� ����
        break;
    }
}