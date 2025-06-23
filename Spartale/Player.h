#pragma once
#include "Actor.h"
#include <string>

enum class Direction { UP, DOWN, LEFT, RIGHT };

// Actor�� ��ӹ޾� Player Ŭ������ �����մϴ�.
class Player : public Actor
{
public:
    Player(std::wstring name);
    virtual ~Player() = default;

    // Actor�� ���� �Լ����� ������(override)
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Render() override;

    void SetDirection(Direction dir);
    Direction GetDirection() const;

private:
    Direction m_direction;
};