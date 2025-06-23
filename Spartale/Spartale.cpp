#include <iostream>
#include <io.h>
#include <fcntl.h>
#include "Sword.h"  // 강철검 장비 정의
#include "Player.h"

int main() {
    // 유니코드 출력 설정 (한글 출력용)
    _setmode(_fileno(stdout), _O_U16TEXT);

    // 플레이어 생성 및 초기화
    Player player;
    // 스탯 출력 전 초기 상태
    wcout << L"초기 능력치:\n";
    player.DisplayStats();

    // 강철검 생성 (Sword는 EquipItem을 상속받고 내부에 GameplayEffect 포함)
    Sword* steelSword = new Sword();

    // 장착
    player.EquipItems(steelSword);

    wcout << L"\n장착 후 능력치:\n";
    player.DisplayStats();
    player.DisplayEquipment();

    // 탈착
    player.Unequip(EquipSlotType::RightHand);

    wcout << L"\n탈착 후 능력치:\n";
    player.DisplayStats();
    player.DisplayEquipment();
    delete steelSword;
    return 0;
}