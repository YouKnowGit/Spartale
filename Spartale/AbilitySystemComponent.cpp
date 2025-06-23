#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameplayAbility.h"
#include "GameplayEffect.h"
#include "Actor.h"

#include <algorithm> // std::max ����� ���� ����
#include <iostream> // �α� ����� ���� ����

AbilitySystemComponent::AbilitySystemComponent()
{
    MyAttributeSet = std::make_unique<AttributeSet>(this);
    EquippedAbilities.resize(4, nullptr);
}

AbilitySystemComponent::AbilitySystemComponent(Actor* Owner)
    : OwnerActor(Owner) // ��� �̴ϼȶ������� ����Ͽ� OwnerActor�� �ʱ�ȭ (Actor.cpp ���� �����)
{
	// �� ������Ʈ�� ������ ��, �ڽ��� AttributeSet�� �����Ͽ� ���� (��� Actor�� AttributeSet�� ����)
    MyAttributeSet = std::make_unique<AttributeSet>(this);

    // EquippedAbilities(���� �� ����� ��ų 4��) ���͸� �̸� 4ĭ(0, 1, 2, 3�� ����)���� �ʱ�ȭ
    EquippedAbilities.resize(4, nullptr);
}

// unique_ptr ����ϴ� �Ҹ��ڴ� ������
AbilitySystemComponent::~AbilitySystemComponent()
{
}

// AbilitySystemComponent�� �ʱ�ȭ �Լ�
void AbilitySystemComponent::Initialize()
{
}

// unique_ptr�� ���ڷ� �޾Ƽ� GrantedAbilities ���Ϳ� �߰�. (��ų�� ���� �Լ�)
void AbilitySystemComponent::GrantAbility(std::unique_ptr<GameplayAbility> Ability)
{
	if (Ability) // nullptr üũ�� ���� ��ȿ�� �˻� (�𸮾��� IsValid ��� ����)
    {
        GrantedAbilities.push_back(std::move(Ability));
    }
}

// ��ų�� �̸��� ���� �ش� ��ų�� ã�� �����ϴ� �Լ�
void AbilitySystemComponent::ClearAbility(GameplayAbility* Ability)
{
}

// EquipAbility �� ���� �� ����� 4���� ��ų�� ��Ƴ��� ����
// ����ڰ� PauseMenu ���� ��ų�� �����ϰ� ������ �� ����ϴ� �Լ�
void AbilitySystemComponent::EquipAbility(int32_t SlotIndex, GameplayAbility* Ability)
{
    // ���� �ε����� ��ȿ�� ����(0~3)���� Ȯ��
    if (SlotIndex >= 0 && SlotIndex < EquippedAbilities.size())
    {
        EquippedAbilities[SlotIndex] = Ability;
    }
}

// �ݴ�� ��ų�� �����ϴ� �Լ�
void AbilitySystemComponent::UnEquipAbility(int32_t SlotIndex)
{
    if (SlotIndex >= 0 && SlotIndex < EquippedAbilities.size())
    {
        EquippedAbilities[SlotIndex] = nullptr;
    }
}

// ���������� ��ų�� Ȱ��ȭ�ϴ� �Լ� (��Ƽ� ����)
std::wstring AbilitySystemComponent::TryActivateAbility(int32_t SlotIndex, Actor* Target)
{ // 1. ���� �ε����� ��ȿ����, �ش� ���Կ� �����Ƽ�� �����Ǿ� �ִ��� Ȯ��
    if (SlotIndex < 0 || SlotIndex >= EquippedAbilities.size() || EquippedAbilities[SlotIndex] == nullptr)
    {
        return L"�ش� ���Կ� ��ų�� �����ϴ�.";
    }

    // 2. ������ �����Ƽ�� ������
    GameplayAbility* AbilityToActivate = EquippedAbilities[SlotIndex];

    // 3. �ش� �����Ƽ�� ���� ����� �� �ִ��� Ȯ�� (���, ��Ÿ�� ��)
    if (!AbilityToActivate->CanActivateAbility(this))
    {
        return L"��ų�� ����� �� �����ϴ�. (��Ÿ��, ���� ���� ��)";
    }

    // 4. ��� �˻縦 ���������, �����Ƽ�� ���������� Ȱ��ȭ(���)
    std::wstring LogMessage = AbilityToActivate->ActivateAbility(this, Target);

    // 5. �����Ƽ ���� ����� ���� �α� �޽����� ��ȯ
    return LogMessage;
}

void AbilitySystemComponent::ApplyGameplayEffectToSelf(const GameplayEffect* Effect)
{
    // Effect �����Ϳ� �ڽ��� AttributeSet ��ȿ�� �˻�
    if (Effect && MyAttributeSet)
    {
        // Effect�� Apply �Լ��� ȣ���Ͽ� ���� ȿ���� ����
        Effect->Apply(MyAttributeSet.get());

        if (Effect->TargetAttributeName == L"HP")
        {
            AttributeSet* MyAS = GetAttributeSet();

			// '�ڽ���' AttributeSet�� �ִ� HP ���� �о 0�� ��. ( 0���� ������ 0���� ���� )
            MyAS->HP.CurrentValue = std::max(0.0f, MyAS->HP.CurrentValue);
        }
    }
}

// Getter
GameplayAbility* AbilitySystemComponent::GetGrantedAbility(int32_t Index) const
{
    // Index�� ��ȿ�� ����(0���� ���� ũ�� - 1����)�� �ִ��� Ȯ��
        if (Index >= 0 && Index < GrantedAbilities.size())
        {
            // unique_ptr�� ����� �����Ƽ�� ���� ������(.get())�� ��ȯ
            return GrantedAbilities[Index].get();
        }

    // ��ȿ���� ���� �ε������ nullptr�� ��ȯ
    return nullptr;
}