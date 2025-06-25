#include "ConsumableItem.h"
#include "GameLogic/Units/Player.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/GameplayEffect.h"
#include "Core/TypeConverter.h"
#include <iostream>
#include <Windows.h>

bool ConsumableItem::CanUse(Player* targetPlayer) const
{
    if (!targetPlayer || !m_pItemData) return false;

    const FConsumableEffectData& effect = m_pItemData->EffectData;
    AttributeSet* stats = targetPlayer->GetAbilityComponent()->GetAttributeSet();
    if (!stats) return false;
	
    // 효과 타입에 따라 다른 사용 조건을 검사합니다.
    if (effect.Type == "HEAL_HP")
    {
        // 현재 HP가 최대 HP보다 적을 때만 true를 반환 (즉, 체력이 꽉 차지 않았을 때만 사용 가능)
        return stats->HP.CurrentValue < stats->HP.BaseValue;
    }

    /* ----다른 제한 효과는 여기에서 추가해주세요.---- */

    return true;
}

bool ConsumableItem::use(Player* targetPlayer, int sourceSlotIndex)
{
    // 사용 대상(플레이어)이나 아이템 데이터가 없으면 실패
    if (!targetPlayer || !m_pItemData)
    {
        return false;
    }

    const FConsumableEffectData& effectData = m_pItemData->EffectData;

    // "HEAL_HP" 효과 처리
    if (effectData.Type == "HEAL_HP")
    {
        // Player에 만들어 두었던 Heal 함수 호출
        targetPlayer->Heal(effectData.Value);
    }
    // "BUFF_STAT" 효과 처리
    else if (effectData.Type == "BUFF_STAT")
    {
        // 1. GameplayEffect '명령서' 생성
        auto effect = std::make_unique<GameplayEffect>();

        // 2. DataManager에서 읽어온 데이터로 명령서 내용 채우기
        effect->EffectName = m_pItemData->Name;
        effect->Duration = effectData.Duration;
        effect->Magnitude = effectData.Value;
        effect->TargetAttributeName = StatTypeToString(effectData.TargetStat); // "Strength" 등
        effect->ApplicationType = EEffectApplication::Duration; // 지속 효과

        // 3. 플레이어의 AbilitySystemComponent에 명령서 전달
        targetPlayer->GetAbilityComponent()->ApplyGameplayEffectToSelf(std::move(effect));
    }

    // 소모품은 항상 사용 시 소모되므로 true 반환
    return true;
}