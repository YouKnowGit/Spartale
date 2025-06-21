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

    // Getter
    AbilitySystemComponent* GetAbilityComponent() const { return AbilityComponent.get(); }

protected:
    int ActorID;
};