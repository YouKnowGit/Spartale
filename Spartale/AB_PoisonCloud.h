#pragma once
#include "GameplayAbility.h"

// '���Ȱ�' �����Ƽ Ŭ������ ����
class AB_PoisonCloud : public GameplayAbility
{
public:
    AB_PoisonCloud();
    virtual ~AB_PoisonCloud() = default;

    // GameplayAbility�� ���� ���� �Լ� ������
    virtual std::wstring ActivateAbility(AbilitySystemComponent* SourceASC, Actor* Target) override;
    virtual void SetupEffects() override {}

private:
    // �� ��ų�� ������ Ÿ��
    EDamageType MyDamageType;

    // �� ��ų�� ���� ������
    int PoisonDuration;       // �� ���ӽð� (��)
};