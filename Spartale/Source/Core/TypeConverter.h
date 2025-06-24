#pragma once
#include "Core/Global.h"

#include <string>
#include <map>

// EstatType을 string으로 변환하는 함수
inline std::string StatTypeToString(EStatType Type)
{
    static const std::map<EStatType, std::string> converter = {
        { EStatType::HP, "HP"},
        { EStatType::MP, "MP"},
        { EStatType::STRENGTH, "Strength" },
        { EStatType::AGILITY, "Agility" },
        { EStatType::INTELLIGENCE, "Intelligence" },
        { EStatType::DEFENCE, "Defence" },
        { EStatType::MAGIC_RESISTANCE, "MagicResistance" },
        { EStatType::CRITICAL_HIT_CHANCE, "CriticalHitChance" },
        { EStatType::CRITICAL_HIT_DAMAGE_MULTIPLIER, "CriticalHitDamageMultiplier" },
    };

    auto it = converter.find(Type);

	// 맵에서 해당 타입을 찾아 문자열로 변환, 없으면 빈문자열 반환
	return (it != converter.end()) ? it->second : "";
}
