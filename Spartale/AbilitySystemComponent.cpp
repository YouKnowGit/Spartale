#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameplayAbility.h"
#include "GameplayEffect.h"
#include "Actor.h"

#include <algorithm> // std::max ����� ���� ����
#include <iostream> // �α� ����� ���� ����
#include <string> // std::wstring ����� ���� ����

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

void AbilitySystemComponent::ApplyGameplayEffectToSelf(std::unique_ptr<GameplayEffect> Effect)
{
    // Effect �����Ϳ� �ڽ��� AttributeSet ��ȿ�� �˻�
    if (!Effect || !MyAttributeSet)
    {
        return;
    }

	// ApplicationType�� ���� Case �б� ó��
    switch (Effect->ApplicationType)
    {
        case EEffectApplication::Instant:
        {
            // ��� ȿ��
            Effect->Apply(MyAttributeSet.get());
            break;
        }

        case EEffectApplication::Duration:
        case EEffectApplication::Infinite:
        {
            bool bFoundAndRefreshed = false;
            // ���� ���� ���� ȿ�� ���(ActiveEffects)�� Ȯ��
            for (FActiveGameplayEffect& ActiveEffect : this->ActiveEffects)
            {
                // 'ȿ�� �̸�'�� ���� ���� �ִ��� Ȯ��
                if (ActiveEffect.SourceEffect->EffectName == Effect->EffectName)
                {
                    // ���� ȿ���� ã�Ҵٸ� ���� �߰��ϴ� ���, ���� ȿ���� ���ӽð��� ���� (���Ȱ� �� ��ø ����)
                    ActiveEffect.RemainingTurns = Effect->Duration;
                    bFoundAndRefreshed = true;
                    break;
                }
            }

            // �ߺ��� ȿ���� ã�� ���ϰ� ���������� �ʾҴٸ� ���� ��Ͽ� �߰�
            if (!bFoundAndRefreshed)
            {
                FActiveGameplayEffect NewActiveEffect;
                NewActiveEffect.RemainingTurns = (Effect->ApplicationType == EEffectApplication::Infinite) ? -1 : Effect->Duration;
                NewActiveEffect.SourceEffect = std::move(Effect); // ������ ����
                this->ActiveEffects.push_back(std::move(NewActiveEffect));
            }

            break;
        }
    }
}

std::wstring AbilitySystemComponent::UpdateActiveEffects()
{
    std::wstring LogMessage = L"";

    for (int i = ActiveEffects.size() - 1; i >= 0; --i)
    {
        FActiveGameplayEffect& ActiveEffect = ActiveEffects[i];
        GameplayEffect* Effect = ActiveEffect.SourceEffect.get();

        if (!Effect) continue;

        // "�� �� �ߵ�" ȿ�� ó��
        if (Effect->bExecuteOnTurn)
        {
            // ����� Magnitude�� �״�� ���
            Effect->Apply(MyAttributeSet.get());

            // �α׸� ����. Effect�� ����� SourceActor�� ���
            if (Effect->SourceActor)
            {
                LogMessage += Effect->SourceActor->Name + L"�� " + Effect->EffectName + L" ȿ���� "
                    + OwnerActor->Name + L"��(��) "
                    + std::to_wstring(static_cast<int>(-Effect->Magnitude)) + L"�� ���ظ� �Ծ����ϴ�.";
            }
        }

        // ���� �� ����
        if (ActiveEffect.RemainingTurns > 0)
        {
            ActiveEffect.RemainingTurns--;
        }

        // ȿ�� ���� ó��
        if (ActiveEffect.RemainingTurns == 0)
        {
            ActiveEffects.erase(ActiveEffects.begin() + i);
        }
    }
    return LogMessage;
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