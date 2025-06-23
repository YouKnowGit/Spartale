#include "GameplayAbility.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"

GameplayAbility::GameplayAbility()
{
}


bool GameplayAbility::CanActivateAbility(AbilitySystemComponent* SourceASC) const
{
    AttributeSet* SourceAttributeSet = SourceASC->GetAttributeSet();

    // ��ȿ�� ����
    if (!SourceAttributeSet)
    {
        return false;
    }

    // ������ ������� Ȯ��
    if (SourceAttributeSet->MP.CurrentValue < this->ManaCost)
    {
        return false;
    }

	// . . . �� ���� ��ų ������ �߰��� �����Ͽ� ��ų ���� ���� �Ǻ� . . . (True/False ��ȯ)

    return true;
}