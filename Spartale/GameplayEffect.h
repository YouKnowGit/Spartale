#pragma once

#include <string>
#include "Global.h"
#include "AttributeSet.h"

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

    // ���� �ð� (�� ����)
    int Duration = 0;

    // ������ Ÿ��
    EDamageType DamageType;

    // ���� ��ġ (��������, ȸ����, ���� ������ ��)
    float Magnitude = 0.0f;

    // �� ����Ʈ�� ����� AttributeSet�� ������ �����ϴ� �Լ�
    virtual void Apply(AttributeSet* TargetAttributeSet) const;
};