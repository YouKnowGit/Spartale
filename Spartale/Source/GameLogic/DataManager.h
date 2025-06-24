#pragma once
#include <string>
#include <vector>
#include <map>
#include "Core/Global.h" // FMapLocation 

struct MonsterData
{
    // Monster 들의 기본 Base 속성
    std::wstring name;
    int baseLevel;
    float baseHp;
    float baseStrength;
    float baseDefence;
    float baseAgility;
    float baseIntelligence;
    float baseMagicResistance;
    std::vector<std::string> skillIds;
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

    // 몬스터 데이터를 가져오는 함수
    const MonsterData* GetMonsterData(const std::string& monsterId) const;

private:
    // 생성자를 private으로 막아서 외부 생성을 방지
    DataManager() = default;
    ~DataManager() = default;

    // 복사 및 대입 X
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    // 몬스터 데이터베이스
    std::map<std::string, MonsterData> m_monsterDatabase;
};