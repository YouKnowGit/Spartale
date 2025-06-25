#include "GameLogic/DataManager.h"
#include "Utils/StringUtils.h"
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

        // StringUtils::ConverToWstring 함수로 UTF8 -> 유니코드 변환
        monsterData.name = StringUtils::ConvertToWstring(monsterJson.value("name", ""));


        // 속성에 관한 데이터 추출. 뽑아오는데 값이 없으면 오른쪽 값을 사용
        monsterData.baseLevel = monsterJson.value("level", 1);
        monsterData.baseHp = monsterJson.value("hp", 50.f);
        monsterData.baseMp = monsterJson.value("mp", 50.f);
        monsterData.baseStrength = monsterJson.value("strength", 5.f);
        monsterData.baseDefence = monsterJson.value("defence", 0.f);
        monsterData.baseAgility = monsterJson.value("agility", 5.f);
        monsterData.baseIntelligence = monsterJson.value("intelligence", 5.f);
        monsterData.baseMagicResistance = monsterJson.value("magicResistance", 0.f);


        if (monsterJson.contains("skills"))
        {
            // 스킬 목록을 순회
            for (const json& skillIdJson : monsterJson["skills"])
            {
                // .get<string>() 을 통해 명시적으로 타입을 변환하여 오류 방지
                monsterData.skillIds.push_back(skillIdJson.get<std::string>());

            }
        }
        m_monsterDatabase[id] = monsterData;
    }
}
void DataManager::LoadSkillData(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) { return; }

    json data = json::parse(file);
    file.close();

    for (auto& element : data.items())
    {
        const std::string& skillId = element.key();
        const json& skillJson = element.value();

        SkillData skillData;
        skillData.id = skillId;

        // 이름, 설명 등 기본 정보 파싱 (문자열은 UTF8 -> 유니코드 변환)
        skillData.name = StringUtils::ConvertToWstring(skillJson.value("name", ""));
        skillData.description = StringUtils::ConvertToWstring(skillJson.value("description", ""));
        skillData.manaCost = skillJson.value("mana_cost", 0.0f);
        skillData.soundId = skillJson.value("sound_id", "");

        // 'effects' 배열 파싱
        if (skillJson.contains("effects"))
        {
            for (const auto& effectJson : skillJson["effects"])
            {
                EffectData effectData;
                effectData.applyTo = effectJson.value("apply_to", "Target");
                effectData.effectName = effectJson.value("effect_name", "Effect");
                effectData.targetAttribute = effectJson.value("target_attribute", "HP");
                effectData.duration = effectJson.value("duration", 0);
                effectData.executeOnTurn = effectJson.value("execute_on_turn", false);

                // 적용 타입 (string -> enum)
                std::string appTypeStr = effectJson.value("application_type", "Instant");
                if (appTypeStr == "Duration") effectData.applicationType = EEffectApplication::Duration;
                else if (appTypeStr == "Infinite") effectData.applicationType = EEffectApplication::Infinite;
                else effectData.applicationType = EEffectApplication::Instant;

                // Magnitude 정보 파싱
                if (effectJson.contains("magnitude"))
                {
                    const auto& magJson = effectJson["magnitude"];
                    effectData.magnitudeCalculation = magJson.value("calculation", "Flat");
                    effectData.baseValue = magJson.value("base_value", 0.0f);
                    effectData.adRatio = magJson.value("ad_ratio", 0.0f);
                    effectData.apRatio = magJson.value("ap_ratio", 0.0f);
                    effectData.isHeal = magJson.value("is_heal", false);

                    std::string damageTypeStr = magJson.value("damage_type", "None");
                    if (damageTypeStr == "Physical") effectData.damageType = EDamageType::Physical;
                    else if (damageTypeStr == "Magical") effectData.damageType = EDamageType::Magical;
                    else if (damageTypeStr == "True") effectData.damageType = EDamageType::True;
                    else effectData.damageType = EDamageType::None;
                }
                skillData.effects.push_back(effectData);
            }
        }
        m_skillDatabase[skillId] = skillData;
    }
}

// LoadItemData 함수 구현
void DataManager::LoadItemData(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) { return; }
    json data = json::parse(file);
    file.close();

    // '번역기' 람다 함수를 맨 위에 한 번만 정의하여 재사용합니다.
    auto StringToStatType = [](const std::string& s) -> EStatType {
        if (s == "HP") return EStatType::HP;
        if (s == "MP") return EStatType::MP;
        if (s == "STRENGTH") return EStatType::STRENGTH;
        if (s == "DEFENCE") return EStatType::DEFENCE;
        if (s == "AGILITY") return EStatType::AGILITY;
        if (s == "MAGIC_RESISTANCE") return EStatType::MAGIC_RESISTANCE;
        if (s == "INTELLIGENCE") return EStatType::INTELLIGENCE;
        if (s == "CRITICAL_HIT_CHANCE") return EStatType::CRITICAL_HIT_CHANCE;
        if (s == "CRITICAL_HIT_DAMAGE_MULTIPLIER") return EStatType::CRITICAL_HIT_DAMAGE_MULTIPLIER;
        
        /* ---이후 추가될 스탯에 대해서는 여기에 추가--- */

        return EStatType::HP; // 매칭 안될 시 기본값
        };

    for (auto& element : data.items())
    {
        const std::string& itemId = element.key();
        const json& itemJson = element.value();
        ItemData itemData;
        itemData.ID = itemId;

        // 공통 데이터 파싱
        itemData.Name = StringUtils::ConvertToWstring(itemJson.value("name", ""));
        itemData.Description = StringUtils::ConvertToWstring(itemJson.value("description", ""));
        itemData.Price = itemJson.value("price", 0);
        itemData.bIsStackable = itemJson.value("isStackable", false);
        itemData.MaxStackSize = itemJson.value("maxStackSize", 1);

        // 아이템 타입 변환
        std::string typeStr = itemJson.value("type", "");
        if (typeStr == "Consumable") itemData.Type = EItemType::Consumable;
        else if (typeStr == "Equipment") itemData.Type = EItemType::Equipment;
        else itemData.Type = EItemType::Miscellaneous;

        // 타입별 세부 데이터 파싱
        if (itemData.Type == EItemType::Consumable && itemJson.contains("effectData"))
        {
            const auto& effectJson = itemJson["effectData"];
            itemData.EffectData.Type = effectJson.value("effectType", "");
            itemData.EffectData.Value = effectJson.value("value", 0.0f);
            itemData.EffectData.Duration = effectJson.value("duration", 0);
            itemData.EffectData.TargetStat = StringToStatType(effectJson.value("targetStat", ""));
        }
        else if (itemData.Type == EItemType::Equipment && itemJson.contains("equipmentData"))
        {
            const auto& equipJson = itemJson["equipmentData"];
            std::string equipTypeStr = equipJson.value("equipmentType", "");
            if (equipTypeStr == "WEAPON") itemData.EquipmentData.Type = EEquipmentType::WEAPON;
            else if (equipTypeStr == "ARMOR") itemData.EquipmentData.Type = EEquipmentType::ARMOR;
            else if (equipTypeStr == "ACCESSORY") itemData.EquipmentData.Type = EEquipmentType::ACCESSORY;
            else itemData.EquipmentData.Type = EEquipmentType::NOT_EQUIPMENT;

            if (equipJson.contains("statBonuses"))
            {
                for (const auto& bonus : equipJson["statBonuses"].items())
                {
                    itemData.EquipmentData.StatBonuses[StringToStatType(bonus.key())] = bonus.value();
                }
            }
        }
        m_itemDatabase[itemId] = itemData;
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
const SkillData* DataManager::GetSkillData(const std::string& skillId) const
{
    auto it = m_skillDatabase.find(skillId);
    if (it != m_skillDatabase.end())
    {
        return &it->second;
    }
    return nullptr;
}

const ItemData* DataManager::GetItemData(const std::string& itemId) const
{
    auto it = m_itemDatabase.find(itemId);
    if (it != m_itemDatabase.end())
    {
        return &it->second;
    }
    return nullptr;
}