#include "GameLogic/PauseMenu.h"
#include "Utils/ConsoleRenderer.h"
#include "GameLogic/Units/Player.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/AttributeSet.h"

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
    m_currentPaneState(ERightPaneState::MainMenu), // 시작은 메뉴 목록으로
    m_mainMenuSelection(0),
    m_statSelection(0),
    m_skillBookSlotSelection(0),
    m_skillSelectionListCursor(0),
    m_slotIndexToModify(0),
    m_skillListScrollOffset(0)
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
    case ERightPaneState::SkillBook:        ProcessSkillBookInput(key); break;
    case ERightPaneState::SkillSelection:   ProcessSkillSelectionInput(key); break;
    }
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
    case ERightPaneState::SkillBook:        DrawSkillBookScreen(); break;
    case ERightPaneState::SkillSelection:   DrawSkillSelectionScreen(); break;
    }

    m_renderer.Render();
}

// --- 입력 처리 함수들 ---


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
        WORD attributes = 0x000F; // 기본 흰색
        std::wstring cursor = L"  ";

        if (i == m_statSelection)
        {
            attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; // 밝은 노란색
            cursor = L"▶ ";
        }

        // 스탯 이름 (왼쪽 정렬)
        m_renderer.DrawString(contentX, currentStatY, cursor + statNames[i]);
        // 스탯 값 (오른쪽 정렬)
        std::wstring valueStr = std::to_wstring(static_cast<int>(statValues[i]));
        m_renderer.DrawString(boxX + boxInnerWidth - 1 - valueStr.length(), currentStatY, valueStr, attributes);
    }

    // 하단 안내 메시지 표시
    m_renderer.DrawString(m_renderer.GetWidth() / 2 + 3, m_renderer.GetHeight() - 6, L"[Enter: 포인트 분배 | ESC: 뒤로 가기]");

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
            // 2. 선택되었다면, 박스 왼쪽에 커서(▶)를 그립니다.
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
    m_renderer.DrawString(m_renderer.GetWidth() / 2 + 3, m_renderer.GetHeight() - 4, L"[Enter: 스킬 변경 | ESC: 뒤로 가기]");
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
            std::wstring cursor = (skillIndex == m_skillSelectionListCursor) ? L"▶ " : L"  ";
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

    // --- 선택된 스킬 정보창 출력 (기존과 동일하지만 위치 조정) ---
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

            // 커서가 스크롤 영역 위로 벗어나면 스크롤을 올립니다.
            if (m_skillSelectionListCursor < m_skillListScrollOffset)
            {
                m_skillListScrollOffset = m_skillSelectionListCursor;
            }
            // 맨 마지막 항목으로 순환했을 때, 스크롤도 맨 아래로 이동합니다.
            if (m_skillSelectionListCursor == abilityCount - 1)
            {
                m_skillListScrollOffset = (std::max)(0, abilityCount - m_skillListVisibleCount);
            }
        }
        else if (key == 80) // 아래
        {
            m_skillSelectionListCursor = (m_skillSelectionListCursor == abilityCount - 1) ? 0 : m_skillSelectionListCursor + 1;

            // 커서가 스크롤 영역 아래로 벗어나면 스크롤을 내립니다.
            if (m_skillSelectionListCursor >= m_skillListScrollOffset + m_skillListVisibleCount)
            {
                m_skillListScrollOffset = m_skillSelectionListCursor - m_skillListVisibleCount + 1;
            }
            // 맨 첫 항목으로 순환했을 때, 스크롤도 맨 위로 이동합니다.
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

    // 현재 장착된 스킬 목록을 가져옵니다.
    const auto& equippedAbilities = m_player.GetAbilityComponent()->GetEquippedAbilities();

    // 장착된 스킬 목록에 찾는 스킬이 있는지 확인합니다.
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
        case 2: m_currentPaneState = ERightPaneState::SkillBook; m_skillBookSlotSelection = 0; break;
        case 4: m_bIsRunning = false; m_result = EPauseMenuResult::GoToMainMenu; break;
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
    m_renderer.DrawString(infoX, infoY++, L"골드      : " + std::to_wstring(static_cast<int>(stats->Gold.CurrentValue)) + L" G");
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