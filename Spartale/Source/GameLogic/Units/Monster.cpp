#include "GameLogic/Units/Monster.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/AttributeSet.h"

#include <vector>

Monster::Monster(const std::wstring& InName, float InHP, float InStrength, float InDefence, int level)
{
	// Monster 의 이름을 설정
    Name = InName;

    // 난수용 설정
    std::random_device rd;
    m_rng.seed(rd());

	// Actor 가 보유한 ASC 를 통해 AttributeSet 에 접근
    AttributeSet* MyStats = GetAbilityComponent()->GetAttributeSet();
    if (MyStats)
    {
		// 몬스터의 기본 능력치를 설정합니다.
        MyStats->Level = level;
        MyStats->HP.BaseValue = InHP;
        MyStats->HP.CurrentValue = InHP;

        MyStats->Strength.BaseValue = InStrength;
        MyStats->Strength.CurrentValue = InStrength;

        MyStats->Defence.BaseValue = InDefence;
        MyStats->Defence.CurrentValue = InDefence;
    }
}
std::wstring Monster::RunAI(Actor* Target)
{
    if (!GetAbilityComponent())
    {
        return Name + L"은(는) 무엇을 해야할지 모릅니다...";
    }

    // 사용 가능한 스킬 슬롯 인덱스 목록 생성
    const auto& equippedAbilities = GetAbilityComponent()->GetEquippedAbilities();
    std::vector<int> availableSkillSlots;
    for (int i = 0; i < equippedAbilities.size(); ++i)
    {
        if (equippedAbilities[i] != nullptr)
        {
            availableSkillSlots.push_back(i);
        }
    }

    if (availableSkillSlots.empty())
    {
        return Name + L"이(가) 어리둥절해하며 턴을 넘깁니다.";
    }

    // 사용 가능한 스킬 중 하나를 랜덤하게 선택
    std::uniform_int_distribution<int> dist(0, static_cast<int>(availableSkillSlots.size()) - 1);
    int randomSlotIndex = availableSkillSlots[dist(m_rng)];

    // 선택된 스킬을 사용하고, 결과(FActivationResult)를 받음
    FActivationResult result = GetAbilityComponent()->TryActivateAbility(randomSlotIndex, Target);

    if (!result.bSuccess) 
    {
        // TryActivateAbility 에서 False 가 넘어올 경우 방어 태세를 시전함
        GetAbilityComponent()->GetAttributeSet()->bIsDefending = true;
        result.LogMessage = Name + L"은(는) 방어 태세를 갖췄다!";
    }

    // 결과 구조체에서 로그 메시지만을 반환
    return result.LogMessage;
}