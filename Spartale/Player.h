#pragma once
#include "Actor.h"

// Actor�� ��ӹ޾� Player Ŭ������ �����մϴ�.
class Player : public Actor
{
public:
    Player();
    virtual ~Player() = default;

    // Actor�� ���� �Լ����� ������(override)
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Render() override;
};