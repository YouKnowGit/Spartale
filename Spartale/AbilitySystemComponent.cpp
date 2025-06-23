#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameplayAbility.h"
#include "GameplayEffect.h"
#include "Actor.h"

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
std::wstring AbilitySystemComponent::TryActivateAbility(int32_t SlotIndex, Actor* Target)
{ // 1. 슬롯 인덱스가 유효한지, 해당 슬롯에 어빌리티가 장착되어 있는지 확인
    if (SlotIndex < 0 || SlotIndex >= EquippedAbilities.size() || EquippedAbilities[SlotIndex] == nullptr)
    {
        return L"해당 슬롯에 스킬이 없습니다.";
    }

    // 2. 장착된 어빌리티를 가져옴
    GameplayAbility* AbilityToActivate = EquippedAbilities[SlotIndex];

    // 3. 해당 어빌리티를 지금 사용할 수 있는지 확인 (비용, 쿨타임 등)
    if (!AbilityToActivate->CanActivateAbility(this))
    {
        return L"스킬을 사용할 수 없습니다. (쿨타임, 마나 부족 등)";
    }

    // 4. 모든 검사를 통과했으면, 어빌리티를 최종적으로 활성화(사용)
    std::wstring LogMessage = AbilityToActivate->ActivateAbility(this, Target);

    // 5. 어빌리티 실행 결과로 받은 로그 메시지를 반환
    return LogMessage;
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