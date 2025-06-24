#pragma once
#include "Source/Core/Actor.h"

#include <string>
#include <random>

// Actor를 상속받아 Monster 클래스를 정의
class Monster : public Actor
{
public:
    // 몬스터의 종류에 따라 다른 이름과 스탯을 가질 수 있도록 생성자에서 값을 받음
    Monster(const std::wstring& InName, float InHP, float InStrength, float InDefence, int level = 1);
    virtual ~Monster() = default;

    std::wstring RunAI(Actor* Target);

    // 몬스터는 Actor의 기본 Initialize, Update, Render를 그대로 사용
    // 몬스터만의 특별한 로직이 필요하다면 여기서도 재정의하여 처리

private:
    // 난수용 변수
    std::mt19937 m_rng;
};