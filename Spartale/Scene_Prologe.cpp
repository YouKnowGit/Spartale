//#include <iostream>
//#include <string>
#include <thread>
#include <chrono>
#include "Story.h"

void Scene_Prologe() {
    typeEffect("어둠이 깃든 대륙, 에르덴.");
    typeEffect("수많은 전쟁과 피의 역사 속, 희망이라 불릴 전투가 있었다.");
    waitForEnter();

    typeEffect("그 전쟁의 끝자락, 너는 라일라와 함께 최전선에 서 있었다.");
    typeEffect("그녀는 검보다 강인했고, 너는 그녀의 곁에서 흔들림 없이 싸웠다.");
    waitForEnter();

    typeEffect("승리는 거의 손에 잡힐 듯했다.");
    typeEffect("그러나 그때, 하늘이 갈라지고, 죽음을 머금은 날개가 펼쳐졌다.");
    waitForEnter();

    typeEffect("데스페라도—");
    typeEffect("혹은 '죽음의 날개'라 불리는 고대의 드래곤.");
    typeEffect("전설로만 존재하던 그 괴수가, 홀연히 적의 진영 위에 모습을 드러냈다.");
    waitForEnter();

    typeEffect("하늘에서 쏟아지는 불꽃, 갈라진 대지, 날아오른 검은 비늘.");
    typeEffect("네 곁에 있던 라일라는...");
    typeEffect("그 붉은 불꽃 속으로 사라졌다.");
    waitForEnter();

    typeEffect("그리고 너는…");
    typeEffect("그녀를 따라 불꽃 속으로 몸을 던졌다.");
    typeEffect("검이 부서지고, 살이 찢기고, 의식은 까맣게 가라앉았다.");
    waitForEnter();

    typeEffect("...");
    typeEffect("기적처럼 눈을 떴을 때, 너는 더 이상 전장이 아닌 낯선 천장을 보고 있었다.");
    waitForEnter();

    typeEffect("『정신이 드는가, 전사여...』");
    typeEffect("수척한 얼굴의 장로가, 너를 내려다보고 있었다.");
    waitForEnter();

    typeEffect("여기는 스페스.");
    typeEffect("전장을 뒤늦게 수색하던 마을 사람들과 장로가 너를 발견해, 회복실로 데려온 것이다.");
    waitForEnter();

    typeEffect("몸은 만신창이.");
    typeEffect("검은 불타고, 갑옷은 산산조각이 났다.");
    typeEffect("그리고 무엇보다, 그녀는 더 이상 곁에 없다.");
    waitForEnter();

    typeEffect("『그대의 분노를, 이 마을의 방패로 삼게.』");
    typeEffect("장로의 말은 곧 부탁이었다. 스페스를 괴롭히는 야수들을 처치해 달라는.");
    waitForEnter();

    typeEffect("그러나 너는 알고 있었다.");
    typeEffect("이 여정의 끝은 마을이 아니라, '그곳'이라는 걸.");
    typeEffect("수페르비아 탑.");
    typeEffect("죽음의 날개가 마지막으로 목격된, 하늘을 찌르는 고대의 탑.");
    waitForEnter();

    typeEffect("너는 칼을 쥐고 일어난다.");
    typeEffect("지금은 나약하지만, 반드시 강해질 것이다.");
    typeEffect("라일라의 이름으로—");
    typeEffect("데스페라도를 향한 복수의 길이, 지금 시작된다.");
    waitForEnter();

    std::cout << "\n=== 복수의 서막 ===\n\n";
}