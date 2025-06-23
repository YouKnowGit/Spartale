#pragma once
#include "Global.h" // EDamageType

// ���� ����
class Actor;

namespace DamageUtils
{
    // ��� ������ ����� ó���ϴ� ���� static �Լ�
    // Actor �����͸� ���� �޾� �ʿ��� ��� ���ȿ� ���� ����
    float CalculateDamage(const Actor* SourceActor, 
                                    const Actor* TargetActor, 
                                    float ADRatio, 
                                    float APRatio, 
                                    EDamageType DamageType);
}