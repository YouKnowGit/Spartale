#pragma once
#include "Actor.h"

#include <string>

// Actor�� ��ӹ޾� Monster Ŭ������ ����
class Monster : public Actor
{
public:
    // ������ ������ ���� �ٸ� �̸��� ������ ���� �� �ֵ��� �����ڿ��� ���� ����
    Monster(const std::wstring& InName, float InHP, float InStrength, float InDefence);
    virtual ~Monster() = default;

    std::wstring RunAI(Actor* Target);

    // ���ʹ� Actor�� �⺻ Initialize, Update, Render�� �״�� ���
    // ���͸��� Ư���� ������ �ʿ��ϴٸ� ���⼭�� �������Ͽ� ó��
};