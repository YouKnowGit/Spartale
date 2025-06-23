#include "AB_NormalAttack.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "Actor.h"
#include "AttributeSet.h"
#include "DamageUtils.h"
#include <map>
#include <string> // std::to_wstring ����� ���� ����

AB_NormalAttack::AB_NormalAttack()
{
    // �����Ƽ �⺻ ���� ����
    AbilityName = L"�Ϲ� ����";
    AbilityDescription = L"��󿡰� ���� ���ظ� �����ϴ�.";
    MyDamageType = EDamageType::Physical; // �� ��ų�� '����' Ÿ��
    AD_Ratio = 1.0f; // ���ط� ���. 1.0f�� '��' �ɷ�ġ�� 100%�� ���ط����� ����Ѵٴ� �ǹ�
	AP_Ratio = 0.0f; // EDamageType::Physical Ÿ���� AP�� ������� �����Ƿ� 0���� ����
}

void AB_NormalAttack::SetupEffects()
{
}

std::wstring AB_NormalAttack::ActivateAbility(AbilitySystemComponent* SourceASC, Actor* Target)
{
    // �����ڿ� ����� ��ȿ���� Ȯ��
    if (!SourceASC || !Target)
    {
        return L"����� ���� ������ �� �����ϴ�.";
    }

    const Actor* SourceActor = SourceASC->GetOwnerActor();

    // 1. ������ ����� ���ο� DamageUtils �Լ��� ����
    float Damage = DamageUtils::CalculateDamage(SourceActor, Target, this->AD_Ratio, this->AP_Ratio, this->MyDamageType);

    // 2. ȿ���� ���� ������ GameplayEffect�� ����
    auto DamageEffect = std::make_unique<GameplayEffect>();
    DamageEffect->EffectName = L"������";
    DamageEffect->ApplicationType = EEffectApplication::Instant;
	DamageEffect->TargetAttributeName = "HP";    // ����� HP �Ӽ��� ����
    DamageEffect->Magnitude = -Damage;   // HP �����̹Ƿ� ������ ����

    // 3. ��󿡰� ȿ���� ����
    Target->GetAbilityComponent()->ApplyGameplayEffectToSelf(std::move(DamageEffect));

    // 4. �α׸� ��ȯ
    std::wstring LogMessage = SourceActor->Name + L"��(��) "
        + Target->Name + L"���� "
        + std::to_wstring(static_cast<int>(Damage))
        + L"�� ���ظ� �������ϴ�.";

    return LogMessage;
}