#pragma once
#include <vector>
#include <memory>
#include <string>

// ���� ����
class Actor;
class AttributeSet;
class GameplayAbility;
class GameplayEffect;

struct FActiveGameplayEffect
{
    // � ȿ������
    std::unique_ptr<GameplayEffect> SourceEffect = nullptr;

    // ȿ���� �� �� ���Ҵ��� (-1�� ���� ����)
    int RemainingTurns = 0;
};

class AbilitySystemComponent
{
public:
    AbilitySystemComponent(Actor* Owner);
    ~AbilitySystemComponent();

    void Initialize();

	// �ش� Owner �� ������ ��ų ���: GrantAbility
    void GrantAbility(std::unique_ptr<GameplayAbility> Ability);
    void ClearAbility(GameplayAbility* Ability);

    // ���� �� ����� 4���� ��ų: EquipAbility
    void EquipAbility(int32_t SlotIndex, GameplayAbility* Ability);
    void UnEquipAbility(int32_t SlotIndex);

	// ��ų �� ������ ���
	// TryActivateAbility: �����Ƽ�� ���(Ȱ��ȭ)�ϴ� �Լ�
	// ApplyGameplayEffectToSelf: �ڽſ��� �����÷��� ȿ���� �����ϴ� �Լ�
    std::wstring TryActivateAbility(int32_t SlotIndex, Actor* Target);
    void ApplyGameplayEffectToSelf(std::unique_ptr<GameplayEffect> Effect);

	// Getter
    Actor* GetOwnerActor() const { return OwnerActor; }
    AttributeSet* GetAttributeSet() const { return MyAttributeSet.get(); }
    GameplayAbility* GetGrantedAbility(int32_t Index) const;
    const std::vector<GameplayAbility*>& GetEquippedAbilities() const { return EquippedAbilities; }

    std::wstring UpdateActiveEffects();

protected:
    // �� ������Ʈ�� ������ 
    Actor* OwnerActor;

	// �� ������Ʈ�� AttributeSet ���� (�̿� ������ �ɷ�ġ ��� �� ����, protected �̹Ƿ� Getter ���)
    std::unique_ptr<AttributeSet> MyAttributeSet;

    // ������ ��� �����Ƽ ��� (������ ��ü ��ų)
    std::vector<std::unique_ptr<GameplayAbility>> GrantedAbilities;

    // ������ �����Ƽ ��� (����ڰ� ���� 4���� ��ų)
    std::vector<GameplayAbility*> EquippedAbilities;

    // �� ĳ���Ϳ��� ���� ���� ���� ��� ����/���� ȿ�� ��� (�Խ��� ����)
    std::vector<FActiveGameplayEffect> ActiveEffects;
};