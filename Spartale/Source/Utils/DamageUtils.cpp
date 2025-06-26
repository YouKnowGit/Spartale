#include "Utils/DamageUtils.h"
#include "Core/Actor.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/AttributeSet.h"

#include <random>

namespace DamageUtils
{
    float CalculateDamage(const Actor* SourceActor, const Actor* TargetActor, float ADRatio, float APRatio, EDamageType DamageType)
    {
        if (!SourceActor || !TargetActor) return 0.0f;

        static std::random_device rd;  // 예측 불가능한 시드값을 생성
        static std::mt19937 gen(rd()); // 시드값으로 메르센 트위스터 엔진을 초기화

        const AttributeSet* SourceStats = SourceActor->GetAbilityComponent()->GetAttributeSet();
        const AttributeSet* TargetStats = TargetActor->GetAbilityComponent()->GetAttributeSet();

        float Damage = 0.0f;

        // 데미지 타입에 따라 계산 방식을 분기
        switch (DamageType)
        {
        case EDamageType::Physical:
            // 물리 데미지 = (힘 * 물리 계수) - (대상의 방어력)
            Damage = SourceStats->Strength.CurrentValue * ADRatio;
            Damage -= TargetStats->Defence.CurrentValue;
            break;

        case EDamageType::Magical:
            // 마법 데미지 = (지능 * 마법 계수) - (대상의 마법 저항력)
            Damage = SourceStats->Intelligence.CurrentValue * APRatio;
            Damage -= TargetStats->MagicResistance.CurrentValue;
            break;

        case EDamageType::True:
            // 고정 데미지 = (힘 * 물리 계수) 또는 (지능 * 마법 계수). 방어/저항 무시 (현재 구현 X)
            break;

        case EDamageType::None:
        default:
            break;
        }

        // 치명타 계산 로직
        const float CritChance = SourceStats->CriticalHitChance.CurrentValue;
        const float CritMultiplier = SourceStats->CriticalHitDamageMultiplier.CurrentValue;

        if (Damage > 0 && CritChance > 0)
        {
            static std::uniform_real_distribution<float> crit_distrib(0.0f, 100.0f);
            if (crit_distrib(gen) < CritChance) Damage *= CritMultiplier;
        }

        // 0.95f ~ 1.15f 사이의 실수를 균등하게 생성하는 분포를 정의 (-15% ~ +15% 편차)
        static std::uniform_real_distribution<float> distrib(0.95f, 1.15f);
        // 최종 데미지에 랜덤 배율을 곱합니다.
        if (Damage > 0)
        {
           Damage *= distrib(gen);
        }

        // 최소 데미지는 1로 설정
        if (Damage < 1.f)
        {
            Damage = 1.f;
        }

        return Damage;
    }
}