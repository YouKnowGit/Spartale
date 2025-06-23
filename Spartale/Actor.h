#pragma once
#include <string>
#include <vector>
#include <memory> // ����Ʈ ������ ���
#include "Global.h" // Global.h�� FMapLocation ���� ����

// ASC ���� ����
class AbilitySystemComponent;

class Actor
{
public:
    Actor();
    virtual ~Actor() = default;

	// Actor�� ASC(Ability System Component)�� ����. �̸� ���� �ɷ�,���¸� ����
    std::unique_ptr<AbilitySystemComponent> AbilityComponent;

    // �⺻ ���� (�̸� �� ��ġ)
    std::wstring Name;
    FMapLocation CurrentLocation;

    virtual void Initialize() {}
    virtual void Update() {}
    virtual void Render() {}

    virtual void SetPosition(int x, int y);
    virtual int GetX() const;
    virtual int GetY() const;

    // Getter
    AbilitySystemComponent* GetAbilityComponent() const { return AbilityComponent.get(); }

protected:
    int ActorID;
    int m_x;
    int m_y;
};