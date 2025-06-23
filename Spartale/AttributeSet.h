#pragma once
#include <map>
#include <string>
#include "Global.h"

class AbilitySystemComponent;

// �� �ɷ�ġ(Attribute)�� ���� ���� �⺻ ���� �����ϴ� ����ü
struct FAttributeData
{
	float BaseValue = 0.0f; // �⺻ ��(�ɷ�ġ �������̳� ��ų�� ���� ������� ����)
	float CurrentValue = 0.0f;  // ���� ��(�ɷ�ġ �������̳� ��ų�� ���� ����� �� ����)

	// �����ڸ� ���� �ϳ��� ������ 2���� ����� �ʱ�ȭ�� �� �ֵ��� ����
    FAttributeData(float value = 0.0f) : BaseValue(value), CurrentValue(value) {}
};
class AttributeSet
{
public:
    AttributeSet(AbilitySystemComponent* OwnerComp);
    virtual ~AttributeSet() = default;

    // �Ӽ� �̸����� ���� �����Ϳ� ������ �� �ְ� ���ִ� ��
	// �Ӽ� �̸��� ���ڿ��� ����Ǹ�, �ش� �Ӽ��� FAttributeData �����͸� ����
    std::map<std::string, FAttributeData*> AttributeMap;

    AbilitySystemComponent* OwningAbilitySystemComponent;

    // --- �⺻ ���� (Primary Stats) ---
    FAttributeData Strength;          // ��
    FAttributeData Agility;           // ��ø
    FAttributeData Intelligence;      // ����
    FAttributeData Defence;           // ����
    FAttributeData MagicResistance;   // ���� ���׷�

    // --- �ڿ� (Resources) ---
    FAttributeData HP;
    FAttributeData MP;
    FAttributeData BaseHP;
    FAttributeData BaseMP;

    // ���� ����ġ, ����
    FAttributeData Gold;
    FAttributeData Experience;
    int Level = 1;

    FAttributeData CriticalHitChance;          // ġ��Ÿ Ȯ�� (��: 0.05 = 5%)
    FAttributeData CriticalHitDamageMultiplier; // ġ��Ÿ ���ط� ���� (��: 1.5 = 150%)

    // --- ��Ÿ ������ (Meta Data) ---
    bool bIsDefending = false; // ���� ��� �������� ���� (��� �ý��� ���� �� ����)
    int AdditionalStatPoints = 0; // �й� ������ �߰� ���� ����Ʈ

    // �ɷ�ġ�� ����Ǳ� ������ ȣ��� �Լ�
    virtual void PreAttributeChange(const FAttributeData& Attribute, float& NewValue);

    // �ɷ�ġ�� ����� ���Ŀ� ȣ��� �Լ�
    virtual void PostAttributeChange(const FAttributeData& Attribute, float OldValue, float NewValue);

    // ��, ���� ���ȿ� ���� MaxHP, MaxMP�� �����ϴ� �Լ�
    void AdjustDependentAttributes();
};