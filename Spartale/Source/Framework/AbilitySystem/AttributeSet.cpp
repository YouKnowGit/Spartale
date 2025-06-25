#include "Framework/AbilitySystem/AttributeSet.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include <fstream>
#include <string>
#include <sstream> 

AttributeSet::AttributeSet(AbilitySystemComponent* OwnerComp)
	: OwningAbilitySystemComponent(OwnerComp) // 멤버 이니셜라이저를 사용해 OwningAbilitySystemComponent 초기화
{
    // 각 송성의 이름과 메모리 주소를 맵에 등록
    AttributeMap["HP"] = &HP;
    AttributeMap["MP"] = &MP;
    AttributeMap["Strength"] = &Strength;
    AttributeMap["Agility"] = &Agility;
    AttributeMap["Intelligence"] = &Intelligence;
    AttributeMap["Defence"] = &Defence;
    AttributeMap["MagicResistance"] = &MagicResistance;
    AttributeMap["Experience"] = &Experience;
    AttributeMap["CriticalHitChance"] = &CriticalHitChance;
    AttributeMap["CriticalHitDamageMultiplier"] = &CriticalHitDamageMultiplier;

    // . . . ( 새로운 스탯 추가 시 여기에 추가 ) . . .
}

void AttributeSet::PreAttributeChange(const FAttributeData& Attribute, float& NewValue)
{
}

void AttributeSet::PostAttributeChange(const FAttributeData& Attribute, float OldValue, float NewValue)
{
    // 포인터 비교를 통해 어떤 속성이 변경되었는지 확인
    if (&Attribute == &HP)
    {
		// HP 가 변경되었다면, 0 과 BaseHP 사이의 값으로 보정 (음수 방지 및 최대값 제한)
        HP.CurrentValue = std::max(0.0f, HP.CurrentValue);
        HP.CurrentValue = std::min(HP.BaseValue, HP.CurrentValue);
    }
    else if (&Attribute == &MP)
    {
		// MP 가 변경되엇다면, 0 과 BaseMP 사이의 값으로 보정 (음수 방지 및 최대값 제한)
        MP.CurrentValue = std::max(0.0f, MP.CurrentValue);
        MP.CurrentValue = std::min(MP.BaseValue, MP.CurrentValue);
    }
}

void AttributeSet::AdjustDependentAttributes()
{
    // --- 재계산에 사용할 계수 정의 --
    const float BaseHPConstant = 200.0f + (Level * 20.0f - 20.0f);
    const float HPPerStrength = 4.0f;        // 힘 1포인트당 증가하는 최대 HP

    const float BaseMPConstant = 100.0f;
    const float MPPerIntelligence = 6.0f;     // 지능 1포인트당 증가하는 최대 MP


    // 재계산 전의 최대 HP 값을 기억
    float oldMaxHP = HP.BaseValue;
    // 새로운 최대 HP를 계산: (기본 HP) + ((힘 - 초기 힘 30) * 힘당HP)
    HP.BaseValue = BaseHPConstant + ((this->Strength.CurrentValue - 30) * HPPerStrength);
    // 최대치가 증가했다면, 증가한 만큼 현재 HP도 회복
    float hpIncrease = HP.BaseValue - oldMaxHP;
    if (hpIncrease > 0)
    {
        HP.CurrentValue += hpIncrease;
    }
    // 현재 HP가 새로운 최대 HP를 넘지 않도록 보정
    HP.CurrentValue = std::min(HP.CurrentValue, HP.BaseValue);


    // 재계산 전의 최대 MP 값을 기억
    float oldMaxMP = MP.BaseValue;
    // 새로운 최대 MP를 계산: (기본 MP) + ((지능 - 초기 지능 15) * 지능당MP) 
    MP.BaseValue = BaseMPConstant + ((this->Intelligence.CurrentValue - 10) * MPPerIntelligence);

    // 최대치가 증가했다면, 증가한 만큼 현재 MP도 회복
    float mpIncrease = MP.BaseValue - oldMaxMP;
    if (mpIncrease > 0)
    {
        MP.CurrentValue += mpIncrease;
    }
    MP.CurrentValue = std::min(MP.CurrentValue, MP.BaseValue);
}
void AttributeSet::Save(std::ofstream& file) const
{
    file << "[Attributes]\n";

    // int
    file << "Level " << Level << '\n';
    file << "Gold " << Gold << '\n';
    file << "AdditionalStatPoints " << AdditionalStatPoints << '\n';

    // FAttributeData
    file << "Experience " << Experience.BaseValue << " " << Experience.CurrentValue << '\n';
    file << "HP " << HP.BaseValue << " " << HP.CurrentValue << '\n';
    file << "MP " << MP.BaseValue << " " << MP.CurrentValue << '\n';
    file << "Strength " << Strength.BaseValue << " " << Strength.CurrentValue << '\n';
    file << "Agility " << Agility.BaseValue << " " << Agility.CurrentValue << '\n';
    file << "Intelligence " << Intelligence.BaseValue << " " << Intelligence.CurrentValue << '\n';
    file << "Defence " << Defence.BaseValue << " " << Defence.CurrentValue << '\n';
    file << "MagicResistance " << MagicResistance.BaseValue << " " << MagicResistance.CurrentValue << '\n';
    file << "CriticalHitChance " << CriticalHitChance.BaseValue << " " << CriticalHitChance.CurrentValue << '\n';
    file << "CriticalHitDamageMultiplier " << CriticalHitDamageMultiplier.BaseValue << " " << CriticalHitDamageMultiplier.CurrentValue << '\n';


}
void AttributeSet::Load(std::ifstream& file)
{
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())   continue;

        if (line[0] == '[') {
            file.seekg(static_cast<long>(file.tellg()) - static_cast<long>(line.length()) - 1);
            break;
        }

        std::stringstream ss(line);
        std::string key;
        ss >> key;

        if (key == "Level") ss >> Level;
        else if (key == "Gold") ss >> Gold;
        else if (key == "AdditionalStatPoints") ss >> AdditionalStatPoints;
        else if (key == "Experience") ss >> Experience.BaseValue >> Experience.CurrentValue;
        else if (key == "HP") ss >> HP.BaseValue >> HP.CurrentValue;
        else if (key == "MP") ss >> MP.BaseValue >> MP.CurrentValue;
        else if (key == "Strength") ss >> Strength.BaseValue >> Strength.CurrentValue;
        else if (key == "Agility") ss >> Agility.BaseValue >> Agility.CurrentValue;
        else if (key == "Intelligence") ss >> Intelligence.BaseValue >> Intelligence.CurrentValue;
        else if (key == "Defence") ss >> Defence.BaseValue >> Defence.CurrentValue;
        else if (key == "MagicResistance") ss >> MagicResistance.BaseValue >> MagicResistance.CurrentValue;
        else if (key == "CriticalHitChance") ss >> CriticalHitChance.BaseValue >> CriticalHitChance.CurrentValue;
        else if (key == "CriticalHitDamageMultiplier") ss >> CriticalHitDamageMultiplier.BaseValue >> CriticalHitDamageMultiplier.CurrentValue;
        else if (key == "bIsDefending")
        {
            int defendingFlag;
            ss >> defendingFlag;
            bIsDefending = (defendingFlag == 1);
        }
    }

    AdjustDependentAttributes();
}