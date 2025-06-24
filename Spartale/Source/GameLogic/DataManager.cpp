#include "GameLogic/DataManager.h"
#include <fstream>
#include <json.hpp> // JSON 라이브러리
#include <Windows.h>

using json = nlohmann::json;

void DataManager::LoadMonsterData(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        return;
    }

    json data;
    try {
        data = json::parse(file);
    }
    catch (json::parse_error& e) {
        // JSON 파일 형식에 문제가 있을 경우 예외 처리
        file.close();
        return;
    }
    file.close();


    for (auto& element : data.items())
    {
        const std::string& id = element.key();
        const json& monsterJson = element.value();

        MonsterData monsterData;

        // UTF-8(string)을 유니코드(wstring)로 변환
        std::string utf8Name = monsterJson["name"];
        int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8Name.c_str(), -1, NULL, 0);
        if (wlen > 0) {
            wchar_t* wstr = new wchar_t[wlen];
            MultiByteToWideChar(CP_UTF8, 0, utf8Name.c_str(), -1, wstr, wlen);
            monsterData.name = wstr;
            delete[] wstr;
        }

        // 속성에 관한 데이터 추출. 뽑아오는데 값이 없으면 오른쪽 값을 사용
        monsterData.baseLevel = monsterJson.value("level", 1);
        monsterData.baseHp = monsterJson.value("hp", 50.f);
        monsterData.baseHp = monsterJson.value("mp", 50.f);
        monsterData.baseStrength = monsterJson.value("strength", 5.f);
        monsterData.baseDefence = monsterJson.value("defence", 0.f);
        monsterData.baseAgility = monsterJson.value("agility", 5.f);
        monsterData.baseIntelligence = monsterJson.value("intelligence", 5.f);
        monsterData.baseMagicResistance = monsterJson.value("magicResistance", 0.f);


        if (monsterJson.contains("skills"))
        {
            // 3. 스킬 목록을 순회하는 부분도 더 안전한 방식으로 수정
            for (const json& skillIdJson : monsterJson["skills"])
            {
                // .get<string>() 을 통해 명시적으로 타입을 변환하여 오류 방지
                monsterData.skillIds.push_back(skillIdJson.get<std::string>());
            }
        }
        m_monsterDatabase[id] = monsterData;
    }
}

const MonsterData* DataManager::GetMonsterData(const std::string& monsterId) const
{
    auto it = m_monsterDatabase.find(monsterId);
    if (it != m_monsterDatabase.end())
    {
        return &it->second;
    }
    return nullptr;
}