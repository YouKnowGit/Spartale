#pragma once
#include <string>
#include <memory>

class GameplayAbility;

class SkillFactory
{
public:
    static std::unique_ptr<GameplayAbility> CreateSkill(const std::string& skillId);
};