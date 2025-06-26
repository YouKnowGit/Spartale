#include "GameLogic/PauseMenu.h"
#include "Utils/ConsoleRenderer.h"
#include "GameLogic/Units/Player.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/AttributeSet.h"
#include "GameLogic/SaveManager.h"

#include <iostream>
#include <conio.h>
#include <iomanip> 
#include <sstream> 
#include <algorithm> // std::find
#include <mmsystem.h>
#include <Windows.h>

#pragma comment(lib, "winmm.lib")

void DrawBar(ConsoleRenderer& renderer, int x, int y, int width, float current, float max, WORD attribute);

PauseMenu::PauseMenu(ConsoleRenderer& renderer, Player& player)
    : m_renderer(renderer),
    m_player(player),
    m_bIsRunning(true),
    m_result(EPauseMenuResult::Resume),
    m_currentPaneState(ERightPaneState::MainMenu),
    m_currentShopState(EShopState::MainMenu),
    m_mainMenuSelection(0),
    m_statSelection(0),
    m_shopSelection(0),
    m_shopBuyCursor(0),
    m_shopBuyScrollOffset(0),
    m_skillBookSlotSelection(0),
    m_skillSelectionListCursor(0),
    m_slotIndexToModify(0),
    m_skillListScrollOffset(0),
    m_inventorySlotSelection(0),
    m_inventoryScrollOffset(0),
    m_itemActionCursor(0),
    m_feedbackMessageEndTime(0)

{
    m_navigateSoundPath = L"Sounds/UI/pausemenu_select.wav";    // 메뉴 이동 효과음
    m_confirmSoundPath = L"Sounds/UI/pausemenu_confirm.wav";    // 메뉴 선택 효과음
    m_rejectSoundPath = L"Sounds/UI/menu_reject2.wav";          // 메뉴 선택 효과음
    m_escSoundPath = L"Sounds/UI/mainmenu_select.wav";          // 메뉴 이동 효과음
}

EPauseMenuResult PauseMenu::Run()
{
    m_renderer.Clear();

    PlaySound(m_escSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
    while (m_bIsRunning)
    {
        ProcessInput();
        Render();
        Sleep(16);
    }
    return m_result;
}
EPauseMenuResult PauseMenu::Shop()
{
    m_renderer.Clear();

    PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
    while (m_bIsRunning)
    {
        ProcessInputShop();
        RenderShop();
        Sleep(16);
    }
    return m_result;
}

// --- 입력 처리 함수들 ---
void PauseMenu::ProcessInput()
{
    if (!_kbhit()) return;
    int key = _getch();

    // ESC 키는 현재 상태에 따라 다르게 동작
    if (key == 27) // ESC
    {
        PlaySound(m_escSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        switch (m_currentPaneState)
        {
        case ERightPaneState::MainMenu:                 m_bIsRunning = false; m_result = EPauseMenuResult::Resume; break;
        case ERightPaneState::StatDistribution:         m_currentPaneState = ERightPaneState::MainMenu; break;
        case ERightPaneState::Inventory:                m_currentPaneState = ERightPaneState::MainMenu; break;
        case ERightPaneState::InventoryActionSelection: m_currentPaneState = ERightPaneState::Inventory; break;
        case ERightPaneState::InventoryDropQuantity:    m_currentPaneState = ERightPaneState::InventoryActionSelection; break;
        case ERightPaneState::SkillBook:                m_currentPaneState = ERightPaneState::MainMenu; break;
        case ERightPaneState::SkillSelection:           m_currentPaneState = ERightPaneState::SkillBook; break;
        }
        return;
    }

    // 상태에 따라 입력을 분기 처리
    switch (m_currentPaneState)
    {
    case ERightPaneState::MainMenu:                 ProcessMainMenuInput(key); break;
    case ERightPaneState::StatDistribution:         ProcessStatDistributionInput(key); break;
    case ERightPaneState::Inventory:                ProcessInventoryInput(key); break;
    case ERightPaneState::InventoryActionSelection: ProcessInventoryActionInput(key); break;
    case ERightPaneState::InventoryDropQuantity:    ProcessQuantityInput(key); break;
    case ERightPaneState::SkillBook:                ProcessSkillBookInput(key); break;
    case ERightPaneState::SkillSelection:           ProcessSkillSelectionInput(key); break;
    }
}

void PauseMenu::ProcessInputShop()
{
    if (!_kbhit()) return;
    int key = _getch();

    // ESC 키는 현재 상태에 따라 다르게 동작
    if (key == 27) // ESC
    {
        PlaySound(m_escSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        switch (m_currentShopState)
        {
        case EShopState::MainMenu:              m_bIsRunning = false; m_result = EPauseMenuResult::Resume; break;
        case EShopState::Shop_Buy:              m_currentShopState = EShopState::MainMenu; break;
		case EShopState::Shop_Buy_Action:       m_currentShopState = EShopState::Shop_Buy; break;
		case EShopState::Shop_Buy_Quantity:     m_currentShopState = EShopState::Shop_Buy_Action; break;
        case EShopState::Shop_Sell:             m_currentShopState = EShopState::MainMenu; break;
        case EShopState::Shop_Sell_Action:      m_currentShopState = EShopState::Shop_Sell; break;
        case EShopState::Shop_Sell_Quantity:    m_currentShopState = EShopState::Shop_Sell_Action; break;
        }
        return;
    }

    // 상태에 따라 입력을 분기 처리
    switch (m_currentShopState)
    {
    case EShopState::MainMenu:              ProcessMainShopInput(key); break;
    case EShopState::Shop_Buy:              ProcessShopBuyInput(key); break;
	case EShopState::Shop_Buy_Action:       ProcessShopBuyActionInput(key); break;
	case EShopState::Shop_Buy_Quantity:     ProcessQuantityInput(key); break;
    case EShopState::Shop_Sell:             ProcessShopSellInput(key); break;
    case EShopState::Shop_Sell_Action:      ProcessShopActionInput(key); break;
    case EShopState::Shop_Sell_Quantity:    ProcessQuantityInput(key); break;
    }
}

void PauseMenu::ProcessMainMenuInput(int key)
{
    const std::vector<std::wstring> options = { L"스탯 분배", L"인벤토리", L"스킬북", L"저장", L"메인메뉴로 나가기" };
    if (key == 224)
    {
        key = _getch();
        if (key == 72) // 위쪽 화살표
        {
            // 선택 인덱스를 1 감소. 0보다 작아지면 마지막 인덱스로 순환
            PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            m_mainMenuSelection = (m_mainMenuSelection == 0) ? static_cast<int>(options.size()) - 1 : m_mainMenuSelection - 1;
        }
        else if (key == 80) // 아래쪽 화살표
        {
            // 선택 인덱스를 1 증가. 마지막 인덱스를 넘어가면 0으로 순환
            PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            m_mainMenuSelection = (m_mainMenuSelection + 1) % static_cast<int>(options.size());
        }
    }
    else if (key == 13) // 엔터
    {
        PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        switch (m_mainMenuSelection)
        {
        case 0: m_currentPaneState = ERightPaneState::StatDistribution; m_statSelection = 0; break;
        case 1: m_currentPaneState = ERightPaneState::Inventory; m_inventorySlotSelection = 0; break;
        case 2: m_currentPaneState = ERightPaneState::SkillBook; m_skillBookSlotSelection = 0; break;
        case 3:
        {
            SaveManager sm(m_player);
            sm.SaveGame("save.txt");
            break;
        }
        case 4: m_bIsRunning = false; m_result = EPauseMenuResult::GoToMainMenu; break;
        }
    }
}

void PauseMenu::ProcessStatDistributionInput(int key)
{
    const int statCount = 3; // 힘, 민첩, 지능

    if (key == 224)
    {
        key = _getch();
        if (key == 72) // 위
        {
            PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            m_statSelection = (m_statSelection == 0) ? statCount - 1 : m_statSelection - 1;
        }
        else if (key == 80) // 아래
        {
            PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            m_statSelection = (m_statSelection + 1) % statCount;
        }
    }
    else if (key == 13) // 엔터: 스탯 포인트 분배
    {
        AttributeSet* stats = m_player.GetAbilityComponent()->GetAttributeSet();
        if (stats && stats->AdditionalStatPoints > 0)
        {
            PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            stats->AdditionalStatPoints--;
            switch (m_statSelection)
            {
            case 0: // 힘
                stats->Strength.BaseValue++;
                stats->Strength.CurrentValue++;
                break;
            case 1: // 민첩
                stats->Agility.BaseValue++;
                stats->Agility.CurrentValue++;
                break;
            case 2: // 지능
                stats->Intelligence.BaseValue++;
                stats->Intelligence.CurrentValue++;
                break;
            }
            stats->AdjustDependentAttributes();
        }
        else // 보너스 스탯이 없으면 reject
        {
            PlaySound(m_rejectSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        }
    }
}

void PauseMenu::ProcessSkillBookInput(int key)
{
    if (key == 224)
    {
        key = _getch();
        if (key == 72) // 위
        {
            PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            m_skillBookSlotSelection = (m_skillBookSlotSelection == 0) ? 3 : m_skillBookSlotSelection - 1;
        }
        else if (key == 80) // 아래
        {
            PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            m_skillBookSlotSelection = (m_skillBookSlotSelection == 3) ? 0 : m_skillBookSlotSelection + 1;
        }
    }
    else if (key == 13) // 엔터: 스킬 변경 시작
    {
        PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        m_slotIndexToModify = m_skillBookSlotSelection;
        m_skillSelectionListCursor = 0;
        m_currentPaneState = ERightPaneState::SkillSelection;
    }
}

void PauseMenu::ProcessSkillSelectionInput(int key)
{
    const auto& grantedAbilities = m_player.GetAbilityComponent()->GetGrantedAbilities();
    if (grantedAbilities.empty()) return;

    // 습득 스킬 개수를 읽어와서 저장
    int abilityCount = static_cast<int>(grantedAbilities.size());

    // 장착 가능한 어빌리티 목록 생성
    std::vector<int> availableSkillIndices;
    for (size_t i = 0; i < grantedAbilities.size(); ++i)
    {
        if (!IsSkillEquipped(grantedAbilities[i].get()))
        {
            availableSkillIndices.push_back(i);
        }
    }

    // 장착 가능한 스킬이 아예 없으면 종료
    if (availableSkillIndices.empty()) return;

    // --- 2. Enter 키 입력 처리 ---
    if (key == 13) // 엔터: 스킬 장착 확정
    {
        // 현재 커서가 가리키는 스킬이 장착 가능한지 다시 한번 확인
        bool bCanEquip = std::find(availableSkillIndices.begin(), availableSkillIndices.end(), m_skillSelectionListCursor) != availableSkillIndices.end();
        PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);

        if (bCanEquip)
        {
            GameplayAbility* selectedAbility = m_player.GetAbilityComponent()->GetGrantedAbility(m_skillSelectionListCursor);
            m_player.GetAbilityComponent()->EquipAbility(m_slotIndexToModify, selectedAbility);
            m_currentPaneState = ERightPaneState::SkillBook;
            PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        }
        return; // 장착 불가능한 스킬이면 종료
    }

    if (key == 224) // 방향키
    {
        key = _getch();
        PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);

        if (key == 72) // 위
        {
            m_skillSelectionListCursor = (m_skillSelectionListCursor == 0) ? abilityCount - 1 : m_skillSelectionListCursor - 1;

            // 커서가 스크롤 영역 위로 벗어나면 스크롤을 올림
            if (m_skillSelectionListCursor < m_skillListScrollOffset)
            {
                m_skillListScrollOffset = m_skillSelectionListCursor;
            }
            // 맨 마지막 항목으로 순환했을 때, 스크롤도 맨 아래로 이동
            if (m_skillSelectionListCursor == abilityCount - 1)
            {
                m_skillListScrollOffset = (std::max)(0, abilityCount - m_skillListVisibleCount);
            }
        }
        else if (key == 80) // 아래
        {
            m_skillSelectionListCursor = (m_skillSelectionListCursor == abilityCount - 1) ? 0 : m_skillSelectionListCursor + 1;

            // 커서가 스크롤 영역 아래로 벗어나면 스크롤을 내림
            if (m_skillSelectionListCursor >= m_skillListScrollOffset + m_skillListVisibleCount)
            {
                m_skillListScrollOffset = m_skillSelectionListCursor - m_skillListVisibleCount + 1;
            }
            // 맨 첫 항목으로 순환했을 때, 스크롤도 맨 위로 이동
            if (m_skillSelectionListCursor == 0)
            {
                m_skillListScrollOffset = 0;
            }
        }
    }
}

void PauseMenu::ProcessQuantityInput(int key)
{
    // --- 숫자/백스페이스 입력 처리 로직 (기존과 동일) ---
    if (key >= '0' && key <= '9') {
        if (m_dropQuantityInput.length() < 5) m_dropQuantityInput += static_cast<wchar_t>(key);
    }
    else if (key == 8) {
        if (!m_dropQuantityInput.empty()) m_dropQuantityInput.pop_back();
    }
    // --- 엔터 키 입력 처리 (리팩토링된 로직) ---
    else if (key == 13) // 엔터: 확정
    {
        if (m_dropQuantityInput.empty()) return;

        try 
        {
            int quantity = std::stoi(m_dropQuantityInput);
            if (quantity <= 0) { // 0개는 처리하지 않음
                PlaySound(m_rejectSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
                m_dropQuantityInput.clear();
                return;
            }

            // '문맥'에 따라 다른 행동을 수행
            switch (m_quantityContext)
            {
            case EQuantityInputContext::Drop:
            case EQuantityInputContext::Sell:
            {
                InventoryComponent* inventory = m_player.GetInventory();
                const InventorySlot* slot = inventory ? inventory->GetSlotAtIndex(m_inventorySlotSelection) : nullptr;

                // 판매/버리기는 내가 가진 수량보다 많이 할 수 없음
                if (!slot || !slot->pItemData || quantity > slot->Quantity) {
                    PlaySound(m_rejectSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
                    m_dropQuantityInput.clear();
                    return;
                }

                PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);

                int goldGained = static_cast<int>(slot->pItemData->Price * 0.6) * quantity;

                if (m_quantityContext == EQuantityInputContext::Drop) {
                    // 장착 중인 아이템이면 먼저 해제
                    if (slot->bIsEquipped) m_player.Unequip(m_inventorySlotSelection);
                    inventory->RemoveItem(m_inventorySlotSelection, quantity);
                    m_currentPaneState = ERightPaneState::Inventory;
                }
                else { // Sell
                    if (slot->bIsEquipped) m_player.Unequip(m_inventorySlotSelection);
                    inventory->RemoveItem(m_inventorySlotSelection, quantity);
                    m_player.IncreaseGold(goldGained);
                    m_currentShopState = EShopState::Shop_Sell;
                }
                break;
            }

            case EQuantityInputContext::Buy:
            {
                const ShopData* shopData = DataManager::GetInstance().GetShopData("vendor_general");
                if (!shopData || m_shopBuyCursor >= shopData->itemIds.size()) return;

                const std::string& itemId = shopData->itemIds[m_shopBuyCursor];
                const ItemData* itemData = DataManager::GetInstance().GetItemData(itemId);
                AttributeSet* playerStats = m_player.GetAbilityComponent()->GetAttributeSet();
                if (!itemData || !playerStats) return;

                int totalCost = itemData->Price * quantity;

                if (playerStats->Gold >= totalCost) {
                    if (m_player.GetInventory()->AddItem(itemId, quantity) == 0) {
                        m_player.DecreaseGold(totalCost);
                        PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
                        m_feedbackMessage = L"성공적으로 구매했습니다.";
                    }
                    else {
                        m_player.GetInventory()->RemoveItemByItemID(itemId, quantity); // 롤백
                        PlaySound(m_rejectSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
                        m_feedbackMessage = L"인벤토리 공간이 부족합니다.";
                    }
                }
                else {
                    PlaySound(m_rejectSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
                    m_feedbackMessage = L"골드가 부족합니다.";
                }
                m_feedbackMessageEndTime = GetTickCount64() + m_feedbackMessageDuration;
                m_currentShopState = EShopState::Shop_Buy;
                break;
            }
            }
        }
        catch (const std::exception&) 
        {
            PlaySound(m_rejectSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            m_dropQuantityInput.clear();
        }
    }
}

void PauseMenu::ProcessInventoryInput(int key)
{
    InventoryComponent* inventory = m_player.GetInventory();
    if (!inventory) return;

    const int capacity = inventory->GetCapacity();

    if (key == 224) {
        key = _getch();
        PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        if (key == 72) { // 위
            m_inventorySlotSelection = (m_inventorySlotSelection == 0) ? capacity - 1 : m_inventorySlotSelection - 1;
            if (m_inventorySlotSelection < m_inventoryScrollOffset) {
                m_inventoryScrollOffset = m_inventorySlotSelection;
            }
            if (m_inventorySlotSelection == capacity - 1) {
                m_inventoryScrollOffset = (std::max)(0, capacity - m_inventoryVisibleCount);
            }
        }
        else if (key == 80) { // 아래
            m_inventorySlotSelection = (m_inventorySlotSelection + 1) % capacity;
            if (m_inventorySlotSelection >= m_inventoryScrollOffset + m_inventoryVisibleCount) {
                m_inventoryScrollOffset = m_inventorySlotSelection - m_inventoryVisibleCount + 1;
            }
            if (m_inventorySlotSelection == 0) {
                m_inventoryScrollOffset = 0;
            }
        }
    }
    else if (key == 13) { // 엔터
        const InventorySlot* slot = m_player.GetInventory()->GetSlotAtIndex(m_inventorySlotSelection);
        if (slot && slot->Quantity > 0) {
            PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            BuildInventoryActionMenu(false); // 인벤토리 모드
            m_currentPaneState = ERightPaneState::InventoryActionSelection;
            m_itemActionCursor = 0;
        }
        else {
            PlaySound(m_rejectSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        }
    }
}

void PauseMenu::ProcessInventoryActionInput(int key)
{
    if (m_currentItemActions.empty()) return;

    if (key == 224) // 방향키
    {
        key = _getch();
        PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        if (key == 72) // 위
        {
            m_itemActionCursor = (m_itemActionCursor == 0) ? m_currentItemActions.size() - 1 : m_itemActionCursor - 1;
        }
        else if (key == 80) // 아래
        {
            m_itemActionCursor = (m_itemActionCursor + 1) % m_currentItemActions.size();
        }
    }
    else if (key == 13) // 엔터: 행동 선택
    {
        PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);

        std::wstring selectedAction = m_currentItemActions[m_itemActionCursor];

        if (selectedAction == L"사용하기" || selectedAction == L"장착하기" || selectedAction == L"장착해제")
        {
            m_player.GetInventory()->UseItem(m_inventorySlotSelection, &m_player);
            m_currentPaneState = ERightPaneState::Inventory;
        }
        else if (selectedAction == L"버리기")
        {
            const InventorySlot* slot = m_player.GetInventory()->GetSlotAtIndex(m_inventorySlotSelection);
            if (slot && slot->Quantity > 0)
            {
                // 장착 중인 아이템 장착 해제 먼저
                if (slot->bIsEquipped)
                {
                    m_player.Unequip(m_inventorySlotSelection);
                }
                // 아이템 수량을 확인
                if (slot->Quantity > 1)
                {
                    // 수량이 1개보다 많으면, '수량 입력' 상태로 전환
                    PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
                    m_quantityContext = EQuantityInputContext::Drop;
                    m_currentPaneState = ERightPaneState::InventoryDropQuantity;
                    m_dropQuantityInput.clear();
                }
                else
                {
                    // 수량이 1개뿐이면, 바로 버리고 인벤토리 목록으로
                    PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
                    m_player.GetInventory()->RemoveItem(m_inventorySlotSelection, 1);
                    m_currentPaneState = ERightPaneState::Inventory;
                }
            }

        }
        else if (selectedAction == L"취소")
        {
            PlaySound(m_escSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            m_currentPaneState = ERightPaneState::Inventory; // 인벤토리 목록으로 복귀
        }
    }
}

void PauseMenu::ProcessMainShopInput(int key)
{
    const std::vector<std::wstring> options = { L"구매", L"판매", L"나가기" };
    if (key == 224)
    {
        key = _getch();
        if (key == 72) // 위쪽 화살표
        {
            // 선택 인덱스를 1 감소. 0보다 작아지면 마지막 인덱스로 순환
            PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            m_shopSelection = (m_shopSelection == 0) ? static_cast<int>(options.size()) - 1 : m_shopSelection - 1;
        }
        else if (key == 80) // 아래쪽 화살표
        {
            // 선택 인덱스를 1 증가. 마지막 인덱스를 넘어가면 0으로 순환
            PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            m_shopSelection = (m_shopSelection + 1) % static_cast<int>(options.size());
        }
    }
    else if (key == 13) // 엔터
    {
        PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        switch (m_shopSelection)
        {
        case 0: // 구매
            m_currentShopState = EShopState::Shop_Buy;
            m_shopBuyCursor = 0;          // 올바른 커서 변수 사용
            m_shopBuyScrollOffset = 0;    // 구매 목록 스크롤도 함께 초기화
            break;
        case 1: // 판매
            m_currentShopState = EShopState::Shop_Sell;
            m_inventorySlotSelection = 0; // 판매 목록은 인벤토리 커서를 사용
            m_inventoryScrollOffset = 0;  // 인벤토리 스크롤도 함께 초기화
            break;
        case 2: // 나가기
            m_currentShopState = EShopState::Shop_Exit;
            m_bIsRunning = false;
            m_result = EPauseMenuResult::Resume;
            break;
        }
    }
}

void PauseMenu::ProcessShopBuyInput(int key)
{
    const ShopData* shopData = DataManager::GetInstance().GetShopData("vendor_general");
    if (!shopData) return;

    const int totalItems = static_cast<int>(shopData->itemIds.size());
    if (totalItems == 0) return; // 팔 아이템이 없으면 아무것도 안 함

    // --- 1. 방향키 입력 처리 ---
    if (key == 224)
    {
        key = _getch();
        PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        if (key == 72) // 위
        {
            m_shopBuyCursor = (m_shopBuyCursor == 0) ? totalItems - 1 : m_shopBuyCursor - 1;
            if (m_shopBuyCursor < m_shopBuyScrollOffset) {
                m_shopBuyScrollOffset = m_shopBuyCursor;
            }
            if (m_shopBuyCursor == totalItems - 1) {
                m_shopBuyScrollOffset = (std::max)(0, totalItems - m_inventoryVisibleCount);
            }
        }
        else if (key == 80) // 아래
        {
            m_shopBuyCursor = (m_shopBuyCursor + 1) % totalItems;
            if (m_shopBuyCursor >= m_shopBuyScrollOffset + m_inventoryVisibleCount) {
                m_shopBuyScrollOffset = m_shopBuyCursor - m_inventoryVisibleCount + 1;
            }
            if (m_shopBuyCursor == 0) {
                m_shopBuyScrollOffset = 0;
            }
        }
    }
    // --- 2. 엔터 키 입력 처리 (구매 실행) ---
    else if (key == 13)
    {
        PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);

        // 가능한 행동 목록("구매", "취소")
        m_currentItemActions.clear();
        m_currentItemActions.push_back(L"구매");
        m_currentItemActions.push_back(L"취소");

        // '구매 액션 선택' 상태로 전환하고 커서를 초기화
        m_currentShopState = EShopState::Shop_Buy_Action;
        m_itemActionCursor = 0;
    }
}

void PauseMenu::ProcessShopBuyActionInput(int key)
{
    if (m_currentItemActions.empty()) return;

    if (key == 224) // 방향키
    {
        key = _getch();
        PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        if (key == 72) // 위
        {
            m_itemActionCursor = (m_itemActionCursor == 0) ? m_currentItemActions.size() - 1 : m_itemActionCursor - 1;
        }
        else if (key == 80) // 아래
        {
            m_itemActionCursor = (m_itemActionCursor + 1) % m_currentItemActions.size();
        }
    }
    else if (key == 13) // 엔터
    {
        const ShopData* shopData = DataManager::GetInstance().GetShopData("vendor_general");
        if (!shopData || m_shopBuyCursor >= shopData->itemIds.size()) return;
        std::wstring selectedAction = m_currentItemActions[m_itemActionCursor];
        const std::string& itemId = shopData->itemIds[m_shopBuyCursor];
        const ItemData* itemData = DataManager::GetInstance().GetItemData(itemId);
        if (!itemData) return;

        if (selectedAction == L"구매")
        {
            if (itemData->MaxStackSize > 1)
            {
                PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
                m_quantityContext = EQuantityInputContext::Buy;
                m_currentShopState = EShopState::Shop_Buy_Quantity;
                m_dropQuantityInput.clear();
            }
            else // 겹쳐지는 아이템이 아닌경우
            {
                // 즉시 1개 구매 로직 실행
                // 이후 개선 필요
                AttributeSet * playerStats = m_player.GetAbilityComponent()->GetAttributeSet();
                if (!playerStats) return;

                if (playerStats->Gold >= itemData->Price) {
                    if (m_player.GetInventory()->AddItem(itemId, 1) == 0) {
                        m_player.DecreaseGold(itemData->Price);
                        m_feedbackMessage = L"구매했습니다.";
                    }
                    else {
                        m_feedbackMessage = L"인벤토리가 가득 찼습니다.";
                    }
                }
                else {
                    m_feedbackMessage = L"골드가 부족합니다.";
                }
                m_feedbackMessageEndTime = GetTickCount64() + m_feedbackMessageDuration;
                m_currentShopState = EShopState::Shop_Buy;
            }
        }
        else if (selectedAction == L"취소")
        {
            PlaySound(m_escSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            m_currentShopState = EShopState::Shop_Buy; // 구매 목록으로 복귀
        }
    }
}

void PauseMenu::ProcessShopSellInput(int key)
{
    InventoryComponent* inventory = m_player.GetInventory();
    if (!inventory) return;

    // 방향키/스크롤 로직은 ProcessInventoryInput과 완전히 동일합니다.
    const int capacity = inventory->GetCapacity();
    if (key == 224)
    {
        key = _getch();
        PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);

        if (key == 72) // 위
        {
            m_inventorySlotSelection = (m_inventorySlotSelection == 0) ? capacity - 1 : m_inventorySlotSelection - 1;
            // 스크롤 조정
            if (m_inventorySlotSelection < m_inventoryScrollOffset) {
                m_inventoryScrollOffset = m_inventorySlotSelection;
            }
            if (m_inventorySlotSelection == capacity - 1) {
                m_inventoryScrollOffset = (std::max)(0, capacity - m_inventoryVisibleCount);
            }
        }
        else if (key == 80) // 아래
        {
            m_inventorySlotSelection = (m_inventorySlotSelection + 1) % capacity;
            // 스크롤 조정
            if (m_inventorySlotSelection >= m_inventoryScrollOffset + m_inventoryVisibleCount) {
                m_inventoryScrollOffset = m_inventorySlotSelection - m_inventoryVisibleCount + 1;
            }
            if (m_inventorySlotSelection == 0) {
                m_inventoryScrollOffset = 0;
            }
        }
    }

    else if (key == 13) // 엔터
    {
        const InventorySlot* slot = m_player.GetInventory()->GetSlotAtIndex(m_inventorySlotSelection);
        if (slot && slot->Quantity > 0)
        {
            PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            BuildInventoryActionMenu(true); // 판매 모드이므로 true
            m_currentShopState = EShopState::Shop_Sell_Action;
            m_itemActionCursor = 0;
        }
        else
        {
            PlaySound(m_rejectSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        }
    }
}

void PauseMenu::ProcessShopActionInput(int key)
{
    if (m_currentItemActions.empty()) return;

    if (key == 224) // 방향키
    {
        key = _getch();
        PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        if (key == 72) // 위
        {
            m_itemActionCursor = (m_itemActionCursor == 0) ? m_currentItemActions.size() - 1 : m_itemActionCursor - 1;
        }
        else if (key == 80) // 아래
        {
            m_itemActionCursor = (m_itemActionCursor + 1) % m_currentItemActions.size();
        }
    }
    else if (key == 13) // 엔터: 행동 선택
    {
        PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);

        std::wstring selectedAction = m_currentItemActions[m_itemActionCursor];

        if (selectedAction == L"판매")
        {
            const InventorySlot* slot = m_player.GetInventory()->GetSlotAtIndex(m_inventorySlotSelection);
            if (!slot || slot->Quantity <= 0) return; // 슬롯이 유효하지 않으면 아무것도 안함

            // 1. 장착 중인 아이템이라면, 판매 전에 먼저 장착 해제
            if (slot->bIsEquipped)
            {
                m_player.Unequip(m_inventorySlotSelection);
            }
            
            // 2. 아이템 수량을 확인하여 분기 처리합니다.
            if (slot->Quantity > 1)
            {
                m_quantityContext = EQuantityInputContext::Sell;
                m_currentShopState = EShopState::Shop_Sell_Quantity;
                m_dropQuantityInput.clear();
            }
            else
            {
                const ItemData* data = DataManager::GetInstance().GetItemData(slot->ItemID);
                if (!data) return;

                m_player.GetInventory()->RemoveItem(m_inventorySlotSelection, 1);
                m_player.IncreaseGold(static_cast<int>(data->Price * 0.6));
                m_currentShopState = EShopState::Shop_Sell;
            }
        }
        else if (selectedAction == L"취소")
        {
            PlaySound(m_escSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            m_currentShopState = EShopState::Shop_Sell;
        }
    }
}

// --- 그리기 함수들 ---
void PauseMenu::Render()
{
    // --- 왼쪽 패널: 캐릭터 정보 (항상 고정) ---
    DrawPlayerInfo();

    // --- 오른쪽 패널: 상태에 따라 다르게 그림 ---
    switch (m_currentPaneState)
    {
    case ERightPaneState::MainMenu:                 DrawMainMenuOptions(); break;
    case ERightPaneState::StatDistribution:         DrawStatDistributionScreen(); break;
    case ERightPaneState::Inventory:                DrawInventoryScreen(false); break;
    case ERightPaneState::InventoryActionSelection: DrawInventoryScreen(false); DrawInventoryActionMenu(); break;
    case ERightPaneState::InventoryDropQuantity:    DrawInventoryScreen(false); DrawInventoryDropPrompt(); break;
    case ERightPaneState::SkillBook:                DrawSkillBookScreen(); break;
    case ERightPaneState::SkillSelection:           DrawSkillSelectionScreen(); break;
    }
    m_renderer.Render();
}

void DrawBar(ConsoleRenderer& renderer, int x, int y, int width, float current, float max, WORD attribute)
{
    float ratio = (max > 0) ? (current / max) : 0;
    int filledWidth = static_cast<int>(ratio * width);
    for (int i = 0; i < width; ++i)
    {
        if (i < filledWidth) renderer.Draw(x + i, y, L'█', attribute);
        else renderer.Draw(x + i, y, L'░', attribute);
    }
}

void PauseMenu::RenderShop()
{
    // --- 왼쪽 패널: 캐릭터 정보 (항상 고정) ---
    DrawPlayerInfo();

    // --- 오른쪽 패널: 상태에 따라 다르게 그림 ---
    switch (m_currentShopState)
    {
    case EShopState::MainMenu:              DrawShopOptions(); break;
    case EShopState::Shop_Buy:              DrawShopBuyScreen(); break;
    case EShopState::Shop_Buy_Action:       DrawShopBuyScreen(); DrawShopBuyActionMenu(); break;
    case EShopState::Shop_Buy_Quantity:     DrawShopBuyScreen(); DrawInventoryDropPrompt(); break;
    case EShopState::Shop_Sell:             DrawInventoryScreen(true); break;
    case EShopState::Shop_Sell_Action:      DrawInventoryScreen(true); DrawInventoryActionMenu(); break;
    case EShopState::Shop_Sell_Quantity:    DrawInventoryScreen(true); DrawInventoryDropPrompt(); break;
    }
    m_renderer.Render();
}

void PauseMenu::DrawPlayerInfo()
{
    AttributeSet* stats = m_player.GetAbilityComponent()->GetAttributeSet();
    if (!stats) return;

    int panelX = 4, panelY = 2;
    int panelWidth = m_renderer.GetWidth() - (panelX * 2);
    int panelHeight = m_renderer.GetHeight() - (panelY * 2) - 1;
    int midX = panelX + panelWidth / 2;

    // 메인 패널 프레임 그리기
    m_renderer.DrawString(panelX, panelY, L"╔" + std::wstring(panelWidth - 2, L'═') + L"╗");
    for (int i = 1; i < panelHeight; ++i) m_renderer.DrawString(panelX, panelY + i, L"║" + std::wstring(panelWidth - 2, L' ') + L"║");
    m_renderer.DrawString(panelX, panelY + panelHeight, L"╚" + std::wstring(panelWidth - 2, L'═') + L"╝");
    std::wstring title = L"P A U S E D";
    m_renderer.DrawString(panelX + (panelWidth - title.length()) / 2, panelY + 2, title);
    m_renderer.DrawString(panelX, panelY + 4, L"╠" + std::wstring(panelWidth - 2, L'═') + L"╣");
    m_renderer.DrawString(midX, panelY, L"╦");
    m_renderer.DrawString(midX, panelY + 4, L"╬");
    for (int i = 5; i < panelHeight; ++i) m_renderer.DrawString(midX, panelY + i, L"║");
    m_renderer.DrawString(midX, panelY + panelHeight, L"╩");

    int infoX = panelX + 3;
    int infoY = panelY + 6;

    m_renderer.DrawString(infoX, infoY++, L"이름: " + m_player.Name);
    m_renderer.DrawString(infoX, infoY++, L"레벨: " + std::to_wstring(stats->Level));
    infoY++;

    std::wstringstream hpStream;
    hpStream << L"HP  [" << static_cast<int>(stats->HP.CurrentValue) << L" / " << static_cast<int>(stats->HP.BaseValue) << L"]";
    m_renderer.DrawString(infoX, infoY++, hpStream.str());
    DrawBar(m_renderer, infoX, infoY, midX - infoX - 4, stats->HP.CurrentValue, stats->HP.BaseValue, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    infoY += 2;

    std::wstringstream mpStream;
    mpStream << L"MP  [" << static_cast<int>(stats->MP.CurrentValue) << L" / " << static_cast<int>(stats->MP.BaseValue) << L"]";
    m_renderer.DrawString(infoX, infoY++, mpStream.str());
    DrawBar(m_renderer, infoX, infoY, midX - infoX - 4, stats->MP.CurrentValue, stats->MP.BaseValue, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    infoY += 2;

    std::wstringstream expStream;
    float requiredExp = 100.0f * stats->Level;
    expStream << L"EXP [" << static_cast<int>(stats->Experience.CurrentValue) << L" / " << static_cast<int>(requiredExp) << L"]";
    m_renderer.DrawString(infoX, infoY++, expStream.str());
    DrawBar(m_renderer, infoX, infoY, midX - infoX - 4, stats->Experience.CurrentValue, requiredExp, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    infoY += 3;

    m_renderer.DrawString(infoX, infoY++, L"힘        : " + std::to_wstring(static_cast<int>(stats->Strength.CurrentValue)));
    m_renderer.DrawString(infoX, infoY++, L"민첩      : " + std::to_wstring(static_cast<int>(stats->Agility.CurrentValue)));
    m_renderer.DrawString(infoX, infoY++, L"지능      : " + std::to_wstring(static_cast<int>(stats->Intelligence.CurrentValue)));
    m_renderer.DrawString(infoX, infoY++, L"방어력    : " + std::to_wstring(static_cast<int>(stats->Defence.CurrentValue)));
    infoY++;
    m_renderer.DrawString(infoX, infoY++, L"골드      : " + std::to_wstring(static_cast<int>(stats->Gold)) + L" G");

    infoY++; // 골드와 장비 목록 사이에 한 줄 띄우기
    m_renderer.DrawString(infoX, infoY++, L"--- [ 장착 장비 ] ---");

    InventoryComponent* inventory = m_player.GetInventory();
    if (inventory) // 인벤토리 포인터가 유효한지 확인
    {
        // 1. 무기 정보 가져오기
        std::wstring weaponName = L"없음";
        int weaponSlotIndex = m_player.GetEquippedWeaponSlot();
        if (weaponSlotIndex != -1)
        {
            const InventorySlot* slot = inventory->GetSlotAtIndex(weaponSlotIndex);
            if (slot && slot->pItemData) {
                weaponName = slot->pItemData->Name;
            }
        }
        m_renderer.DrawString(infoX, infoY++, L" 무기      : " + weaponName);

        // 2. 방어구 정보 가져오기
        std::wstring armorName = L"없음";
        int armorSlotIndex = m_player.GetEquippedArmorSlot();
        if (armorSlotIndex != -1)
        {
            const InventorySlot* slot = inventory->GetSlotAtIndex(armorSlotIndex);
            if (slot && slot->pItemData) {
                armorName = slot->pItemData->Name;
            }
        }
        m_renderer.DrawString(infoX, infoY++, L" 방어구    : " + armorName);

        // 3. 장신구 정보 가져오기
        std::wstring accessoryName = L"없음";
        int accessorySlotIndex = m_player.GetEquippedAccessorySlot();
        if (accessorySlotIndex != -1)
        {
            const InventorySlot* slot = inventory->GetSlotAtIndex(accessorySlotIndex);
            if (slot && slot->pItemData) {
                accessoryName = slot->pItemData->Name;
            }
        }
        m_renderer.DrawString(infoX, infoY++, L" 장신구    : " + accessoryName);
    }
}

void PauseMenu::DrawMainMenuOptions()
{
    // 그리기 전, 오른쪽 패널을 지움
    ClearRightPane();

    const std::vector<std::wstring> options = { L"스탯 분배", L"인벤토리", L"스킬북", L"저장", L"메인메뉴로 나가기" };

    // --- 레이아웃 좌표 및 박스 너비 설정 ---
    int boxX = m_renderer.GetWidth() / 2 + 5;
    int boxY = 10;
    int boxInnerWidth = 24; // 박스 내부 너비 (텍스트와 공백, 커서가 들어갈 공간)

    for (size_t i = 0; i < options.size(); ++i)
    {
        // 각 메뉴 아이템은 3줄의 높이를 차지합니다 (윗 테두리, 내용, 아래 테두리)
        int currentY = boxY + i * 3;

        // --- 박스 테두리 그리기 ---
        m_renderer.DrawString(boxX, currentY, L"┌" + std::wstring(boxInnerWidth, L'─') + L"┐");
        m_renderer.DrawString(boxX, currentY + 2, L"└" + std::wstring(boxInnerWidth, L'─') + L"┘");

        // --- 박스 내용 그리기 ---
        m_renderer.DrawString(boxX, currentY + 1, L"│");
        m_renderer.DrawString(boxX + boxInnerWidth + 1, currentY + 1, L"│");
        m_renderer.DrawString(boxX + 2, currentY + 1, options[i]);

        if (i == m_mainMenuSelection)
        {
            m_renderer.DrawString(boxX + boxInnerWidth - 2, currentY + 1, L"◀");
        }
    }
}

void PauseMenu::DrawStatDistributionScreen()
{
    // 그리기 전, 오른쪽 패널을 지움
    ClearRightPane();

    AttributeSet* stats = m_player.GetAbilityComponent()->GetAttributeSet();
    if (!stats) return;

    // --- 레이아웃 좌표 및 박스 너비 설정 ---
    int boxX = m_renderer.GetWidth() / 2 + 5;
    int boxY = 10;
    int boxInnerWidth = 24;

    // --- 박스 그리기 ---
    std::wstring title = L" 스탯 분배 ";
    int titleWidth = 0;
    for (wchar_t c : title) { titleWidth += (c >= L'가' && c <= L'힣') ? 2 : 1; }
    int padding = (boxInnerWidth - titleWidth) / 2;
    m_renderer.DrawString(boxX, boxY, L"┌" + std::wstring(padding, L'─') + title + std::wstring(boxInnerWidth - padding - titleWidth, L'─') + L"┐");

    for (int i = 1; i < 13; ++i) // 박스 높이를 13으로 설정
    {
        m_renderer.DrawString(boxX, boxY + i, L"│" + std::wstring(boxInnerWidth, L' ') + L"│");
    }
    m_renderer.DrawString(boxX, boxY + 13, L"└" + std::wstring(boxInnerWidth, L'─') + L"┘");

    // --- 내용 채우기 ---
    int contentX = boxX + 2;
    int contentY = boxY + 2;

    // 분배 가능 포인트 표시
    std::wstring pointText = L"분배 가능한 포인트: " + std::to_wstring(stats->AdditionalStatPoints);
    m_renderer.DrawString(contentX, contentY, pointText);

    // 구분선
    m_renderer.DrawString(boxX, contentY + 2, L"├" + std::wstring(boxInnerWidth, L'─') + L"┤");

    // 스탯 목록 표시
    contentY += 4;
    const std::vector<std::wstring> statNames = { L" 힘", L" 민첩", L" 지능" };
    const std::vector<float> statValues = { stats->Strength.CurrentValue, stats->Agility.CurrentValue, stats->Intelligence.CurrentValue };

    for (size_t i = 0; i < statNames.size(); ++i)
    {
        int currentStatY = contentY + i * 2;
        std::wstring cursor = L"  ";
        if (i == m_statSelection)   cursor = L"▶ ";

        // 스탯 이름 (왼쪽 정렬)
        m_renderer.DrawString(contentX, currentStatY, cursor + statNames[i]);
        // 스탯 값 (오른쪽 정렬)
        std::wstring valueStr = std::to_wstring(static_cast<int>(statValues[i]));
        m_renderer.DrawString(boxX + boxInnerWidth - 1 - valueStr.length(), currentStatY, valueStr);
    }

    // 하단 안내 메시지 표시
    m_renderer.DrawString(m_renderer.GetWidth() / 2 + 3, m_renderer.GetHeight() - 6, L"[Enter: 포인트 분배 | ESC: 뒤로 가기]");

}

void PauseMenu::DrawSkillBookScreen()
{
    // 그리기 전, 오른쪽 패널을 깨끗하게 지움
    ClearRightPane();

    // --- 레이아웃 좌표 설정 ---
    int boxX = m_renderer.GetWidth() / 2 + 5;
    int boxY = 8;
    int boxInnerWidth = 24;
    int boxHeight = 4; // 각 슬롯 박스의 높이

    m_renderer.DrawString(boxX, boxY, L"    [ 전투 스킬 장착 ]");
    boxY += 2; // 타이틀 아래로 Y좌표 이동

    // 4개의 장착 슬롯
    for (int i = 0; i < 4; ++i)
    {
        int currentY = boxY + i * (boxHeight + 1); // 박스 사이 간격 1 추가
        if (i == m_skillBookSlotSelection)
        {
            // 2. 선택되었다면, 박스 왼쪽에 커서(▶)
            m_renderer.DrawString(boxX - 2, currentY + (boxHeight / 2), L"▶");
        }

        // 박스 테두리 그리기
        m_renderer.DrawString(boxX, currentY, L"┌" + std::wstring(boxInnerWidth, L'─') + L"┐");
        for (int h = 1; h < boxHeight; ++h) m_renderer.DrawString(boxX, currentY + h, L"│" + std::wstring(boxInnerWidth, L' ') + L"│");
        m_renderer.DrawString(boxX, currentY + boxHeight, L"└" + std::wstring(boxInnerWidth, L'─') + L"┘");

        // 박스 안에 내용 채우기
        GameplayAbility* equipped = m_player.GetAbilityComponent()->GetEquippedAbilities()[i];
        std::wstring skillName = equipped ? equipped->AbilityName : L"( 비어있음 )";
        std::wstring costText = equipped ? L"MP " + std::to_wstring(static_cast<int>(equipped->ManaCost)) : L"";

        m_renderer.DrawString(boxX + 2, currentY + 1, L"슬롯 " + std::to_wstring(i + 1));
        m_renderer.DrawString(boxX + 2, currentY + 2, skillName); // 스킬 이름
        m_renderer.DrawString(boxX + boxInnerWidth - costText.length(), currentY + 3, costText); // 마나 소모량
    }

    // 하단 안내 메시지
    m_renderer.DrawString(m_renderer.GetWidth() / 2 + 3, m_renderer.GetHeight() - 2, L"[Enter: 스킬 변경 | ESC: 뒤로 가기]");
    // 하단 안내 문구
}

void PauseMenu::DrawSkillSelectionScreen()
{
    ClearRightPane();

    int listX = m_renderer.GetWidth() / 2 + 5;
    int listY = 8;
    m_renderer.DrawString(listX, listY++, L"[ 장착할 스킬 선택 ]");
    listY += 2;

    const auto& grantedAbilities = m_player.GetAbilityComponent()->GetGrantedAbilities();
    int abilityCount = static_cast<int>(grantedAbilities.size());

    // --- 스크롤되는 스킬 목록 출력 ---
    if (grantedAbilities.empty())
    {
        m_renderer.DrawString(listX, listY - 1, L"보유한 스킬이 없습니다.");
    }
    else
    {
        // 스크롤바 위쪽 화살표 표시
        if (m_skillListScrollOffset > 0)
        {
            m_renderer.DrawString(listX + 10, listY - 1, L"▲");
        }

        // 화면에 보여줄 개수만큼만 반복
        for (int i = 0; i < m_skillListVisibleCount; ++i)
        {
            int skillIndex = m_skillListScrollOffset + i;
            if (skillIndex >= abilityCount) break;

            GameplayAbility* currentSkill = grantedAbilities[skillIndex].get();

            // 커서와 스킬 기본 텍스트를 준비
            std::wstring cursor = (skillIndex == m_skillSelectionListCursor) ? L"▶  " : L"   ";
            std::wstring skillText = cursor + currentSkill->AbilityName;

            bool bIsEquipped = IsSkillEquipped(currentSkill);

            // 장착된 스킬이면, 텍스트 뒤에 "[장착중]" 라벨을 추가
            if (bIsEquipped)
            {
                skillText += L" [장착중]";
            }

            // 최종 skillText 출력
            m_renderer.DrawString(listX + 2, listY + i + 1, skillText);
        }

        // 스크롤바 아래쪽 화살표 표시
        if (m_skillListScrollOffset + m_skillListVisibleCount < abilityCount)
        {
            m_renderer.DrawString(listX + 10, listY + m_skillListVisibleCount + 2, L"▼");
        }
    }

    // 선택된 스킬 정보창 출력
    GameplayAbility* highlightedSkill = nullptr;
    if (!grantedAbilities.empty())
    {
        highlightedSkill = grantedAbilities[m_skillSelectionListCursor].get();
    }

    if (highlightedSkill)
    {
        int boxY = m_renderer.GetHeight() - 9;
        int boxX = m_renderer.GetWidth() / 2 + 3;
        int boxWidth = m_renderer.GetWidth() - boxX - 10;

        m_renderer.DrawString(boxX, boxY, L"┌" + std::wstring(boxWidth, L'─') + L"┐");
        m_renderer.DrawString(boxX, boxY + 1, L"│" + std::wstring(boxWidth, L' ') + L"│");
        m_renderer.DrawString(boxX, boxY + 2, L"│" + std::wstring(boxWidth, L' ') + L"│");
        m_renderer.DrawString(boxX, boxY + 3, L"└" + std::wstring(boxWidth, L'─') + L"┘");

        int contentX = boxX + 2;
        int contentY = boxY + 1;

        std::wstring title = L"[" + highlightedSkill->AbilityName + L"]";
        std::wstring mana = L"MP " + std::to_wstring(static_cast<int>(highlightedSkill->ManaCost));
        m_renderer.DrawString(contentX, contentY, title);
        m_renderer.DrawString(contentX + boxWidth - mana.length() - 1, contentY, mana);

        m_renderer.DrawString(contentX, contentY + 1, highlightedSkill->AbilityDescription);
    }
}

void PauseMenu::DrawShopOptions()
{
    // 그리기 전, 오른쪽 패널을 지움
    ClearRightPane();

    const std::vector<std::wstring> options = { L"구매", L"판매", L"나가기" };

    // --- 레이아웃 좌표 및 박스 너비 설정 ---
    int boxX = m_renderer.GetWidth() / 2 + 5;
    int boxY = 10;
    int boxInnerWidth = 24; // 박스 내부 너비 (텍스트와 공백, 커서가 들어갈 공간)

    for (size_t i = 0; i < options.size(); ++i)
    {
        // 각 메뉴 아이템은 3줄의 높이를 차지합니다 (윗 테두리, 내용, 아래 테두리)
        int currentY = boxY + i * 3;

        // --- 박스 테두리 그리기 ---
        m_renderer.DrawString(boxX, currentY, L"┌" + std::wstring(boxInnerWidth, L'─') + L"┐");
        m_renderer.DrawString(boxX, currentY + 2, L"└" + std::wstring(boxInnerWidth, L'─') + L"┘");

        // --- 박스 내용 그리기 ---
        m_renderer.DrawString(boxX, currentY + 1, L"│");
        m_renderer.DrawString(boxX + boxInnerWidth + 1, currentY + 1, L"│");
        m_renderer.DrawString(boxX + 2, currentY + 1, options[i]);

        if (i == m_shopSelection)
        {
            m_renderer.DrawString(boxX + boxInnerWidth - 2, currentY + 1, L"◀");
        }
    }
}

void PauseMenu::DrawShopBuyScreen()
{
    ClearRightPane();

    // 1. DataManager에서 상점 정보를 가져옵니다.
    const ShopData* shopData = DataManager::GetInstance().GetShopData("vendor_general");
    AttributeSet* playerStats = m_player.GetAbilityComponent()->GetAttributeSet();
    if (!shopData || !playerStats) return;

    const auto& itemsForSale = shopData->itemIds;
    const int totalItems = static_cast<int>(itemsForSale.size());

    // --- 2. 레이아웃 및 스크롤 설정 ---
    int boxX = m_renderer.GetWidth() / 2 + 5;
    int boxY = 7;
    int boxInnerWidth = 26;
    int boxHeight = 3;

    m_renderer.DrawString(boxX, boxY, L"      [ 상점 - 구매 ]");
    boxY += 2;

    // 스크롤바 그리기
    if (m_shopBuyScrollOffset > 0) {
        m_renderer.DrawString(boxX + 15, boxY, L"▲");
    }
    if (m_shopBuyScrollOffset + m_inventoryVisibleCount < totalItems) {
        m_renderer.DrawString(boxX + 15, boxY + (m_inventoryVisibleCount * (boxHeight + 1)), L"▼");
    }

    // --- 3. 판매 아이템 목록 그리기 ---
    for (int i = 0; i < m_inventoryVisibleCount; ++i)
    {
        int itemIndex = m_shopBuyScrollOffset + i;
        if (itemIndex >= totalItems) break;

        int currentY = boxY + i * (boxHeight + 1);

        // 커서는 m_shopBuyCursor 변수를 기준으로 그림
        if (itemIndex == m_shopBuyCursor)
        {
            m_renderer.DrawString(boxX - 2, currentY + (boxHeight / 2), L"▶");
        }

        m_renderer.DrawString(boxX, currentY, L"┌" + std::wstring(boxInnerWidth, L'─') + L"┐");
        for (int h = 1; h < boxHeight; ++h) m_renderer.DrawString(boxX, currentY + h, L"│" + std::wstring(boxInnerWidth, L' ') + L"│");
        m_renderer.DrawString(boxX, currentY + boxHeight, L"└" + std::wstring(boxInnerWidth, L'─') + L"┘");

        const ItemData* data = DataManager::GetInstance().GetItemData(itemsForSale[itemIndex]);
        if (data)
        {
            m_renderer.DrawString(boxX + 2, currentY + 1, data->Name);

            // 구매 가격 표시
            std::wstring priceStr = std::to_wstring(data->Price) + L" G";
            m_renderer.DrawString(boxX + boxInnerWidth - priceStr.length(), currentY + 2, priceStr);
        }
    }

    const ItemData* selectedItemData = nullptr; // 기본적으로는 비어있음

    // 현재 커서가 유효한 아이템을 가리키고 있는지 확인
    if (m_shopBuyCursor >= 0 && m_shopBuyCursor < shopData->itemIds.size()) {
        selectedItemData = DataManager::GetInstance().GetItemData(shopData->itemIds[m_shopBuyCursor]);
    }

    m_renderer.DrawString(m_renderer.GetWidth() / 2 + 3, m_renderer.GetHeight() - 2, L"[Enter: 구매 | ESC: 뒤로 가기]");

    // 아이템 정보창은 항상 그림
    DrawItemInfoBox(selectedItemData, false);

    // 피드백 메시지 표시
    if (!m_feedbackMessage.empty() && GetTickCount64() < m_feedbackMessageEndTime)
    {
        int msgX = m_renderer.GetWidth() / 2 + 5;
        int msgY = m_renderer.GetHeight() - 4; // 하단 안내 문구 바로 위에 표시

        m_renderer.DrawString(msgX, msgY, m_feedbackMessage);
    }
    else
    {
        m_feedbackMessage.clear();
    }
}

void PauseMenu::DrawShopBuyActionMenu()
{
    int boxX = m_renderer.GetWidth() / 2 + 35;
    int boxY = 9 + (m_shopBuyCursor - m_shopBuyScrollOffset) * 4;
    int boxWidth = 15;
    int boxHeight = static_cast<int>(m_currentItemActions.size()) + 1;

    m_renderer.DrawString(boxX, boxY, L"┌" + std::wstring(boxWidth, L'─') + L"┐");
    for (int i = 1; i <= boxHeight; ++i) {
        m_renderer.DrawString(boxX, boxY + i, L"│" + std::wstring(boxWidth, L' ') + L"│");
    }
    m_renderer.DrawString(boxX, boxY + boxHeight + 1, L"└" + std::wstring(boxWidth, L'─') + L"┘");

    for (size_t i = 0; i < m_currentItemActions.size(); ++i)
    {
        std::wstring line = m_currentItemActions[i];
        if (i == m_itemActionCursor)
        {
            line = L"▶  " + line;
            m_renderer.DrawString(boxX + 2, boxY + 1 + i, line);
        }
        else
        {
            line = L"    " + line;
            m_renderer.DrawString(boxX + 2, boxY + 1 + i, line);
        }
    }
}

void PauseMenu::DrawInventoryScreen(bool bIsSellMode)
{
    ClearRightPane();

    InventoryComponent* inventory = m_player.GetInventory();
    if (!inventory) return;

    int boxX = m_renderer.GetWidth() / 2 + 5;
    int boxY = 7; // 6
    int boxInnerWidth = 26;
    int boxHeight = 3;
    const int visibleSlots = 5; // 한 화면에 보여줄 아이템 개수

    m_renderer.DrawString(boxX, boxY, L"      [ 인벤토리 ]");
    boxY += 2;

    const int totalSlots = inventory->GetCapacity();

    // 스크롤바 그리기
    if (m_inventoryScrollOffset > 0) {
        m_renderer.DrawString(boxX + 15, boxY - 1, L"▲");
    }
    if (m_inventoryScrollOffset + visibleSlots < totalSlots) {
        m_renderer.DrawString(boxX + 15, boxY + (visibleSlots * (boxHeight + 1)), L"▼");
    }

    // 아이템 목록 그리기
    for (int i = 0; i < visibleSlots; ++i)
    {
        int slotIndex = m_inventoryScrollOffset + i;
        if (slotIndex >= totalSlots) break;

        int currentY = boxY + i * (boxHeight + 1);

        // 커서는 m_inventorySlotSelection 변수를 기준으로 그림
        if (slotIndex == m_inventorySlotSelection)
        {
            m_renderer.DrawString(boxX - 2, currentY + (boxHeight / 2), L"▶");
        }

        m_renderer.DrawString(boxX, currentY, L"┌" + std::wstring(boxInnerWidth, L'─') + L"┐");
        for (int h = 1; h < boxHeight; ++h) m_renderer.DrawString(boxX, currentY + h, L"│" + std::wstring(boxInnerWidth, L' ') + L"│");
        m_renderer.DrawString(boxX, currentY + boxHeight, L"└" + std::wstring(boxInnerWidth, L'─') + L"┘");

        const InventorySlot* slot = inventory->GetSlotAtIndex(slotIndex);
        if (slot && slot->Quantity > 0)
        {
            const ItemData* data = DataManager::GetInstance().GetItemData(slot->ItemID);
            if (data)
            {
                std::wstring typeStr = L"";
                if (data->Type == EItemType::Equipment) typeStr = L"[장비]";
                else if (data->Type == EItemType::Consumable) typeStr = L"[소모품]";

                std::wstring line = data->Name;

                if (slot->bIsEquipped) line += L" [E]";

                line += L" " + typeStr;

                m_renderer.DrawString(boxX + 2, currentY + 1, line);

                std::wstring quantityStr = L"수량: " + std::to_wstring(slot->Quantity);
                m_renderer.DrawString(boxX + 2, currentY + 2, quantityStr);
            }
        }
        else
        {
            m_renderer.DrawString(boxX + 2, currentY + 1, L"( 비어있음 )");
        }
    }
    
    const InventorySlot* selectedSlot = inventory->GetSlotAtIndex(m_inventorySlotSelection);
    const ItemData* selectedItemData = nullptr;
    if (selectedSlot)
    {
        selectedItemData = selectedSlot->pItemData; // 슬롯에 캐싱된 포인터를 사용
    }

    // 아이템 정보창 및 피드백 메시지 처리
    m_renderer.DrawString(m_renderer.GetWidth() / 2 + 3, m_renderer.GetHeight() - 2, L"[Enter: 선택 | ESC: 뒤로 가기]");

    DrawItemInfoBox(selectedSlot ? selectedSlot->pItemData : nullptr, bIsSellMode);
}

void PauseMenu::DrawInventoryActionMenu()
{
    const InventorySlot* slot = m_player.GetInventory()->GetSlotAtIndex(m_inventorySlotSelection);
    if (!slot || slot->Quantity <= 0) return;

    int boxX = m_renderer.GetWidth() / 2 + 35;
    int boxY = 8 + (m_inventorySlotSelection - m_inventoryScrollOffset) * 4;
    int boxWidth = 15;
    int boxHeight = static_cast<int>(m_currentItemActions.size()) + 1;

    m_renderer.DrawString(boxX, boxY, L"┌" + std::wstring(boxWidth, L'─') + L"┐");
    for (int i = 1; i <= boxHeight; ++i) {
        m_renderer.DrawString(boxX, boxY + i, L"│" + std::wstring(boxWidth, L' ') + L"│");
    }
    m_renderer.DrawString(boxX, boxY + boxHeight + 1, L"└" + std::wstring(boxWidth, L'─') + L"┘");

    for (size_t i = 0; i < m_currentItemActions.size(); ++i)
    {
        std::wstring line = m_currentItemActions[i];
        if (i == m_itemActionCursor)
        {
            line = L"▶  " + line;
            //m_renderer.DrawString(boxX + 2, boxY + 1 + i, line, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            m_renderer.DrawString(boxX + 2, boxY + 1 + i, line);
            // 제 기준 한글 깨져서 주석 처리
        }
        else
        {
            line = L"   " + line;
            m_renderer.DrawString(boxX + 2, boxY + 1 + i, line);
        }
    }
}

void PauseMenu::DrawInventoryDropPrompt()
{
    const InventorySlot* slot = m_player.GetInventory()->GetSlotAtIndex(m_inventorySlotSelection);
    if (!slot || !slot->pItemData) return;

    // 레이아웃 좌표 계산 (화면 중앙에 작은 박스)
    int boxWidth = 40;
    int boxHeight = 4;
    int boxX = (m_renderer.GetWidth() - boxWidth) / 2;
    int boxY = (m_renderer.GetHeight() - boxHeight) / 2;

    // 반투명한 배경처럼 보이게 하기 위해 회색으로 클리어 (선택 사항)
    // for (int y = boxY; ... )

    // 박스 테두리 그리기
    m_renderer.DrawString(boxX, boxY, L"┌" + std::wstring(boxWidth, L'─') + L"┐");
    for (int i = 1; i <= boxHeight; ++i) m_renderer.DrawString(boxX, boxY + i, L"│" + std::wstring(boxWidth, L' ') + L"│");
    m_renderer.DrawString(boxX, boxY + boxHeight + 1, L"└" + std::wstring(boxWidth, L'─') + L"┘");

    int contentX = boxX + 2;
    int contentY = boxY + 1;

    std::wstring promptText;

    switch (m_quantityContext)
    {
    case EQuantityInputContext::Buy:
    {
        // 구매 문맥일 때의 텍스트
        const ShopData* shopData = DataManager::GetInstance().GetShopData("vendor_general");
        if (shopData && m_shopBuyCursor < shopData->itemIds.size()) {
            const ItemData* itemData = DataManager::GetInstance().GetItemData(shopData->itemIds[m_shopBuyCursor]);
            if (itemData) {
                promptText = L"구매할 수량을 입력하세요:";
            }
        }
        break;
    }
    case EQuantityInputContext::Sell:
    case EQuantityInputContext::Drop:
    {
        // 판매 또는 버리기 문맥일 때의 텍스트 (기존 로직)
        const InventorySlot* slot = m_player.GetInventory()->GetSlotAtIndex(m_inventorySlotSelection);
        if (slot && slot->pItemData) {
            promptText = L"수량 입력 (1-" + std::to_wstring(slot->Quantity) + L"):";
        }
        break;
    }
    }
    m_renderer.DrawString(contentX, contentY, promptText);
    m_renderer.DrawString(contentX, contentY + 2, L"> " + m_dropQuantityInput);

    if (GetTickCount64() / 500 % 2 == 0) {
        m_renderer.Draw(contentX + 2 + m_dropQuantityInput.length(), contentY + 2, L'█');
    }
}

void PauseMenu::DrawItemInfoBox(const ItemData* data, bool bIsSellMode)
{
	if (!data) return;

    // --- 레이아웃 및 박스 그리기 ---
    int boxY = m_renderer.GetHeight() - 9;
    int boxX = m_renderer.GetWidth() / 2 + 3;
    int boxWidth = m_renderer.GetWidth() - boxX - 10;

    m_renderer.DrawString(boxX, boxY, L"┌" + std::wstring(boxWidth, L'─') + L"┐");
    for (int i = 1; i < 5; ++i) {
        m_renderer.DrawString(boxX, boxY + i, L"│" + std::wstring(boxWidth, L' ') + L"│");
    }
    m_renderer.DrawString(boxX, boxY + 5, L"└" + std::wstring(boxWidth, L'─') + L"┘");

    // --- 박스 안에 내용 채우기 ---
    int contentX = boxX + 2;
    int contentY = boxY + 1;

    // 2. 이제 'data'는 함수 내부에서 새로 선언하는 것이 아니라,
    //    매개변수로 받은 'data'를 그대로 사용합니다.
    std::wstring title = L"[" + data->Name + L"]";
    m_renderer.DrawString(contentX, contentY, title);

    float priceToShow = bIsSellMode ? (data->Price * 0.6f) : data->Price;
    std::wstring price = std::to_wstring(static_cast<int>(priceToShow)) + L" G";
    m_renderer.DrawString(contentX + boxWidth - price.length() - 1, contentY, price);
    contentY++;

    m_renderer.DrawString(contentX, contentY, data->Description);
    contentY += 2;

    // --- 3. 아이템 타입에 따라 다른 상세 정보 표시 ---

    // EStatType을 한글 이름으로 바꿔주는 작은 '번역기' 람다 함수
    auto StatTypeToWString = [](EStatType type) -> std::wstring {
        switch (type) {
        case EStatType::HP:                             return L"체력";
        case EStatType::MP:                             return L"마나";
        case EStatType::STRENGTH:                       return L"힘";
        case EStatType::AGILITY:                        return L"민첩";
        case EStatType::INTELLIGENCE:                   return L"지능";
        case EStatType::DEFENCE:                        return L"방어력";
        case EStatType::MAGIC_RESISTANCE:               return L"마법저항력";
        case EStatType::CRITICAL_HIT_CHANCE:            return L"치명타 확률";
        case EStatType::CRITICAL_HIT_DAMAGE_MULTIPLIER: return L"치명타 피해 증가";
        default:                                        return L"알수없는 스탯";
        }
        };

    if (data->Type == EItemType::Equipment)
    {
        std::wstringstream wss; // 여러 스탯을 하나의 문자열로 만들기 위해 사용
        wss << L"착용 효과: ";
        for (const auto& bonus : data->EquipmentData.StatBonuses)
        {
            wss << StatTypeToWString(bonus.first) << L" ";
            if (bonus.second > 0) {
                wss << L"+" << static_cast<int>(bonus.second);
            }
            else {
                wss << static_cast<int>(bonus.second);
            }
            wss << L" / ";
        }
        std::wstring statText = wss.str();
        // 마지막 " / " 제거
        if (!statText.empty()) {
            statText.pop_back();
            statText.pop_back();
        }
        m_renderer.DrawString(contentX, contentY, statText);
    }
    else if (data->Type == EItemType::Consumable)
    {
        const FConsumableEffectData& effect = data->EffectData;
        std::wstringstream wss;
        wss << L"사용 효과: ";

        if (effect.Type == "HEAL_HP")
        {
            wss << L"체력 " << static_cast<int>(effect.Value) << L" 회복";
        }
        else if (effect.Type == "BUFF_STAT")
        {
            wss << effect.Duration << L"턴 동안 " << StatTypeToWString(effect.TargetStat)
                << L" " << static_cast<int>(effect.Value) << L" 증가";
        }
        m_renderer.DrawString(contentX, contentY, wss.str());
    }
}


// --- 헬퍼 함수 ---
void PauseMenu::ClearRightPane()
{
    // 레이아웃 좌표 계산
    int panelX = 4, panelY = 2;
    int panelWidth = m_renderer.GetWidth() - (panelX * 2);
    int panelHeight = m_renderer.GetHeight() - (panelY * 2) - 1;
    int midX = panelX + panelWidth / 2;

    // 오른쪽 패널의 '내용' 부분만 정확히 클리어
    int contentStartX = midX + 1;
    int contentStartY = panelY + 5;
    int contentWidth = panelX + panelWidth - contentStartX - 1;
    int contentHeight = panelY + panelHeight - contentStartY;

    for (int i = 0; i < contentHeight; ++i)
    {
        m_renderer.DrawString(contentStartX, contentStartY + i, std::wstring(contentWidth, L' '));
    }
}

bool PauseMenu::IsSkillEquipped(const GameplayAbility* skill) const
{
    if (!skill) return false; // 스킬 포인터가 유효하지 않으면 false

    // 현재 장착된 스킬 목록을 가져옴
    const auto& equippedAbilities = m_player.GetAbilityComponent()->GetEquippedAbilities();

    // 장착된 스킬 목록에 찾는 스킬이 있는지 확인
    for (const auto& equippedSkill : equippedAbilities)
    {
        if (equippedSkill == skill)
        {
            return true; // 찾았으면 true
        }
    }
    return false; // 못 찾았으면 false
}

void PauseMenu::BuildInventoryActionMenu(bool bIsSellMode)
{
    m_currentItemActions.clear();
    const InventorySlot* slot = m_player.GetInventory()->GetSlotAtIndex(m_inventorySlotSelection);
    if (!slot || slot->Quantity <= 0) return;

    if (bIsSellMode)
    {
        m_currentItemActions.push_back(L"판매");
    }
    else
    {
        const ItemData* data = slot->pItemData;
        if (data)
        {
            if (data->Type == EItemType::Consumable) m_currentItemActions.push_back(L"사용하기");
            else if (data->Type == EItemType::Equipment)
            {
                if (slot->bIsEquipped) m_currentItemActions.push_back(L"장착해제");
                else m_currentItemActions.push_back(L"장착하기");
            }
        }
        m_currentItemActions.push_back(L"버리기");
    }
    m_currentItemActions.push_back(L"취소");
}

