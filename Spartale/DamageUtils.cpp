#include "DamageUtils.h"
#include "Actor.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"

#include <random>

namespace DamageUtils
{
    float CalculateDamage(const Actor* SourceActor, const Actor* TargetActor, float ADRatio, float APRatio, EDamageType DamageType)
    {
        if (!SourceActor || !TargetActor) return 0.0f;

        const AttributeSet* SourceStats = SourceActor->GetAbilityComponent()->GetAttributeSet();
        const AttributeSet* TargetStats = TargetActor->GetAbilityComponent()->GetAttributeSet();

        if (!SourceStats || !TargetStats) return 0.0f;

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

        // 최소 데미지는 1로 설정
        if (Damage < 1.f)
        {
            Damage = 1.f;
        } 
        
        {
            // 데미지 편차를 위한 난수 발생 구간
            // static으로 선언하여, 이 함수가 처음 호출될 때 단 한 번만 생성
            static std::random_device rd;  // 예측 불가능한 시드값을 생성
            static std::mt19937 gen(rd()); // 시드값으로 메르센 트위스터 엔진을 초기화

            // 0.95f ~ 1.15f 사이의 실수를 균등하게 생성하는 분포를 정의 (-15% ~ +15% 편차)
            static std::uniform_real_distribution<float> distrib(0.95f, 1.15f);

            // 최종 데미지에 랜덤 배율을 곱합니다.
            Damage *= distrib(gen);
        }

        return Damage;
    }
}