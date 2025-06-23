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
{// 대상의 AttributeSet 유효성 검사
    if (!TargetAttributeSet)
    {
        return;
    }

    // 이펙트 적용 방식에 따라 로직을 분기
    // 현재는 즉시 적용(Instant)만 구현
    switch (ApplicationType)
    {
    case EEffectApplication::Instant:
    {
        // TargetAttributeName 문자열을 비교하여 어떤 속성을 변경할지 결정
        if (TargetAttributeName == L"HP")
        {
            TargetAttributeSet->HP.CurrentValue += Magnitude;
        }
        else if (TargetAttributeName == L"MP")
        {
            TargetAttributeSet->MP.CurrentValue += Magnitude;
        }
        else if (TargetAttributeName == L"Strength")
        {
            TargetAttributeSet->Strength.CurrentValue += Magnitude;
        }
        else if (TargetAttributeName == L"Agility")
        {
            TargetAttributeSet->Agility.CurrentValue += Magnitude;
        }
        // ... ( 추가적인 속성들에 대해 처리하려면 여기에 else if 문을 추가하세용 ) ...
        else if (TargetAttributeName == L"Gold")
        {
            TargetAttributeSet->Gold.CurrentValue += Magnitude;
        }

        break;
    }

    case EEffectApplication::Duration:
    {
		// 지속 효과 (독, 버프 등)
        break;
    }

    case EEffectApplication::Infinite:
    {
        // TODO: 영구 효과 로직 구현
        // 영구적인 장비 아이템의 스탯 보너스 등에 사용
        break;
    }
    }
}
void GameplayEffect::Remove(AttributeSet* TargetAttributeSet) const
{
    if (!TargetAttributeSet) return;

    // 이펙트 적용 방식에 따라 로직을 분기
    switch (ApplicationType)
    {
    case EEffectApplication::Instant:
    case EEffectApplication::Infinite:
    {
        if (TargetAttributeName == L"HP")
        {
            TargetAttributeSet->HP.CurrentValue -= Magnitude;
        }
        else if (TargetAttributeName == L"MP")
        {
            TargetAttributeSet->MP.CurrentValue -= Magnitude;
        }
        else if (TargetAttributeName == L"Strength")
        {
            TargetAttributeSet->Strength.CurrentValue -= Magnitude;
        }
        else if (TargetAttributeName == L"Agility")
        {
            TargetAttributeSet->Agility.CurrentValue -= Magnitude;
        }
        else if (TargetAttributeName == L"Gold")
        {
            TargetAttributeSet->Gold.CurrentValue -= Magnitude;
        }
        break;
    }

    case EEffectApplication::Duration:
        // TODO: 지속 효과 제거 로직
        break;
    }
}