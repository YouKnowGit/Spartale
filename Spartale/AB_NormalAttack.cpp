#include "AB_NormalAttack.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "Actor.h"
#include "AttributeSet.h"
#include "DamageUtils.h"
#include <map>
#include <string> // std::to_wstring 사용을 위해 포함

AB_NormalAttack::AB_NormalAttack()
{
    // 어빌리티 기본 정보 설정
    AbilityName = L"일반 공격";
    AbilityDescription = L"대상에게 물리 피해를 입힙니다.";
    MyDamageType = EDamageType::Physical; // 이 스킬은 '물리' 타입
    AD_Ratio = 1.0f; // 피해량 계수. 1.0f는 '힘' 능력치의 100%를 피해량으로 사용한다는 의미
	AP_Ratio = 0.0f; // EDamageType::Physical 타입은 AP를 사용하지 않으므로 0으로 설정
}

void AB_NormalAttack::SetupEffects()
{
}

std::wstring AB_NormalAttack::ActivateAbility(AbilitySystemComponent* SourceASC, Actor* Target)
{
    // 시전자와 대상이 유효한지 확인
    if (!SourceASC || !Target)
    {
        return L"대상이 없어 공격할 수 없습니다.";
    }

    const Actor* SourceActor = SourceASC->GetOwnerActor();

    // 1. 데미지 계산을 새로운 DamageUtils 함수에 위임
    float Damage = DamageUtils::CalculateDamage(SourceActor, Target, this->AD_Ratio, this->AP_Ratio, this->MyDamageType);

    // 2. 효과에 대해 정의할 GameplayEffect를 생성
    auto DamageEffect = std::make_unique<GameplayEffect>();
    DamageEffect->EffectName = L"데미지";
    DamageEffect->ApplicationType = EEffectApplication::Instant;
	DamageEffect->TargetAttributeName = "HP";    // 대상의 HP 속성에 적용
    DamageEffect->Magnitude = -Damage;   // HP 피해이므로 음수로 설정

    // 3. 대상에게 효과를 적용
    Target->GetAbilityComponent()->ApplyGameplayEffectToSelf(std::move(DamageEffect));

    // 4. 로그를 반환
    std::wstring LogMessage = SourceActor->Name + L"이(가) "
        + Target->Name + L"에게 "
        + std::to_wstring(static_cast<int>(Damage))
        + L"의 피해를 입혔습니다.";

    return LogMessage;
}