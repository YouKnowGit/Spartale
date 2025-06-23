#include "GameplayEffect.h"

GameplayEffect::GameplayEffect()
{
}

GameplayEffect::~GameplayEffect()
{
}

// 가장 핵심적인 함수
// TargetAttributeName을 기반으로 TargetAttributeSet의 실제 속성을 찾아
// Magnitude만큼 수치를 변경하는 함수
void GameplayEffect::Apply(AttributeSet* TargetAttributeSet) const
{
    // 대상의 AttributeSet 유효성 검사
    if (!TargetAttributeSet)
    {
        return;
    }

    // 1. 맵에서 TargetAttributeName에 해당하는 속성을 find
    auto it = TargetAttributeSet->AttributeMap.find(TargetAttributeName);

	// 2. 속성을 정상적으로 찾았는지 검사
    if (it != TargetAttributeSet->AttributeMap.end())
    {
        // 3. 해당 속성의 현재 값을 Magnitude 만큼 변경
        // it->first는 키("HP"), it->second는 값(HP 데이터의 주소)
        FAttributeData* AttributeToChange = it->second;

        // 변경 전의 값을 기록
        float OldValue = AttributeToChange->CurrentValue;

        AttributeToChange->CurrentValue += Magnitude;

        // AttributeSet에게 "값이 바뀌었으니 후처리" 라고 알려줌
        TargetAttributeSet->PostAttributeChange(*AttributeToChange, OldValue, AttributeToChange->CurrentValue);

    }
}

std::unique_ptr<GameplayEffect> GameplayEffect::CreateInverseEffect() const
{
    // 1. 새로운 GameplayEffect 객체 생성
    auto InverseEffect = std::make_unique<GameplayEffect>();

	// 2. InverseEffect의 속성들을 설정
    InverseEffect->EffectName = this->EffectName + L" (제거)";
    InverseEffect->TargetAttributeName = this->TargetAttributeName;

    // 3. 적용 방식은 즉시 (Instant) 로 처리
    InverseEffect->ApplicationType = EEffectApplication::Instant;

    // 4. 변경 수치(Magnitude) 를 정반대로 설정
    InverseEffect->Magnitude = -this->Magnitude;

    // 5. 완성된 반대 효과를 반환
    return InverseEffect;
}