#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameplayAbility.h"
#include "GameplayEffect.h"
#include "Actor.h"

#include <algorithm> // std::max 사용을 위해 포함
#include <iostream> // 로그 출력을 위해 포함

AbilitySystemComponent::AbilitySystemComponent()
{
    MyAttributeSet = std::make_unique<AttributeSet>(this);
    EquippedAbilities.resize(4, nullptr);
}

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

void AbilitySystemComponent::ApplyGameplayEffectToSelf(const GameplayEffect* Effect)
{
    // Effect 포인터와 자신의 AttributeSet 유효성 검사
    if (Effect && MyAttributeSet)
    {
        // Effect의 Apply 함수를 호출하여 실제 효과를 적용
        Effect->Apply(MyAttributeSet.get());

        if (Effect->TargetAttributeName == L"HP")
        {
            AttributeSet* MyAS = GetAttributeSet();

			// '자신의' AttributeSet에 있는 HP 값을 읽어서 0과 비교. ( 0보다 작으면 0으로 설정 )
            MyAS->HP.CurrentValue = std::max(0.0f, MyAS->HP.CurrentValue);
        }
    }
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