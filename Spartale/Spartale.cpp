#include <iostream>
#include <io.h>
#include <fcntl.h>

#include "Player.h"
#include "Equipment.h"
#include "GameplayEffect.h"
#include "EquipSlotType.h"

int main() {
    // 유니코드 출력 설정 (한글 출력용)
    _setmode(_fileno(stdout), _O_U16TEXT);

    // 플레이어 생성 및 초기화
    Player player;

    // 장비 이펙트 생성: 힘 +5
    GameplayEffect swordEffect;
    swordEffect.EffectName = L"강화된 힘";
    swordEffect.ApplicationType = EEffectApplication::Instant;
    swordEffect.TargetAttributeName = "Strength";
    swordEffect.Magnitude = 5.0f;

    // 장비 생성: 오른손 무기
    Equipment sword(L"강철 검", EquipSlotType::RightHand, swordEffect);

    // 장착
    player.Equip(&sword);

    // 결과 출력
    std::wcout << L"\n현재 능력치 상태:\n";
    player.DisplayStats();
    player.DisplayEquipment();

    return 0;
}