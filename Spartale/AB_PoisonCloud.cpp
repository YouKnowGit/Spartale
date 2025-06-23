#include "AB_PoisonCloud.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "Actor.h"
#include "AttributeSet.h"
#include "DamageUtils.h"
#include <string>
#include <memory>

AB_PoisonCloud::AB_PoisonCloud()
{
    // �����Ƽ �⺻ ���� ����
    AbilityName = L"�� �Ȱ�";
    AbilityDescription = L"��󿡰� 3�ϰ� ���ӵǴ� �� ���ظ� �����ϴ�.";
    ManaCost = 15.0f;

    MyDamageType = EDamageType::Magical; // �� ��ų�� '����' Ÿ��
    AD_Ratio = 0.0f; // EDamageType::Magical Ÿ���� AD�� ������� �����Ƿ� 0���� ����
    AP_Ratio = 0.25f; // ���� ���ط� ���

    // ��ų ���� ������ �ʱ�ȭ
    PoisonDuration = 3;
}

std::wstring AB_PoisonCloud::ActivateAbility(AbilitySystemComponent* SourceASC, Actor* Target)
{
    // �����ڿ� ����� ��ȿ���� Ȯ��
    if (!SourceASC || !Target)
    {
        return L"����� ���� ������ �� �����ϴ�.";
    }

    const Actor* SourceActor = SourceASC->GetOwnerActor();
    AbilitySystemComponent* TargetASC = Target->GetAbilityComponent();

    if (!TargetASC)
    {
        return L"����� ȿ���� ���� �� �����ϴ�.";
    }
    // ��ų�� �ϴ� ���� �������� ���� ���
    float DotDamage = DamageUtils::CalculateDamage(SourceActor, Target, this->AD_Ratio, this->AP_Ratio, this->MyDamageType);

    // [ȿ���� ����] �ϰ� ���� ������(DotDamage)�� Magnitude�� ���� (���� �Ҵ�)
    auto PoisonEffect = std::make_unique<GameplayEffect>();
    PoisonEffect->EffectName = L"�� ����";
    PoisonEffect->ApplicationType = EEffectApplication::Duration;
    PoisonEffect->Duration = this->PoisonDuration;
    PoisonEffect->bExecuteOnTurn = true;
    PoisonEffect->TargetAttributeName = "HP";
    PoisonEffect->Magnitude = -DotDamage;
    PoisonEffect->SourceActor = SourceASC->GetOwnerActor(); // ������ �������� ���

    TargetASC->ApplyGameplayEffectToSelf(std::move(PoisonEffect));

    // �α� ��ȯ
    std::wstring LogMessage = SourceActor->Name + L"��(��) "
        + AbilityName + L"��(��) �����߽��ϴ�!";

    return LogMessage;
}