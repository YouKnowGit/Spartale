#pragma once
#include <string>
#include <vector>
#include <memory> // 스마트 포인터 사용
#include "Global.h" // Global.h는 FMapLocation 정의 포함

// ASC 전방 선언
class AbilitySystemComponent;

class Actor
{
public:
    Actor();
    virtual ~Actor() = default;

    // Actor는 ASC(Ability System Component)를 소유. 이를 통해 능력,상태를 관리
    std::unique_ptr<AbilitySystemComponent> AbilityComponent;

    // 기본 정보 (이름 및 위치)
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
};