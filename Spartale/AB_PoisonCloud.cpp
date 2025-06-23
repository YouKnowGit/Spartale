#include "AB_PoisonCloud.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "Actor.h"
#include "AttributeSet.h"
#include "DamageUtils.h"
#include <string>
#include <memory>

AB_PoisonCloud::AB_PoisonCloud()
{
    // 어빌리티 기본 정보 설정
    AbilityName = L"독 안개";
    AbilityDescription = L"대상에게 3턴간 지속되는 독 피해를 입힙니다.";
    ManaCost = 15.0f;

    MyDamageType = EDamageType::Magical; // 이 스킬은 '물리' 타입
    AD_Ratio = 0.0f; // EDamageType::Magical 타입은 AD를 사용하지 않으므로 0으로 설정
    AP_Ratio = 0.25f; // 마법 피해량 계수

    // 스킬 세부 설정값 초기화
    PoisonDuration = 3;
}

std::wstring AB_PoisonCloud::ActivateAbility(AbilitySystemComponent* SourceASC, Actor* Target)
{
    // 시전자와 대상이 유효한지 확인
    if (!SourceASC || !Target)
    {
        return L"대상이 없어 공격할 수 없습니다.";
    }

    const Actor* SourceActor = SourceASC->GetOwnerActor();
    AbilitySystemComponent* TargetASC = Target->GetAbilityComponent();

    if (!TargetASC)
    {
        return L"대상이 효과를 받을 수 없습니다.";
    }
    // 스킬이 턴당 얼마의 데미지를 줄지 계산
    float DotDamage = DamageUtils::CalculateDamage(SourceActor, Target, this->AD_Ratio, this->AP_Ratio, this->MyDamageType);

    // [효과를 생성] 하고 계산된 데미지(DotDamage)를 Magnitude로 설정 (힙에 할당)
    auto PoisonEffect = std::make_unique<GameplayEffect>();
    PoisonEffect->EffectName = L"독 피해";
    PoisonEffect->ApplicationType = EEffectApplication::Duration;
    PoisonEffect->Duration = this->PoisonDuration;
    PoisonEffect->bExecuteOnTurn = true;
    PoisonEffect->TargetAttributeName = "HP";
    PoisonEffect->Magnitude = -DotDamage;
    PoisonEffect->SourceActor = SourceASC->GetOwnerActor(); // 시전자 누구인지 기록

    TargetASC->ApplyGameplayEffectToSelf(std::move(PoisonEffect));

    // 로그 반환
    std::wstring LogMessage = SourceActor->Name + L"이(가) "
        + AbilityName + L"을(를) 시전했습니다!";

    return LogMessage;
}