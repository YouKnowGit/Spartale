#include "GameLogic/Skills/GenericAbility.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/GameplayEffect.h"
#include "Core/Actor.h"
#include "Utils/DamageUtils.h"
#include "Utils/StringUtils.h"
#include "GameLogic/DataManager.h"

#include <string>

#pragma comment(lib, "winmm.lib")

void GenericAbility::InitializeFromData(const SkillData* data)
{
    // 부모 클래스의 초기화 함수를 호출해 이름, 마나 소모량 등을 설정
    GameplayAbility::InitializeFromData(data);

    // 이 스킬이 사용할 데이터 전체에 대한 포인터를 저장
    m_SkillData = data;
}

std::wstring GenericAbility::ActivateAbility(AbilitySystemComponent* SourceASC, Actor* Target)
{
    if (!m_SkillData || !SourceASC)
    {
        return L"스킬 데이터가 없습니다.";
    }

    Actor* sourceActor = SourceASC->GetOwnerActor();
    std::wstring finalLogMessage = L""; // 로그 누적을 위해 빈 문자열로 초기화

    // JSON에 정의된 'effects' 배열을 순회
    for (const auto& effectData : m_SkillData->effects)
    {
        // 실제 효과를 적용할 대상(Actor)을 결정 (Self 가 아닐 경우는 Target 으로 설정)
        Actor* actualTarget = nullptr;
        if (effectData.applyTo == "Self")
        {
            actualTarget = sourceActor;
        }
        else 
        {
            actualTarget = Target;
        }

        if (!actualTarget) continue; // 대상이 없으면 이 효과는 건너뜁니다.

        // 효과의 강도(Magnitude)를 계산
        float finalMagnitude = 0.0f;
        if (effectData.magnitudeCalculation == "Ratio")
        {
            finalMagnitude = DamageUtils::CalculateDamage(sourceActor, actualTarget, effectData.adRatio, effectData.apRatio, effectData.damageType);
        }
        else // "Flat" 또는 그 외
        {
            finalMagnitude = effectData.baseValue;
        }

        // 로그 메시지를 생성
        if (effectData.magnitudeCalculation == "Ratio")
        {
            // 데미지 또는 비율 기반 힐 로그
            finalLogMessage += actualTarget->Name + L"이(가) "
                + StringUtils::ConvertToWstring(effectData.effectName) + L" 효과로 "
                + std::to_wstring(static_cast<int>(finalMagnitude))
                + (effectData.isHeal ? L"을(를) 회복했습니다.\n" : L"의 피해를 입었습니다.\n");
        }
        else if (effectData.magnitudeCalculation == "Flat")
        {
            // 고정 수치 기반 버프/디버프 로그
            finalLogMessage += actualTarget->Name + L"의 "
                + StringUtils::ConvertToWstring(effectData.targetAttribute) + L"이(가) "
                + std::to_wstring(static_cast<int>(abs(finalMagnitude))) // Log 에는 양수로 표시되어야함
                + (finalMagnitude >= 0 ? L" 만큼 증가했습니다.\n" : L" 만큼 감소했습니다.\n");
        }

        // isHeal 플래그에 따라 최종 Magnitude 부호를 결정 (힐 관련 내용은 양수, 그 외의 경우는 음수)
        if (!effectData.isHeal)
        {
            finalMagnitude *= -1;
        }

        // GameplayEffect 객체를 생성하고, effectData로 속성을 채움
        auto gameplayEffect = std::make_unique<GameplayEffect>();

        // string -> wstring 변환
        gameplayEffect->EffectName = StringUtils::ConvertToWstring(effectData.effectName);

        gameplayEffect->ApplicationType = effectData.applicationType;
        gameplayEffect->TargetAttributeName = effectData.targetAttribute;
        gameplayEffect->Duration = effectData.duration;
        gameplayEffect->bExecuteOnTurn = effectData.executeOnTurn;
        gameplayEffect->SourceActor = sourceActor;
        gameplayEffect->Magnitude = finalMagnitude;

        // 최종 대상에게 효과를 적용
        actualTarget->GetAbilityComponent()->ApplyGameplayEffectToSelf(std::move(gameplayEffect)); 
        std::wstring castSoundPath = L"Sounds/Skills/" + StringUtils::ConvertToWstring(this->soundId) + L"_sound.wav";
        PlaySound(castSoundPath.c_str(), NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOSTOP);

    }

    // 만약 생성된 로그가 비어있다면(시전 효과만 있는 스킬 등), 기본 시전 메시지를 사용
    if (finalLogMessage.empty())
    {
        finalLogMessage = sourceActor->Name + L"이(가) " + this->AbilityName + L"을(를) 시전했습니다!";
    }

    return finalLogMessage;
}