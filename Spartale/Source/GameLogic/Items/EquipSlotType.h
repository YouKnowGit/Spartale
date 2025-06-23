#pragma once

// 캐릭터의 장비 슬롯 종류를 정의한 열거형
// 각각의 값은 캐릭터의 특정 신체 부위를 의미한다.
enum class EquipSlotType
{
    RightHand,   // 오른손 (예: 무기)
    LeftHand,    // 왼손 (예: 방패)
    Chest,       // 가슴 (예: 갑옷)
    Head,        // 머리 (예: 투구)
    Boots,       // 신발
    Neck         // 목 (예: 목걸이)
};