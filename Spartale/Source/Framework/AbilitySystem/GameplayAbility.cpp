#include "Framework/AbilitySystem/GameplayAbility.h"
#include "Framework/AbilitySystem/GameplayEffect.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "GameLogic/DataManager.h"

GameplayAbility::GameplayAbility()
{
}

GameplayAbility::~GameplayAbility() = default; 

bool GameplayAbility::CanActivateAbility(AbilitySystemComponent* SourceASC) const
{
    AttributeSet* SourceAttributeSet = SourceASC->GetAttributeSet();

    // 유효성 검증
    if (!SourceAttributeSet)
    {
        return false;
    }

    // 마나가 충분한지 확인
    if (SourceAttributeSet->MP.CurrentValue < this->ManaCost)
    {
        return false;
    }

    // . . . 이 외의 스킬 조건을 추가로 검증하여 스킬 가능 여부 판별 . . . (True/False 반환)

    return true;
}

void GameplayAbility::InitializeFromData(const SkillData* data)
{
    if (!data) return;

    // SkillData 에서 속성들을 읽어와 멤버 변수에게 할당
    this->AbilityName = data->name;
    this->AbilityDescription = data->description;
    this->ManaCost = data->manaCost;
    this->soundId = data->soundId;
    this->ABID = data->id;
}