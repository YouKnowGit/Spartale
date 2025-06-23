#include "GameLogic/BattleManager.h"
#include "GameLogic/Units/Player.h"
#include "GameLogic/Units/Monster.h"
#include "Framework/AbilitySystem/AttributeSet.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/GameplayAbility.h"
#include "Utils/ConsoleUtils.h"
#include "Utils/ConsoleRenderer.h"

#include <algorithm>
#include <iostream>
#include <windows.h>
#include <conio.h>

BattleManager::BattleManager(Player* player, Monster* monster, ConsoleRenderer& renderer)
    : m_player(player),
    m_monster(monster),
    m_renderer(renderer),
    m_bIsBattleOver(false),
    m_battleResult(EBattleResult::InProgress),
    m_CurrentTurn(1),
    m_battleState(EBattleState::Intro),
    m_currentMenuSelection(0)
{
    std::random_device rd;
    m_rng.seed(rd());
    PlayIntroAnimation();
}

EBattleResult BattleManager::Run()
{
    while (!m_bIsBattleOver)
    {
        ProcessInput();
        Update();
        Render();
        Sleep(16); // 약 60 FPS
    }
    // 전투가 종료되면 EndBattle 호출
    EndBattle();
    return m_battleResult;
}

void BattleManager::ProcessInput()
{
    if (!_kbhit()) return;

    int key = _getch();

    // 메뉴 선택 상태일 때만 위/아래/엔터/ESC 키가 작동
    if (m_battleState == EBattleState::PlayerActionSelect || m_battleState == EBattleState::PlayerSkillSelect)
    {
        if (key == 224) // 방향키
        {
            key = _getch();
            if (key == 72) // 위
                m_currentMenuSelection = (m_currentMenuSelection == 0) ? m_currentMenuOptions.size() - 1 : m_currentMenuSelection - 1;
            if (key == 80) // 아래
                m_currentMenuSelection = (m_currentMenuSelection + 1) % m_currentMenuOptions.size();
        }
        else if (key == 13) // 엔터: 선택 확정
        {
            m_battleState = EBattleState::ExecutingPlayerAction;
        }
        else if (key == 27) // ESC: 뒤로 가기
        {
            if (m_battleState == EBattleState::PlayerSkillSelect)
                m_battleState = EBattleState::PlayerActionSelect;
        }
    }
}

void BattleManager::Update()
{
    CheckBattleStatus();
    if (m_bIsBattleOver) return;

    // 상태 머신: 현재 상태에 따라 다음 행동을 결정
    switch (m_battleState)
    {
    case EBattleState::Intro:
        LogAndWait(L"야생의 " + m_monster->Name + L"이(가) 나타났다!");
        m_battleState = EBattleState::PlayerActionSelect; // 바로 플레이어 선택으로 전환
        break;

    case EBattleState::PlayerActionSelect:
        m_statusMessage = L"무엇을 할까?";
        m_currentMenuOptions = { L"공격", L"방어", L"아이템", L"도망가기" };
        break;

    case EBattleState::PlayerSkillSelect:
    {
        m_statusMessage = L"사용할 스킬을 선택하세요.";
        m_currentMenuOptions.clear();
        AbilitySystemComponent* pASC = m_player->GetAbilityComponent();
        const auto& equippedAbilities = pASC->GetEquippedAbilities();
        for (const auto& skill : equippedAbilities)
        {
            if (skill) m_currentMenuOptions.push_back(skill->AbilityName + L" (MP:" + std::to_wstring((int)skill->ManaCost) + L")");
            else m_currentMenuOptions.push_back(L"(비어있음)");
        }
        m_currentMenuOptions.push_back(L"뒤로가기");
    }
    break;

    case EBattleState::ExecutingPlayerAction:
    {
        int choice = m_currentMenuSelection;
        m_currentMenuSelection = 0; // 메뉴 선택 인덱스 초기화

        if (m_currentMenuOptions[0] == L"공격") // 현재 메뉴가 '행동 선택' 메뉴였다면
        {
            if (choice == 0) { // 공격
                m_battleState = EBattleState::PlayerSkillSelect;
            }
            else if (choice == 1) { // 방어
                m_player->GetAbilityComponent()->GetAttributeSet()->bIsDefending = true;
                LogAndWait(m_player->Name + L"은(는) 방어 태세를 갖췄다!");
                m_battleState = EBattleState::TurnEnd;
            }
            else if (choice == 2) { // 아이템
                LogAndWait(L"아이템 가방이 비어있습니다!");
                m_battleState = EBattleState::TurnEnd;
            }
            else if (choice == 3) { // 도망가기
                if (AttemptToFlee())
                {
                    // 성공
                    LogAndWait(L"성공적으로 도망쳤다!");
                    m_battleResult = EBattleResult::PlayerFled;
                    m_bIsBattleOver = true;
                }
                else
                {
                    // 실패
                    LogAndWait(L"도망에 실패했다!");
                    m_battleState = EBattleState::TurnEnd;
                }
            }
        }
        else { // 스킬 사용
            // 반환된 결과 구조체를 받음
            FActivationResult result = m_player->GetAbilityComponent()->TryActivateAbility(choice, m_monster);
            LogAndWait(result.LogMessage);

            // 결과의 bSuccess 값에 따라 다음 상태를 결정
            if (result.bSuccess)
            {
                // 성공했다면 턴을 종료
                m_battleState = EBattleState::TurnEnd;
            }
            else
            {
                // 실패했다면, 다시 스킬 선택 화면으로 돌아감 (턴 종료 X)
                m_battleState = EBattleState::PlayerSkillSelect;
            }
        }
    }
    break;

    case EBattleState::TurnEnd:
        // 플레이어 행동 후 효과 처리
    {
        std::wstring playerLog = m_player->GetAbilityComponent()->UpdateActiveEffects();
        if (!playerLog.empty()) { LogAndWait(playerLog); }
        CheckBattleStatus();
        if (m_bIsBattleOver) break;
        m_battleState = EBattleState::EnemyTurn;
    }
    break;

    case EBattleState::EnemyTurn:
    {
        LogAndWait(m_monster->Name + L"의 턴!");
        LogAndWait(m_monster->RunAI(m_player));

        // 몬스터 턴 후 효과 처리
        std::wstring monsterLog = m_monster->GetAbilityComponent()->UpdateActiveEffects();
        if (!monsterLog.empty()) { LogAndWait(monsterLog); }
        CheckBattleStatus();
        if (m_bIsBattleOver) break;

        m_CurrentTurn++;
        m_battleState = EBattleState::PlayerActionSelect; // 다시 플레이어 턴으로
    }
    break;

    case EBattleState::BattleOver:
        // 전투 종료 플래그만 설정하고 루프가 자연스럽게 끝나도록 둠
        break;
    }
}

void BattleManager::Render()
{
    m_renderer.Clear();
    DrawUI(); // UI 배경 및 정보 그리기

    // 상태에 따라 메뉴 그리기
    if (m_battleState == EBattleState::PlayerActionSelect)
        DrawActionSelectMenu();
    else if (m_battleState == EBattleState::PlayerSkillSelect)
        DrawSkillSelectMenu();

    m_renderer.Render();
}

void BattleManager::DrawUI()
{
    AttributeSet* playerAttr = m_player->GetAbilityComponent()->GetAttributeSet();
    AttributeSet* monsterAttr = m_monster->GetAbilityComponent()->GetAttributeSet();


    auto DrawBox = [&](int x, int y, int width, int height) {
        m_renderer.Draw(x, y, L'┌'); m_renderer.Draw(x + width - 1, y, L'┐');
        m_renderer.Draw(x, y + height - 1, L'└'); m_renderer.Draw(x + width - 1, y + height - 1, L'┘');
        for (int i = 1; i < width - 1; ++i) { m_renderer.Draw(x + i, y, L'─'); m_renderer.Draw(x + i, y + height - 1, L'─'); }
        for (int i = 1; i < height - 1; ++i) { m_renderer.Draw(x, y + i, L'│'); m_renderer.Draw(x + width - 1, y + i, L'│'); }
        };

    // 몬스터 정보
    DrawBox(45, 2, 30, 6);
    DrawString(47, 3, m_monster->Name + L"      Lv. " + std::to_wstring(monsterAttr->Level));
    DrawString(47, 4, DrawStatBar(L"HP", monsterAttr->HP.CurrentValue, monsterAttr->HP.BaseValue, 15));
    DrawString(49, 5, std::to_wstring((int)monsterAttr->HP.CurrentValue) + L" / " + std::to_wstring((int)monsterAttr->HP.BaseValue));

    // 플레이어 정보
    DrawBox(2, 12, 30, 7);
    DrawString(4, 13, m_player->Name + L"      Lv. " + std::to_wstring(playerAttr->Level));
    DrawString(4, 14, DrawStatBar(L"HP", playerAttr->HP.CurrentValue, playerAttr->HP.BaseValue, 15));
    DrawString(6, 15, std::to_wstring((int)playerAttr->HP.CurrentValue) + L" / " + std::to_wstring((int)playerAttr->HP.BaseValue));
    DrawString(4, 16, L"MP : " + std::to_wstring((int)playerAttr->MP.CurrentValue) + L" / " + std::to_wstring((int)playerAttr->MP.BaseValue));

    // 메시지 박스
    DrawBox(2, 20, 79, 8);
    DrawBox(82, 20, 25, 8);
    DrawString(4, 22, m_statusMessage);
}

void BattleManager::DrawActionSelectMenu()
{
    for (size_t i = 0; i < m_currentMenuOptions.size(); ++i) {
        std::wstring menuText = (i == m_currentMenuSelection) ? L"▶ " : L"  ";
        menuText += m_currentMenuOptions[i];
        DrawString(84, 22 + i, menuText);
    }
}

void BattleManager::DrawSkillSelectMenu()
{
    for (size_t i = 0; i < m_currentMenuOptions.size(); ++i) {
        std::wstring menuText = (i == m_currentMenuSelection) ? L"▶ " : L"  ";
        menuText += m_currentMenuOptions[i];
        DrawString(84, 22 + i, menuText);
    }
}

void BattleManager::Log(const std::wstring& message)
{
    m_statusMessage = message;
}
void BattleManager::LogAndWait(const std::wstring& message)
{
    Log(message);// + L" (계속하려면 아무 키나 누르세요...)");

    Render();
    while (_kbhit()) _getch();
	_getch(); // 아무 키나 누를 때까지 대기
}

void BattleManager::CheckBattleStatus()
{
    if (m_monster->GetAbilityComponent()->GetAttributeSet()->HP.CurrentValue <= 0)
    {
        m_bIsBattleOver = true;
        m_battleResult = EBattleResult::PlayerWon; // 승리 상태로 설정
    }
    else if (m_player->GetAbilityComponent()->GetAttributeSet()->HP.CurrentValue <= 0)
    {
        m_bIsBattleOver = true;
        m_battleResult = EBattleResult::PlayerLost; // 패배 상태로 설정
    }
}

void BattleManager::EndBattle()
{
    switch (m_battleResult)
    {
        case EBattleResult::PlayerWon:
        {
            AttributeSet* playerStats = m_player->GetAbilityComponent()->GetAttributeSet();
            AttributeSet* monsterStats = m_monster->GetAbilityComponent()->GetAttributeSet();
            if (!playerStats || !monsterStats) break;

            // --- 보상 계산 ---
            const float BaseExpPerLevel = 25.0f;
            const float BaseGoldPerLevel = 77.0f;
            std::uniform_real_distribution<float> rewardMultiplier(0.8f, 1.2f);

            int expGained = static_cast<int>(BaseExpPerLevel * monsterStats->Level * rewardMultiplier(m_rng));
            int goldGained = static_cast<int>(BaseGoldPerLevel * monsterStats->Level * rewardMultiplier(m_rng));

            // --- 보상 지급 ---
            playerStats->Experience.CurrentValue += expGained;
            playerStats->Gold.CurrentValue += goldGained;

            // --- 메시지 출력 (순차적으로) ---
            // 몬스터 처치 및 보상 획득 메시지를 먼저 출력
            std::wstring rewardMessage = m_monster->Name + L"을(를) 쓰러뜨렸다! 경험치 "
                + std::to_wstring(expGained) + L", "
                + std::to_wstring(goldGained) + L" G를 획득했다!";
            LogAndWait(rewardMessage);

            // 레벨업 체크
            bool bLeveledUp = m_player->GetAbilityComponent()->CheckAndProcessLevelUp();

            // 레벨업을 했다면, 별도의 메시지를 다시 출력
            if (bLeveledUp)
            {
                std::wstring levelUpMessage = m_player->Name + L"은(는) 레벨 업 했다!";
                LogAndWait(levelUpMessage);
            }
            break;
        }
        case EBattleResult::PlayerLost:
            LogAndWait(m_player->Name + L"은(는) 쓰러졌다...");
            break;
        case EBattleResult::PlayerFled:
            break;
        default:
            break;
    }
}

std::wstring BattleManager::DrawStatBar(const std::wstring& label, float current, float max, int barLength) const
{
    if (current < 0) current = 0;
    float ratio = (max > 0) ? (current / max) : 0;
    int filledLength = static_cast<int>(ratio * barLength);
    std::wstring bar = label + L" [";
    for (int i = 0; i < barLength; ++i) {
        if (i < filledLength) bar += L"■";
        else bar += L" ";
    }
    bar += L"]";
    return bar;
}
void BattleManager::DrawString(int x, int y, const std::wstring& str) const
{
    int currentX = x;
    for (const auto& ch : str)
    {
        m_renderer.Draw(currentX, y, ch);

        // 문자가 한글 범위에 속하는지 확인
        if (ch >= L'가' && ch <= L'힣')
        {
            currentX += 2; // 한글이면 2칸 이동
        }
        else
        {
            currentX += 1; // 그 외에는 1칸 이동
        }
    }
}
void BattleManager::PlayIntroAnimation()
{
    // 화면 깜빡임 효과 (색상 변경)
    for (int i = 0; i < 3; ++i) {
        // 모든 버퍼 셀의 색상 속성을 변경
        for (int j = 0; j < m_renderer.GetWidth() * m_renderer.GetHeight(); ++j) {
            m_renderer.Draw(j % m_renderer.GetWidth(), j / m_renderer.GetWidth(), L' ', 0xFF); // 빨간 배경, 흰 
        }
        m_renderer.Render();
        Sleep(80);

        for (int j = 0; j < m_renderer.GetWidth() * m_renderer.GetHeight(); ++j) {
            m_renderer.Draw(j % m_renderer.GetWidth(), j / m_renderer.GetWidth(), L' ', 0x0F); // 검은 배경, 흰 글씨
        }
        m_renderer.Render();
        Sleep(80);
    }

    // 화면을 블록으로 채우기
    for (int y = 0; y < m_renderer.GetHeight(); ++y) {
        for (int x = 0; x < m_renderer.GetWidth(); ++x) {
            m_renderer.Draw(x, y, L'█');
        }
    }
    m_renderer.Render();
    Sleep(100);

    // 가운데에서 좌우로 갈라지는 애니메이션
    int centerX = m_renderer.GetWidth() / 2;
    for (int offset = 0; offset <= centerX; ++offset)
    {
        for (int y = 0; y < m_renderer.GetHeight(); ++y)
        {
            // 왼쪽 칸 지우기
            if (centerX - offset >= 0) {
                m_renderer.Draw(centerX - offset, y, L' ');
            }
            // 오른쪽 칸 지우기
            if (centerX + offset < m_renderer.GetWidth()) {
                m_renderer.Draw(centerX + offset, y, L' ');
            }
        }
        // 매 프레임마다 화면을 다시 그림
        m_renderer.Render();
        Sleep(5);
    }
}
bool BattleManager::AttemptToFlee()
{
    // - - - 도망 확률 조절 - - -
    const float baseFleeChance = 0.60f; // 기본 성공 확률 (70%)
    const float chanceModifierPerLevel = 0.10f; // 레벨 1당 증감 확률 (10%)
    const float minFleeChance = 0.10f; // 최소 성공 확률 (10%)
    const float maxFleeChance = 0.95f; // 최대 성공 확률 (95%)
    // ---------------------------------------------

    // 플레이어와 몬스터의 레벨을 불러옴
    AttributeSet* playerStats = m_player->GetAbilityComponent()->GetAttributeSet();
    AttributeSet* monsterStats = m_monster->GetAbilityComponent()->GetAttributeSet();
    if (!playerStats || !monsterStats) return false; // 예외 처리

    // 공식을 적용하여 최종 확률을 계산
    float levelDifference = static_cast<float>(playerStats->Level - monsterStats->Level);
    float finalFleeChance = baseFleeChance + (levelDifference * chanceModifierPerLevel);

    // 최소/최대 확률 범위에 맞게 값을 보정
    finalFleeChance = (std::max)(minFleeChance, (std::min)(maxFleeChance, finalFleeChance));

    // 0.0 ~ 1.0 사이의 난수를 생성하여 성공 여부를 결정
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    // 디버깅용: 최종 확률을 로그에 표시
    Log(L"도망 성공 확률: " + std::to_wstring(static_cast<int>(finalFleeChance * 100)) + L"%");
    Render();
    Sleep(1000);

    return dist(m_rng) < finalFleeChance;
}