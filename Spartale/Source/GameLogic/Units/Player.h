#pragma once
#include "Source/Core/Actor.h"
#include "Framework/Inventory/InventoryComponent.h"
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

	InventoryComponent* GetInventory() { return &m_inventory; }

private:
    Direction m_direction;

	InventoryComponent m_inventory; // 플레이어의 인벤토리 컴포넌트
};