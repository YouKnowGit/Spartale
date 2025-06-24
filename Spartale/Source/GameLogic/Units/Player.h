#pragma once
#include "Source/Core/Actor.h"
#include "Framework/Inventory/InventoryComponent.h"
#include <string>
#include <memory>

enum class Direction { UP, DOWN, LEFT, RIGHT };

// Actor를 상속받아 Player 클래스를 정의합니다.
class Player : public Actor
{
public:
    Player(std::wstring name);
    virtual ~Player() = default;

    // Actor의 가상 함수들을 재정의(override)
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