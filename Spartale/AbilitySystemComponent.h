#pragma once
#include <vector>
#include <memory>
#include <string>

// 전방 선언
class Actor;
class AttributeSet;
class GameplayAbility;
class GameplayEffect;

class AbilitySystemComponent
{
public:
    AbilitySystemComponent();
    AbilitySystemComponent(Actor* Owner);
    ~AbilitySystemComponent();

    void Initialize();

	// 해당 Owner 가 소유할 스킬 목록: GrantAbility
    void GrantAbility(std::unique_ptr<GameplayAbility> Ability);
    void ClearAbility(GameplayAbility* Ability);

    // 전투 시 사용할 4개의 스킬: EquipAbility
    void EquipAbility(int32_t SlotIndex, GameplayAbility* Ability);
    void UnEquipAbility(int32_t SlotIndex);

	// 스킬 및 아이템 사용
	// TryActivateAbility: 어빌리티를 사용(활성화)하는 함수
	// ApplyGameplayEffectToSelf: 자신에게 게임플레이 효과를 적용하는 함수
    std::wstring TryActivateAbility(int32_t SlotIndex, Actor* Target);
    void ApplyGameplayEffectToSelf(const GameplayEffect* Effect);

	// Getter
    Actor* GetOwnerActor() const { return OwnerActor; }
    AttributeSet* GetAttributeSet() const { return MyAttributeSet.get(); }
    GameplayAbility* GetGrantedAbility(int32_t Index) const;
    const std::vector<GameplayAbility*>& GetEquippedAbilities() const { return EquippedAbilities; }

protected:
    // 이 컴포넌트의 소유자 
    Actor* OwnerActor;

	// 이 컴포넌트가 AttributeSet 소유 (이에 접근해 능력치 계산 및 변경, protected 이므로 Getter 사용)
    std::unique_ptr<AttributeSet> MyAttributeSet;

    // 습득한 모든 어빌리티 목록 (소유한 전체 스킬)
    std::vector<std::unique_ptr<GameplayAbility>> GrantedAbilities;

    // 장착한 어빌리티 목록 (사용자가 정한 4개의 스킬)
    std::vector<GameplayAbility*> EquippedAbilities;
};