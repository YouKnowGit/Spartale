// PauseMenu.h
#pragma once
#include "Framework/AbilitySystem/GameplayAbility.h"
#include "Utils/ConsoleRenderer.h"
#include <vector>
#include <string>
#include <Windows.h>

// 전방 선언
class Player;

// PauseMenu의 선택 결과를 나타내는 열거형
enum class EPauseMenuResult
{
    Resume,         // 게임으로 돌아가기
    GoToMainMenu    // 메인 메뉴로 나가기
    // TODO: 향후 인벤토리, 스킬북 등을 위한 값 추가 가능
};
// Pause 메뉴의 우측 화면에 대한 State
enum class ERightPaneState
{
    MainMenu,           // 기본 메뉴 목록
    StatDistribution,   // 스탯 분배 화면
    Inventory,          // 인벤토리 화면
    SkillBook,          // 세부 스킬 목록 화면
    SkillSelection      // 전체 스킬 목록 화면
};
class PauseMenu
{
public:
    // 생성자에서 렌더러와 플레이어의 참조
    PauseMenu(ConsoleRenderer& renderer, Player& player);
    ~PauseMenu() = default;

    // 메뉴를 실행하고, 사용자의 최종 선택을 반환
    EPauseMenuResult Run();

private:
    void ProcessInput();
    void Render();

    // 우측 메뉴 상태에 따른 분기 함수
    void ProcessMainMenuInput(int key);
    void ProcessStatDistributionInput(int key);
    void ProcessSkillBookInput(int key);
    void ProcessSkillSelectionInput(int key);

    // UI를 그리는 헬퍼 함수
    void DrawPlayerInfo();
    void DrawMainMenuOptions(); // 오른쪽 패널: 메뉴 목록
    void DrawStatDistributionScreen(); // 오른쪽 패널: 스탯 분배
    void DrawSkillBookScreen();
    void DrawSkillSelectionScreen();

    // 오른쪽 화면 Clear 함수
    void ClearRightPane();

    bool IsSkillEquipped(const GameplayAbility* skill) const;

private:
    ConsoleRenderer& m_renderer;
    Player& m_player;

    // 스킬북 화면에 보여줄 스킬 개수와 스크롤(페이지) 변수
    int m_skillListScrollOffset; 
    const int m_skillListVisibleCount = 10;

    // 상태 관리 변수
    bool m_bIsRunning;
    EPauseMenuResult m_result;
    ERightPaneState m_currentPaneState;

    // 커서 및 컨텍스트 변수
    int m_mainMenuSelection;
    int m_statSelection;
    int m_skillBookSlotSelection;    // 스킬북: 현재 선택된 장착 슬롯 (0~3)
    int m_skillSelectionListCursor;  // 스킬 목록: 현재 선택된 스킬 커서
    int m_slotIndexToModify;         // 스킬을 장착할 슬롯 인덱스 임시 저장

    const wchar_t* m_navigateSoundPath;
    const wchar_t* m_confirmSoundPath;
    const wchar_t* m_rejectSoundPath;
};