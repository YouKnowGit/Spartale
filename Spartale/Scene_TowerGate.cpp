#include "Story.h"

void Scene_TowerGate() {
    typeEffect("높이 솟은 돌기둥들이 검게 일그러진 하늘을 향해 뻗어 있다.");
    typeEffect("수페르비아 탑. 죽음의 날개, 데스페라도가 깃든 곳.");
    waitForEnter();

    typeEffect("주인공은 무너진 계단을 올라 탑의 앞에 선다.");
    typeEffect("바람은 거세고, 하늘은 침묵하며 숨을 죽인다.");
    waitForEnter();

    typeEffect("탑의 문은 닫혀 있다. 하지만 묘하게... 기다리고 있는 듯한 기운.");
    typeEffect("그 무엇도 널 막을 수 없다는 듯, 문은 스스로 열리기 시작한다.");
    waitForEnter();

    typeEffect("『여기까지 왔구나... 라일라.』");
    typeEffect("『네 이름을 품고, 여기까지 걸어왔다.』");
    waitForEnter();

    typeEffect("심장은 고요하지만 뜨겁다.");
    typeEffect("그동안 겪은 싸움, 상처, 동료, 그리고... 복수의 불꽃.");
    waitForEnter();

    typeEffect("이 문을 넘어가면, 다시는 돌아오지 못할 수도 있다.");
    typeEffect("그러나 망설임은 없다.");
    waitForEnter();

    typeEffect("『데스페라도... 네가 불태운 모든 것에 값을 치르게 하겠다.』");
    waitForEnter();

    std::cout << "\n=== 수페르비아 탑 입구 ===\n\n";
}
