#pragma once
#include <string>
#include <vector>
#include <map>
#include <json.hpp> 
#include "Core/Global.h" // FMapLocation 

struct EffectData
{
    std::string applyTo = "Target";
    std::string effectName;
    EEffectApplication applicationType = EEffectApplication::Instant;
    std::string targetAttribute;
    int duration = 0;
    bool executeOnTurn = false;

    // Magnitude 계산 정보
    std::string magnitudeCalculation = "Flat"; // "Flat" or "Ratio"
    float baseValue = 0.0f;
    float adRatio = 0.0f;
    float apRatio = 0.0f;
    EDamageType damageType = EDamageType::None;
    bool isHeal = false;
};

struct MonsterData
{
    // Monster 들의 기본 Base 속성을 담을 구조체
    std::wstring name;
    std::string soundId;
    int baseLevel;
    float baseHp;
    float baseMp;
    float baseStrength;
    float baseDefence;
    float baseAgility;
    float baseIntelligence;
    float baseMagicResistance;
    std::vector<std::string> skillIds;
};

struct SkillData
{
    // Skill 들의 기본 정보를 담을 구조체
    std::string id;
    std::string soundId;
    std::wstring name;
    std::wstring description;
    float manaCost = 0.0f;
    std::vector<EffectData> effects;
};


class DataManager
{
public:
    // 싱글턴 인스턴스를 가져오는 함수
    static DataManager& GetInstance()
    {
        static DataManager instance;
        return instance;
    }

    // 데이터 로드 함수
    void LoadMonsterData(const std::string& path);
    void LoadSkillData(const std::string& path);

    // 몬스터, 스킬 데이터를 가져오는 함수
    const MonsterData* GetMonsterData(const std::string& monsterId) const;
    const SkillData* GetSkillData(const std::string& skillId) const;



private:
    // 생성자를 private으로 막아서 외부 생성을 방지
    DataManager() = default;
    ~DataManager() = default;

    // 복사 및 대입 X
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    // 몬스터, 스킬 데이터베이스
    std::map<std::string, MonsterData> m_monsterDatabase;
    std::map<std::string, SkillData> m_skillDatabase;
};