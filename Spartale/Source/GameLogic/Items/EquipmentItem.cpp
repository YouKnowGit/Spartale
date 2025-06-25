#include "EquipmentItem.h"
#include "GameLogic/Units/Player.h"

bool EquipmentItem::use(Player* targetPlayer, int sourceSlotIndex)
{
    if (!targetPlayer)
    {
        return false;
    }

    // Player에 장착 상태인지 물어보고, 그에 따라 Equip 또는 Unequip 함수 호출
    if (targetPlayer->IsSlotEquipped(sourceSlotIndex))
    {
        targetPlayer->Unequip(sourceSlotIndex);
    }
    else
    {
        targetPlayer->Equip(sourceSlotIndex);
    }

    // 장비는 사용해도 사라지지 않으므로 false 반환
    return false;
}