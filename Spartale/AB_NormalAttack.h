#pragma once
#include "GameplayAbility.h"

// '�Ϲ� ����' �����Ƽ Ŭ������ ����
class AB_NormalAttack : public GameplayAbility
{
public:
    AB_NormalAttack();
    virtual ~AB_NormalAttack() = default;

    // GameplayAbility�� ���� ���� �Լ��� ������
    virtual std::wstring ActivateAbility(AbilitySystemComponent* SourceASC, Actor* Target) override;
    virtual void SetupEffects() override;

private:
    // �� ��ų�� ������ Ÿ��
    EDamageType MyDamageType;
};