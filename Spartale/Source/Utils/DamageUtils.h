#pragma once
#include "Core/Global.h" // EDamageType

// 전방 선언
class Actor;

namespace DamageUtils
{
    // 모든 데미지 계산을 처리하는 통합 static 함수
    // Actor 포인터를 직접 받아 필요한 모든 스탯에 접근 가능
    float CalculateDamage(const Actor* SourceActor,
        const Actor* TargetActor,
        float ADRatio,
        float APRatio,
        EDamageType DamageType);
}