#pragma once

#include <string>
#include <map>
#include "Core/Global.h"

// 장비 아이템의 상세 데이터를 담을 구조체
struct FEquipmentData
{
    EEquipmentType Type = EEquipmentType::NOT_EQUIPMENT;
    std::map<EStatType, float> StatBonuses;
};

// 소모품의 효과 데이터를 담을 구조체
struct FConsumableEffectData
{
    // Global.h에 EConsumableEffectType을 추가하거나, string으로 처리합니다.
    // 여기서는 간단하게 string으로 처리하겠습니다.
    std::string Type; // "HEAL_HP", "BUFF_STAT"
    EStatType   TargetStat = EStatType::HP; // 버프일 경우 대상 스탯
    float       Value = 0.0f;
    int         Duration = 0;
};

// 모든 아이템의 정보를 담는 최종 구조체
struct ItemData
{
    std::string             ID;
    std::wstring            Name;           // wstring으로 변경
    std::wstring            Description;    // wstring으로 변경
    EItemType               Type;
    int                     Price;
    bool                    bIsStackable;
    int                     MaxStackSize;

    // 이제 소모품과 장비 데이터를 모두 담을 수 있습니다.
    FConsumableEffectData   EffectData;
    FEquipmentData          EquipmentData;
};