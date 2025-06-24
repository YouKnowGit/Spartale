#include "Story.h"

void Scene_FinalBoss() {
    typeEffect("탑의 꼭대기, 문이 스스로 열리며 어둠이 너를 삼킨다.");
    typeEffect("안은 조용하다. 너무도 조용하다.");
    waitForEnter();

    typeEffect("허물어진 석상들, 바닥엔 타다 남은 재와 뼈.");
    typeEffect("공기 속엔 타락한 마법의 잔재와 죽음의 내음이 섞여 있다.");
    waitForEnter();

    typeEffect("그 순간—");
    typeEffect("지축을 흔드는 낮은 울음소리.");
    waitForEnter();

    typeEffect("어둠 속에서 두 눈이 떠오른다.");
    typeEffect("심장을 찌르는 붉은 빛. 그 눈동자는 전장을 삼켰던 그날과 같았다.");
    waitForEnter();

    typeEffect("『……드디어 왔군, 인간.』");
    typeEffect("『그 불꽃 속에서도 살아남다니, 하찮지만 흥미롭다.』");
    waitForEnter();

    typeEffect("드래곤 데스페라도, 죽음의 날개는 천천히 모습을 드러낸다.");
    typeEffect("수백 미터에 달하는 검은 비늘, 하늘을 가린 거대한 날개.");
    waitForEnter();

    typeEffect("너의 몸은 떨렸지만, 눈은 피하지 않는다.");
    typeEffect("『라일라의 죽음을... 갚아주러 왔다.』");
    waitForEnter();

    typeEffect("『그 이름... 그 불쌍한 인간. 여전히 네 안에 있느냐.』");
    typeEffect("『그녀의 절규는... 아름다웠지.』");
    waitForEnter();

    typeEffect("피가 끓는다. 검이 흔들린다.");
    typeEffect("심장은 고통 속에서 다시 깨어난다.");
    waitForEnter();

    typeEffect("『말을 멈춰라, 짐승아.』");
    typeEffect("『오늘, 네 심장에서 내 검이 나올 것이다.』");
    waitForEnter();

    std::cout << "\n=== 최종 결투 개시: 드래곤 '데스페라도' ===\n\n";
}
