#pragma once
#include "Source/Core/Actor.h"
#include <string>
#include <memory>

enum class Direction { UP, DOWN, LEFT, RIGHT };

class Player : public Actor
{
public:
    Player(std::wstring name);
    virtual ~Player() = default;

    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Render() override;

    void SetDirection(Direction dir);
    Direction GetDirection() const;

private:
    Direction m_direction;
};