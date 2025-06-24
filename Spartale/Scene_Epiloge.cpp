#include "Story.h"

void Scene_Epiloge() {
    typeEffect("검은 날개가 탑 정상에 부서져 내린다.");
    typeEffect("죽음의 포효는 더 이상 울려 퍼지지 않는다.");
    waitForEnter();

    typeEffect("데스페라도.");
    typeEffect("라일라의 생을 앗아간 존재, 너의 모든 분노가 향하던 그 이름.");
    waitForEnter();

    typeEffect("그 거대한 심장이 멎고, 탑은 서서히 무너져간다.");
    typeEffect("불꽃과 먼지 속에서, 너는 그대로 무릎을 꿇었다.");
    waitForEnter();

    typeEffect("『...끝났다.』");
    waitForEnter();

    typeEffect("하늘을 올려다본다. 검게 물든 하늘이 조금은 맑아진 듯했다.");
    typeEffect("그곳엔 누군가의 미소가 어렴풋이 떠오른다.");
    waitForEnter();

    typeEffect("『라일라... 잘 가.』");
    waitForEnter();

    typeEffect("탑은 무너지고, 잿더미가 되어 바람 속으로 흩어진다.");
    typeEffect("그리고 그 잿더미 속에서, 네 발자국만이 남는다.");
    waitForEnter();

    typeEffect("몇 주 후, 스페스 마을.");
    typeEffect("장로는 언덕을 바라보며 말했다.");
    typeEffect("『그대는... 여전히 길 위에 있는가.』");
    waitForEnter();

    typeEffect("그 검은 여전히 주인과 함께 살아 있었다.");
    typeEffect("이제 복수는 끝났지만, 정의는 아직 길 위에 있다.");
    waitForEnter();

    std::cout << "\n=== THE END ===\n\n";
}
