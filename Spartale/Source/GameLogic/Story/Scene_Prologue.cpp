#include <thread>
#include <chrono>
#include "Story.h"

void Scene_Prologe() {
    typeEffect("검은 연기와 피비린내가 뒤섞인 전장의 심장부.");
    typeEffect("전투의 함성과 절규가 얽히며, 칼날과 화살이 하늘을 가른다.");
    waitForEnter();

    typeEffect("너는 라일라와 나란히 싸우고 있었다.");
    typeEffect("그녀의 검은 적의 창을 튕겨냈고, 너의 방패는 그녀의 등을 지켰다.");
    waitForEnter();

    typeEffect("『조금만 더야... 우리가 끝낸다.』");
    typeEffect("라일라의 목소리는 단호했고, 너는 고개를 끄덕였다.");
    waitForEnter();

    typeEffect("승리는 손에 잡힐 듯했다. 적은 물러나고 있었고,");
    typeEffect("전선은 너희의 것으로 보였다.");
    waitForEnter();

    typeEffect("그러나 그 순간—");
    typeEffect("하늘이 갈라지는 굉음과 함께, 검은 날개가 전장을 덮었다.");
    waitForEnter();

    typeEffect("『...데스페라도』");
    typeEffect("속삭이듯 흘러나온 누군가의 절망 섞인 음성.");
    waitForEnter();

    typeEffect("고대의 재앙, 죽음의 날개.");
    typeEffect("그 거대한 드래곤이 하늘을 가르며 내려앉았다.");
    waitForEnter();

    typeEffect("불꽃이 쏟아졌다. 대지가 갈라지고 병사들이 증발하듯 사라졌다.");
    typeEffect("너는 라일라를 향해 외쳤지만, 그녀는 이미—");
    waitForEnter();

    typeEffect("붉은 불길 한가운데에서, 라일라의 실루엣이 서서히 무너졌다.");
    typeEffect("『라일라아아!!』");
    waitForEnter();

    typeEffect("네 몸도 불타올랐다.");
    typeEffect("검은 녹았고, 방패는 산산조각 났으며, 살갗이 찢겨 나갔다.");
    waitForEnter();

    typeEffect("의식은 붕 떠오른 듯 멀어져갔고, 소리는 하나둘 사라졌다.");
    typeEffect("검은 하늘과 붉은 땅만이 시야에 남은 채—");
    waitForEnter();

    typeEffect("너는 무너졌다.");
    waitForEnter();

    std::cout << "\n=== 프롤로그 끝 ===\n\n";
}
