#pragma once

#include <string>
#include <vector>
#include <memory> 
#include "Global.h"

class AbilitySystemComponent;
class GameplayEffect;
class Actor;

// ��ų�� �⺻ Ʋ�� �Ǵ� �߻� Ŭ����
class GameplayAbility
{
public:
    GameplayAbility();
    virtual ~GameplayAbility() = default;

    // ��ų �̸� �� ����
    std::wstring AbilityName;
    std::wstring AbilityDescription;

    // ��� ��� �� ��Ÿ��
    float ManaCost = 0.0f;
    int Cooldown = 0; // �� ���� (���� ���� X)
    int CurrentCooldown = 0;

    // ��ų ���
    float AD_Ratio = 0.0f; // ���� ���
    float AP_Ratio = 0.0f; // ���� ���

    // �� �����Ƽ�� ������ �����÷��� ����Ʈ ��� (�����Ƽ�� ����Ʈ ��ü���� '����')
	// ex) ���ݷ� ����, ���� ���� ��
    std::vector<std::unique_ptr<GameplayEffect>> EffectsToApply;

    // ���� �Լ�: �� �����Ƽ�� ����� �� �ִ��� Ȯ��
	// ex) ������ �������, ��Ÿ���� �������� ��
    virtual bool CanActivateAbility(AbilitySystemComponent* SourceASC) const;

    // ���� ���� �Լ�: �����Ƽ�� ������ �ߵ���Ű�� �Լ�
	// SourceASC: ������(ASC), Target: ���(Actor)
	// ��ȯ���� �����Ƽ �ߵ� ��� �޽��� (����/���� ��)
	// ����ڸ� Actor�� �޴� ������, �����ڿ� ���� �ٲ� �� �ִ� ����� �پ��ϱ� ����
    // �� ����� �� �����ϼ���.
	// ����: ActivateAbility(m_player->GetGetAbilityComponent(), Goblin);
	// ��ų ���� ��(�Ļ� Ŭ���� ���� ��) �ݵ�� �����ؾ� ��
    virtual std::wstring ActivateAbility(AbilitySystemComponent* SourceASC, Actor* Target) = 0;

protected:
    // �����Ƽ�� ������ �� �ʿ��� ����Ʈ���� �����ϴ� ���� �Լ�
    virtual void SetupEffects() = 0;
};