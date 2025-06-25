#pragma once

#include <string>
#include <vector>
#include <memory> 
#include "Core/Global.h"
#include "GameLogic/DataManager.h"

class AbilitySystemComponent;
class GameplayEffect;
class Actor;
class DataManager;

// 스킬의 기본 틀이 되는 추상 클래스
class GameplayAbility
{
public:
    GameplayAbility();
    virtual ~GameplayAbility();

    // 스킬 이름 및 설명
    std::wstring AbilityName;
    std::wstring AbilityDescription;
    std::string ABID;

    // 사용 비용 및 쿨타임
    float ManaCost = 0.0f;
    int Cooldown = 0; // 턴 단위 (아직 구현 X)
    int CurrentCooldown = 0;

    // 스킬 계수
    float AD_Ratio = 0.0f; // 물리 계수
    float AP_Ratio = 0.0f; // 마법 계수

    EDamageType DamageType = EDamageType::None; // 데미지 타입 (물리, 마법 등)

    // 사운드 이름
    std::string soundId;

    // 이 어빌리티가 적용할 게임플레이 이펙트 목록 (어빌리티가 이펙트 객체들을 '소유')
    // ex) 공격력 증가, 방어력 감소 등

    std::vector<std::unique_ptr<GameplayEffect>> EffectsToApply;

    // 가상 함수: 이 어빌리티를 사용할 수 있는지 확인
    // ex) 마나가 충분한지, 쿨타임이 끝났는지 등
    virtual bool CanActivateAbility(AbilitySystemComponent* SourceASC) const;

    // 순수 가상 함수: 어빌리티를 실제로 발동시키는 함수
    // SourceASC: 시전자(ASC), Target: 대상(Actor)
    // 반환값은 어빌리티 발동 결과 메시지 (성공/실패 등)
    // 대상자를 Actor로 받는 이유는, 시전자에 비해 바뀔 수 있는 대상이 다양하기 때문
    // ※ 사용할 때 유의하세요.
    // 예시: ActivateAbility(m_player->GetGetAbilityComponent(), Goblin);
    // 스킬 만들 때(파생 클래스 제작 시) 반드시 구현해야 함
    virtual std::wstring ActivateAbility(AbilitySystemComponent* SourceASC, Actor* Target) = 0;

    virtual void InitializeFromData(const SkillData* data);

    const std::string& GetID() const { return ABID; }

protected:
    // 어빌리티가 생성될 때 필요한 이펙트들을 설정하는 헬퍼 함수
    virtual void SetupEffects() = 0;
};