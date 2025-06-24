#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/AttributeSet.h"
#include "Framework/AbilitySystem/GameplayAbility.h"
#include "Framework/AbilitySystem/GameplayEffect.h"
#include "Core/Actor.h"

#include <algorithm> // std::max 사용을 위해 포함
#include <iostream> // 로그 출력을 위해 포함
#include <string> // std::wstring 사용을 위해 포함

AbilitySystemComponent::AbilitySystemComponent(Actor* Owner)
    : OwnerActor(Owner) // 멤버 이니셜라이저를 사용하여 OwnerActor를 초기화 (Actor.cpp 에서 사용함)
{
	// 이 컴포넌트가 생성될 때, 자신의 AttributeSet을 생성하여 소유 (모든 Actor는 AttributeSet을 소유)
    MyAttributeSet = std::make_unique<AttributeSet>(this);

    // EquippedAbilities(전투 시 사용할 스킬 4개) 벡터를 미리 4칸(0, 1, 2, 3번 슬롯)으로 초기화
    EquippedAbilities.resize(4, nullptr);
}

// unique_ptr 사용하니 소멸자는 구현만
AbilitySystemComponent::~AbilitySystemComponent()
{
}

// AbilitySystemComponent의 초기화 함수
void AbilitySystemComponent::Initialize()
{
}

bool AbilitySystemComponent::CheckAndProcessLevelUp()
{
    if (!MyAttributeSet) return false;

    // 레벨업에 필요한 경험치 공식 (예: 레벨 * 50)
    float requiredExp = 50.0f * MyAttributeSet->Level;
    bool bLeveledUp = false;

    // 현재 경험치가 필요 경험치보다 많으면 레벨업 (여러 번 레벨업 가능하도록 while 사용)
    while (MyAttributeSet->Experience.CurrentValue >= requiredExp)
    {
        // 레벨업 비용만큼 경험치 차감
        MyAttributeSet->Experience.CurrentValue -= requiredExp;

        // 레벨 1 증가
        MyAttributeSet->Level++;
        bLeveledUp = true;

        // 레벨업 보상 (스탯 포인트 5 지급)
        MyAttributeSet->AdditionalStatPoints += 5;

        // 기본 스탯 자동 상승
        MyAttributeSet->Strength.BaseValue += 2;
        MyAttributeSet->Strength.CurrentValue += 2;
        MyAttributeSet->Agility.BaseValue += 1;
        MyAttributeSet->Agility.CurrentValue += 1;
        MyAttributeSet->Intelligence.BaseValue += 1;
        MyAttributeSet->Intelligence.CurrentValue += 1;
        MyAttributeSet->Defence.BaseValue += 1.5;
        MyAttributeSet->Defence.CurrentValue += 1.5;
        MyAttributeSet->MagicResistance.BaseValue += 1.5;
        MyAttributeSet->MagicResistance.CurrentValue += 1.5;

        // 스탯 변경에 따른 최대 HP/MP 재계산
        MyAttributeSet->AdjustDependentAttributes();

        // 다음 레벨업에 필요한 경험치를 다시 계산
        requiredExp = 100.0f * MyAttributeSet->Level;
    }

    return bLeveledUp;
}
// unique_ptr을 인자로 받아서 GrantedAbilities 벡터에 추가. (스킬을 배우는 함수)
void AbilitySystemComponent::GrantAbility(std::unique_ptr<GameplayAbility> Ability)
{
	if (Ability) // nullptr 체크를 통해 유효성 검사 (언리얼의 IsValid 노드 역할)
    {
        GrantedAbilities.push_back(std::move(Ability));
    }
}

// 스킬의 이름을 통해 해당 스킬을 찾고 제거하는 함수
void AbilitySystemComponent::ClearAbility(GameplayAbility* Ability)
{
}

// EquipAbility 는 전투 시 사용할 4개의 스킬을 담아놓는 벡터
// 사용자가 PauseMenu 에서 스킬을 장착하고 해제할 때 사용하는 함수
void AbilitySystemComponent::EquipAbility(int32_t SlotIndex, GameplayAbility* Ability)
{
    // 슬롯 인덱스가 유효한 범위(0~3)인지 확인
    if (SlotIndex >= 0 && SlotIndex < EquippedAbilities.size())
    {
        EquippedAbilities[SlotIndex] = Ability;
    }
}

// 반대로 스킬을 해제하는 함수
void AbilitySystemComponent::UnEquipAbility(int32_t SlotIndex)
{
    if (SlotIndex >= 0 && SlotIndex < EquippedAbilities.size())
    {
        EquippedAbilities[SlotIndex] = nullptr;
    }
}

// 최종적으로 스킬을 활성화하는 함수 (방아쇠 역할)
// 스킬 사용이 가능한지(True, False)와 스킬 시전 결과에 대한 메세지 반환
FActivationResult AbilitySystemComponent::TryActivateAbility(int32_t SlotIndex, Actor* Target)
{
    if (SlotIndex < 0 || SlotIndex >= EquippedAbilities.size() || EquippedAbilities[SlotIndex] == nullptr)
    {
        return { false, L"해당 슬롯에 스킬이 없습니다." };
    }

    GameplayAbility* AbilityToActivate = EquippedAbilities[SlotIndex];

    if (!AbilityToActivate->CanActivateAbility(this))
    {
        return { false, L"스킬을 사용할 수 없습니다. (마나 부족 등)" };
    }

    std::wstring LogMessage = AbilityToActivate->ActivateAbility(this, Target);

    // 성공 시에는 bSuccess를 true로 설정하여 반환
    return { true, LogMessage };
}

void AbilitySystemComponent::ApplyGameplayEffectToSelf(std::unique_ptr<GameplayEffect> Effect)
{
    // Effect 포인터와 자신의 AttributeSet 유효성 검사
    if (!Effect || !MyAttributeSet)
    {
        return;
    }

	// ApplicationType에 따라 Case 분기 처리
    switch (Effect->ApplicationType)
    {
        case EEffectApplication::Instant:
        {
            // 즉시 효과
            Effect->Apply(MyAttributeSet.get());
            break;
        }

        case EEffectApplication::Duration:
        case EEffectApplication::Infinite:
        {
            bool bFoundAndRefreshed = false;
            // 현재 적용 중인 효과 목록(ActiveEffects)을 확인
            for (FActiveGameplayEffect& ActiveEffect : this->ActiveEffects)
            {
                // '효과 이름'이 같은 것이 있는지 확인
                if (ActiveEffect.SourceEffect->EffectName == Effect->EffectName)
                {
                    // 같은 효과를 찾았다면 새로 추가하는 대신, 기존 효과의 지속시간만 갱신 (독안개 등 중첩 방지)
                    ActiveEffect.RemainingTurns = Effect->Duration;
                    bFoundAndRefreshed = true;
                    break;
                }
            }

            // 중복된 효과를 찾지 못하고 갱신하지도 않았다면 새로 목록에 추가
            if (!bFoundAndRefreshed)
            {
                FActiveGameplayEffect NewActiveEffect;
                NewActiveEffect.RemainingTurns = (Effect->ApplicationType == EEffectApplication::Infinite) ? -1 : Effect->Duration;
                NewActiveEffect.SourceEffect = std::move(Effect); // 소유권 이전
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

        // "매 턴 발동" 효과 처리
        if (Effect->bExecuteOnTurn)
        {
            // 저장된 Magnitude를 그대로 사용
            Effect->Apply(MyAttributeSet.get());

            // 로그를 생성. Effect에 저장된 SourceActor를 사용
            if (Effect->SourceActor)
            {
                LogMessage += Effect->SourceActor->Name + L"의 " + Effect->EffectName + L" 효과로 "
                    + OwnerActor->Name + L"이(가) "
                    + std::to_wstring(static_cast<int>(-Effect->Magnitude)) + L"의 피해를 입었습니다.";
            }
        }

        // 남은 턴 관리
        if (ActiveEffect.RemainingTurns > 0)
        {
            ActiveEffect.RemainingTurns--;
        }

        // 효과 만료 처리
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
    // Index가 유효한 범위(0부터 벡터 크기 - 1까지)에 있는지 확인
        if (Index >= 0 && Index < GrantedAbilities.size())
        {
            // unique_ptr로 저장된 어빌리티의 순수 포인터(.get())를 반환
            return GrantedAbilities[Index].get();
        }

    // 유효하지 않은 인덱스라면 nullptr을 반환
    return nullptr;
}
const std::vector<std::unique_ptr<GameplayAbility>>& AbilitySystemComponent::GetGrantedAbilities() const
{
    return GrantedAbilities;
}