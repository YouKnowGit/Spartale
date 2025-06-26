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
    MainMenu,                   // 기본 메뉴 목록
    StatDistribution,           // 스탯 분배 화면
    Inventory,                  // 인벤토리 화면
    InventoryActionSelection,   // 인벤토리에서 아이템 선택 화면
	InventoryDropQuantity,      // 아이템을 버릴 때 수량 선택 화면
    SkillBook,                  // 세부 스킬 목록 화면
    SkillSelection              // 전체 스킬 목록 화면
};
enum class EShopState
{
    MainMenu,               // 기본 메뉴 목록
    Shop_Buy,               // 상점 구매 화면
	Shop_Buy_Action,        // 상점 구매 시 아이템 행동 선택 화면
	Shop_Buy_Quantity,      // 상점 구매 시 수량 입력 화면
    Shop_Sell,              // 상점 판매 화면
	Shop_Sell_Action,       // 상점 판매 시 아이템 행동 선택 화면
	Shop_Sell_Quantity,     // 상점 판매 시 수량 입력 화면
    Shop_Exit               // 상점 나가기
};

// 수량 입력 창 문맥 정의 열거형
enum class EQuantityInputContext
{
    Drop,   // 인벤토리에서 버리기
    Sell,   // 상점에서 판매하기
    Buy     // 상점에서 구매하기
};

class PauseMenu
{
public:
    // 생성자에서 렌더러와 플레이어의 참조
    PauseMenu(ConsoleRenderer& renderer, Player& player);
    ~PauseMenu() = default;

    // 메뉴를 실행하고, 사용자의 최종 선택을 반환
    EPauseMenuResult Run();
    EPauseMenuResult Shop();

private:
    // --- 입력 처리 함수들 ---
    void ProcessInput();
    void ProcessInputShop();
    void ProcessMainMenuInput(int key);
    void ProcessStatDistributionInput(int key);
    void ProcessSkillBookInput(int key);
    void ProcessSkillSelectionInput(int key);
	void ProcessQuantityInput(int key);
    void ProcessInventoryInput(int key);
    void ProcessInventoryActionInput(int key);
    void ProcessMainShopInput(int key);
    void ProcessShopBuyInput(int key);
    void ProcessShopBuyActionInput(int key);
	void ProcessShopSellInput(int key);
	void ProcessShopActionInput(int key);

    // --- 그리기 함수들 ---
    void Render();
    void RenderShop();
    void DrawPlayerInfo();
    void DrawMainMenuOptions();
    void DrawStatDistributionScreen();
    void DrawSkillBookScreen();
    void DrawSkillSelectionScreen();
    void DrawShopOptions();
	void DrawShopBuyScreen();
	void DrawShopBuyActionMenu();
    void DrawInventoryScreen(bool bIsSellMode = false);
    void DrawInventoryActionMenu();
    void DrawInventoryDropPrompt();
    void DrawItemInfoBox(bool bIsSellMode = false);

    // --- 헬퍼 함수 ---
    void ClearRightPane();
    bool IsSkillEquipped(const GameplayAbility* skill) const;
    void BuildInventoryActionMenu(bool bIsSellMode);

private:
    ConsoleRenderer& m_renderer;
    Player& m_player;

    // --- UI 레이아웃 상수 ---
    const int m_skillListVisibleCount = 10;
    const int m_inventoryVisibleCount = 8;

    // --- 상태 관리 변수 ---
    bool m_bIsRunning;
    EPauseMenuResult m_result;
    ERightPaneState m_currentPaneState;
    EShopState m_currentShopState;
    EQuantityInputContext m_quantityContext;

    // --- 커서 및 컨텍스트 변수 ---
    // [메인 메뉴]
    int m_mainMenuSelection;

	// [스탯 분배 관련]
	int m_statSelection;

    // [상점 관련]
    int m_shopSelection;          // 상점 메인 메뉴 (구매/판매/나가기) 커서
    int m_shopBuyCursor;          // 상점 구매 목록 커서
    int m_shopBuyScrollOffset;    // 상점 구매 목록 스크롤

    // [인벤토리 및 판매 목록 공용]
    int m_inventorySlotSelection; // 인벤토리, 판매 목록 커서
    int m_inventoryScrollOffset;  // 인벤토리, 판매 목록 스크롤
    
    // [아이템 행동 선택 관련]
    int m_itemActionCursor;
    std::vector<std::wstring> m_currentItemActions;
    std::wstring m_dropQuantityInput;

    // [스킬북 관련]
    int m_skillBookSlotSelection;
    int m_skillSelectionListCursor;
    int m_skillListScrollOffset;  // 이전에 누락되었던 변수
    int m_slotIndexToModify;

	// --- 피드백 메시지 ---
    std::wstring m_feedbackMessage;                 // 화면에 표시할 메시지 텍스트
    ULONGLONG m_feedbackMessageEndTime;             // 메시지가 사라져야 할 시간 
    const DWORD m_feedbackMessageDuration = 2000;   // 메시지를 보여줄 시간 (2초)

    // --- 사운드 경로 ---
    const wchar_t* m_navigateSoundPath;
    const wchar_t* m_confirmSoundPath;
    const wchar_t* m_rejectSoundPath;
    const wchar_t* m_escSoundPath;
};