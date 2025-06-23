#pragma once

#include <string>
#include <memory>
#include "Global.h"
#include "AttributeSet.h"

class Actor;

class GameplayEffect
{
public:
    GameplayEffect();
    virtual ~GameplayEffect();

    // ����Ʈ�� �̸�
    std::wstring EffectName;

    // ����Ʈ ���� ��� (���, ����, ����)
    EEffectApplication ApplicationType;

    // ����Ʈ�� ����� �Ӽ��� �̸� (��: "HP", "Strength")
    std::string TargetAttributeName;

    EModifierOperation OperationType = EModifierOperation::Add; // �⺻�� ����

    // ȿ���� ���ݴ� ȿ���� �����ؼ� ��ȯ�ϴ� �Լ� (���� ȿ���� ���� �����ϱ� ���� ���)
    std::unique_ptr<GameplayEffect> CreateInverseEffect() const;

    // ���� �ð� (�� ����)
    int Duration = 0;

    // ������ Ÿ��
    EDamageType DamageType;

    // ���� ��ġ (��������, ȸ����, ���� ������ ��)
    float Magnitude = 0.0f;

    bool bExecuteOnTurn = false;

    // ȿ�� ������
    Actor* SourceActor = nullptr;

    // �� ����Ʈ�� ����� AttributeSet�� ������ �����ϴ� �Լ�
    virtual void Apply(AttributeSet* TargetAttributeSet) const;
};