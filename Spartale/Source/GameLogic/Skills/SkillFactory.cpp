#include "GameLogic/Skills/SkillFactory.h"
#include "GameLogic/DataManager.h"
#include "GenericAbility.h"

std::unique_ptr<GameplayAbility> SkillFactory::CreateSkill(const std::string& skillId)
{
    const SkillData* data = DataManager::GetInstance().GetSkillData(skillId);
    if (!data)
    {
        return nullptr;
    }

    // 대부분의 스킬은 GenericAbility로 생성
    // 조금 특이한 스킬이 필요할 때만 여기에 else if 분기를 추가 예정
    auto skill = std::make_unique<GenericAbility>();

    if (skill)
    {
        // 생성된 스킬 객체에 JSON 데이터를 주입
        skill->InitializeFromData(data);
    }

    return skill;
}