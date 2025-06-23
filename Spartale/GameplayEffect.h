#pragma once
#include <string>
#include "Global.h"
#include "AttributeSet.h"
enum class EAttributeOperation {
    Add,
    Subtract,
    Multiply,
    Divide
};
class GameplayEffect
{
private:
    
    EAttributeOperation Operation;
public:
    GameplayEffect();
    GameplayEffect(const std::wstring& attributeName, EAttributeOperation operation, float magnitude)
        : TargetAttributeName(attributeName), Operation(operation), Magnitude(magnitude)
    {
    }
    virtual ~GameplayEffect();

    // ����Ʈ�� �̸�
    std::wstring EffectName;

    // ����Ʈ ���� ��� (���, ����, ����)
    EEffectApplication ApplicationType;

    // ����Ʈ�� ����� �Ӽ��� �̸� (��: "HP", "Strength")
    //std::string TargetAttributeName;
    std::wstring TargetAttributeName;
    // ���� �ð� (�� ����)
    int Duration = 0;

    // ������ Ÿ��
    EDamageType DamageType;

    // ���� ��ġ (��������, ȸ����, ���� ������ ��)
    float Magnitude = 0.0f;

    // �� ����Ʈ�� ����� AttributeSet�� ������ �����ϴ� �Լ�
    virtual void Apply(AttributeSet* TargetAttributeSet) const;
    virtual void Remove(AttributeSet* TargetAttributeSet) const;
    
    //virtual ~GameplayEffect();
    
};