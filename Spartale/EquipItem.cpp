#include "EquipItem.h"
#include "Player.h"

void EquipItem::Use(Actor* user) {
    Player* player = dynamic_cast<Player*>(user);
    if (player) {
        player->EquipItems(this);  // EquipItem* ³Ñ±è
    }
}