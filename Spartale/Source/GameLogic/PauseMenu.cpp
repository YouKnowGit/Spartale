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
    m_shopSelection(0),
    m_statSelection(0),
    m_skillBookSlotSelection(0),
    m_skillSelectionListCursor(0),
    m_slotIndexToModify(0),
    m_skillListScrollOffset(0),
    m_inventorySlotSelection(0),
    m_inventoryScrollOffset(0),
    m_itemActionCursor(0)

{
    m_navigateSoundPath = L"Sounds/UI/pausemenu_select.wav"; // 메뉴 이동 효과음
    m_confirmSoundPath = L"Sounds/UI/pausemenu_confirm.wav";   // 메뉴 선택 효과음
    m_rejectSoundPath = L"Sounds/UI/menu_reject2.wav";   // 메뉴 선택 효과음
    m_escSoundPath = L"Sounds/UI/mainmenu_select.wav"; // 메뉴 이동 효과음
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
void PauseMenu::Render()
{
    // --- 왼쪽 패널: 캐릭터 정보 (항상 고정) ---
    DrawPlayerInfo();

    // --- 오른쪽 패널: 상태에 따라 다르게 그림 ---
    switch (m_currentPaneState)
    {
    case ERightPaneState::MainMenu:         DrawMainMenuOptions(); break;
    case ERightPaneState::StatDistribution: DrawStatDistributionScreen(); break;
    case ERightPaneState::Inventory:        DrawInventoryScreen(); break;
    case ERightPaneState::InventoryActionSelection: DrawInventoryScreen(); DrawInventoryActionMenu(); break;
    case ERightPaneState::SkillBook:        DrawSkillBookScreen(); break;
    case ERightPaneState::SkillSelection:   DrawSkillSelectionScreen(); break;
    }

    m_renderer.Render();
}

void PauseMenu::RenderShop()
{
    // --- 왼쪽 패널: 캐릭터 정보 (항상 고정) ---
    DrawPlayerInfo();

    // --- 오른쪽 패널: 상태에 따라 다르게 그림 ---
    switch (m_currentShopState)
    {
    case EShopState::MainMenu:         DrawShopOptions(); break;
    case EShopState::Shop_Buy:         DrawStatDistributionScreen(); break;
    case EShopState::Shop_Sell:        DrawInventoryScreen(); break;
    }

    m_renderer.Render();
}

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
        case ERightPaneState::MainMenu: // 메인 메뉴에서 누르면 PauseMenu 종료
            m_bIsRunning = false;
            m_result = EPauseMenuResult::Resume;
            break;
        case ERightPaneState::StatDistribution: // 스탯 분배창에서 누르면 메인 메뉴로 복귀
            m_currentPaneState = ERightPaneState::MainMenu;
            break;
        case ERightPaneState::Inventory: 
            m_currentPaneState = ERightPaneState::MainMenu; 
            break;
        case ERightPaneState::InventoryActionSelection: 
            m_currentPaneState = ERightPaneState::Inventory; 
            break;
        case ERightPaneState::SkillBook:
            m_currentPaneState = ERightPaneState::MainMenu;
            break;
        case ERightPaneState::SkillSelection:
            m_currentPaneState = ERightPaneState::SkillBook;
            break;
        }
        return;
    }

    // 상태에 따라 입력을 분기 처리
    switch (m_currentPaneState)
    {
    case ERightPaneState::MainMenu:         ProcessMainMenuInput(key); break;
    case ERightPaneState::StatDistribution: ProcessStatDistributionInput(key); break;
	case ERightPaneState::Inventory:        ProcessInventoryInput(key); break;
	case ERightPaneState::InventoryActionSelection: ProcessInventoryActionInput(key); break;
    case ERightPaneState::SkillBook:        ProcessSkillBookInput(key); break;
    case ERightPaneState::SkillSelection:   ProcessSkillSelectionInput(key); break;
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
        case EShopState::MainMenu: // 메인 메뉴에서 누르면 PauseMenu 종료
            m_bIsRunning = false;
            m_result = EPauseMenuResult::Resume;
            break;
        case EShopState::Shop_Buy:
            m_currentShopState = EShopState::MainMenu;
            break;
        case EShopState::Shop_Sell:
            m_currentShopState = EShopState::MainMenu;
            break;
        case EShopState::Shop_Exit:
            m_bIsRunning = false;
            m_result = EPauseMenuResult::Resume;
            break;
        }
        return;
    }

    // 상태에 따라 입력을 분기 처리
    switch (m_currentShopState)
    {
        case EShopState::MainMenu:  ProcessMainShopInput(key); break;
    }
}

// --- 입력 처리 함수들 ---
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
<<<<<<< HEAD
        std::wstring cursor = L"  "; 
        if (i == m_statSelection)   cursor = L"▶ ";
=======
        std::wstring cursor = L"  ";
>>>>>>> c91c5f2 (fix: 레벨별 스킬 습득 로직, 스킬 사용 로그)

        // 스탯 이름 (왼쪽 정렬)
        m_renderer.DrawString(contentX, currentStatY, cursor + statNames[i]);
        // 스탯 값 (오른쪽 정렬)
        std::wstring valueStr = std::to_wstring(static_cast<int>(statValues[i]));
        m_renderer.DrawString(boxX + boxInnerWidth - 1 - valueStr.length(), currentStatY, valueStr);
    }

    // 하단 안내 메시지 표시
    m_renderer.DrawString(m_renderer.GetWidth() / 2 + 3, m_renderer.GetHeight() - 6, L"[Enter: 포인트 분배 | ESC: 뒤로 가기]");

}

void PauseMenu::ProcessInventoryInput(int key)
{
    InventoryComponent* inventory = m_player.GetInventory();
    if (!inventory) return;

    const int capacity = inventory->GetCapacity();
    const int visibleSlots = 8; // DrawInventoryScreen과 동일한 값

    if (key == 224) // 방향키
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
                m_inventoryScrollOffset = (std::max)(0, capacity - visibleSlots);
            }
        }
        else if (key == 80) // 아래
        {
            m_inventorySlotSelection = (m_inventorySlotSelection + 1) % capacity;
            // 스크롤 조정
            if (m_inventorySlotSelection >= m_inventoryScrollOffset + visibleSlots) {
                m_inventoryScrollOffset = m_inventorySlotSelection - visibleSlots + 1;
            }
            if (m_inventorySlotSelection == 0) {
                m_inventoryScrollOffset = 0;
            }
        }
    }
    else if (key == 13) // 엔터
    {
        // 커서 변수 통일
        const InventorySlot* slot = m_player.GetInventory()->GetSlotAtIndex(m_inventorySlotSelection);
        if (slot && slot->Quantity > 0)
        {
            PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);

            m_currentItemActions.clear();
            const ItemData* data = DataManager::GetInstance().GetItemData(slot->ItemID);
            if (data)
            {
                if (data->Type == EItemType::Consumable) m_currentItemActions.push_back(L"사용하기");
                if (data->Type == EItemType::Equipment) m_currentItemActions.push_back(L"장착하기");
            }
            m_currentItemActions.push_back(L"버리기");
            m_currentItemActions.push_back(L"취소");

            m_currentPaneState = ERightPaneState::InventoryActionSelection;
            m_itemActionCursor = 0;
        }
        else
        {
            PlaySound(m_rejectSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
        }
    }
}

void PauseMenu::DrawItemInfoBox()
{
    InventoryComponent* inventory = m_player.GetInventory();
    if (!inventory) return;

    // 현재 커서가 가리키는 슬롯의 정보를 가져옵니다.
    const InventorySlot* slot = inventory->GetSlotAtIndex(m_inventorySlotSelection);
    if (!slot || slot->Quantity == 0) return; // 슬롯이 비어있거나 아이템이 없으면 그리지 않음

    const ItemData* data = DataManager::GetInstance().GetItemData(slot->ItemID);
    if (!data) return;

    // --- 1. 레이아웃 및 박스 그리기 ---
    int boxY = m_renderer.GetHeight() - 9;
    int boxX = m_renderer.GetWidth() / 2 + 3;
    int boxWidth = m_renderer.GetWidth() - boxX - 10;

    // 박스 테두리 그리기
    m_renderer.DrawString(boxX, boxY, L"┌" + std::wstring(boxWidth, L'─') + L"┐");
    for (int i = 1; i < 5; ++i) {
        m_renderer.DrawString(boxX, boxY + i, L"│" + std::wstring(boxWidth, L' ') + L"│");
    }
    m_renderer.DrawString(boxX, boxY + 5, L"└" + std::wstring(boxWidth, L'─') + L"┘");

    // --- 2. 박스 안에 내용 채우기 ---
    int contentX = boxX + 2;
    int contentY = boxY + 1;

    // 아이템 이름과 가격 표시
    std::wstring title = L"[" + data->Name + L"]";
    std::wstring price = std::to_wstring(data->Price) + L" G";
    m_renderer.DrawString(contentX, contentY, title);
    m_renderer.DrawString(contentX + boxWidth - price.length() - 1, contentY, price);
    contentY++;

    // 아이템 설명 표시
    m_renderer.DrawString(contentX, contentY, data->Description);
    contentY += 2; // 설명과 효과 사이에 한 줄 띄우기

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

void PauseMenu::DrawInventoryScreen()
{
    ClearRightPane();

    InventoryComponent* inventory = m_player.GetInventory();
    if (!inventory) return;

    int boxX = m_renderer.GetWidth() / 2 + 5;
<<<<<<< HEAD
    int boxY = 7; // 6
=======
    int boxY = 9; // 6
>>>>>>> c91c5f2 (fix: 레벨별 스킬 습득 로직, 스킬 사용 로그)
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

                m_renderer.DrawString(boxX + 2, currentY + 1, data->Name + L" " + typeStr);

                std::wstring quantityStr = L"수량: " + std::to_wstring(slot->Quantity);
                m_renderer.DrawString(boxX + 2, currentY + 2, quantityStr);
            }
        }
        else
        {
            m_renderer.DrawString(boxX + 2, currentY + 1, L"( 비어있음 )");
        }
    }

    m_renderer.DrawString(m_renderer.GetWidth() / 2 + 3, m_renderer.GetHeight() - 2, L"[Enter: 선택 | ESC: 뒤로 가기]");
    // 하단 안내 문구

    // 아이템 정보창은 항상 그림
    DrawItemInfoBox();
}

void PauseMenu::DrawInventoryActionMenu()
{
    const InventorySlot* slot = m_player.GetInventory()->GetSlotAtIndex(m_inventorySlotSelection);
    if (!slot || slot->Quantity <= 0) return;

    // --- 1. 레이아웃 좌표 계산 ---
    // 커서 위치 근처에 작은 박스를 그립니다.
    int boxX = m_renderer.GetWidth() / 2 + 35;
    int boxY = 8 + (m_inventorySlotSelection - m_inventoryScrollOffset) * 4;
    int boxWidth = 15;
    int boxHeight = static_cast<int>(m_currentItemActions.size()) + 1;

    // --- 2. 박스 테두리 그리기 ---
    m_renderer.DrawString(boxX, boxY, L"┌" + std::wstring(boxWidth, L'─') + L"┐");
    for (int i = 1; i <= boxHeight; ++i) {
        m_renderer.DrawString(boxX, boxY + i, L"│" + std::wstring(boxWidth, L' ') + L"│");
    }
    m_renderer.DrawString(boxX, boxY + boxHeight + 1, L"└" + std::wstring(boxWidth, L'─') + L"┘");

    // --- 3. 박스 안에 행동 목록 그리기 ---
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

// 스킬북) 스킬이 장착이 된 상태인지 판별하는 함수
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

// PauseMenu 전체 입력 처리 함수
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
        case 0: m_currentShopState = EShopState::Shop_Buy; m_statSelection = 0; break;
        case 1: m_currentShopState = EShopState::Shop_Sell; m_inventorySlotSelection = 0; break;
        case 2: m_currentShopState = EShopState::Shop_Exit; m_bIsRunning = false; m_result = EPauseMenuResult::Resume; break;
        }
    }
}

// Pause 메뉴 좌측 캐릭터 정보 화면
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
}

// 좌측 상태창: 체력, 마나, 경험치 바 그리는 함수
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


// 우측 화면 지우는 함수
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

        if (selectedAction == L"사용하기" || selectedAction == L"장착하기")
        {
            // TODO: 아이템 사용/장착 로직 호출
            // m_player.GetInventory()->UseItem(m_inventorySlotSelection, &m_player);
        }
        else if (selectedAction == L"버리기")
        {
            // TODO: 아이템 버리기 로직 호출
        }

        // 행동 후에는 인벤토리 목록으로 돌아감
        m_currentPaneState = ERightPaneState::Inventory;
    }
}