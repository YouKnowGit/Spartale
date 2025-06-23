#include "BattleManager.h"
#include "Player.h"
#include "Monster.h"
#include "ConsoleRenderer.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbility.h"

#include <iostream>
#include <windows.h>
#include <conio.h>

BattleManager::BattleManager(Player* player, Monster* monster, ConsoleRenderer& renderer)
    : m_player(player),
    m_monster(monster),
    m_renderer(renderer),
    m_bIsBattleOver(false),
    m_bPlayerWon(false),
    m_CurrentTurn(1),
    m_battleState(EBattleState::Intro), // 전투는 '인트로' 상태에서 시작
    m_currentMenuSelection(0)
{
}

void BattleManager::Run()
{
    PlayIntroAnimation();
    while (!m_bIsBattleOver)
    {
        ProcessInput();
        Update();
        Render();
        Sleep(16); // 약 60 FPS
    }
    // 전투가 종료되면 EndBattle 호출
    EndBattle();
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

    // 상태 머신: 현재 상태에 따라 다음 행동을 결정합니다.
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
                LogAndWait(L"성공적으로 도망쳤다!");
                m_bIsBattleOver = true; // 전투 종료 플래그 설정
            }
        }
        else // 현재 메뉴가 '스킬 선택' 메뉴였다면
        {
            if (choice == m_currentMenuOptions.size() - 1) { // 뒤로가기
                m_battleState = EBattleState::PlayerActionSelect;
            }
            else { // 스킬 사용
                std::wstring resultLog = m_player->GetAbilityComponent()->TryActivateAbility(choice, m_monster);
                LogAndWait(resultLog);
                m_battleState = EBattleState::TurnEnd;
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
        //Render(); Sleep(1500);
        LogAndWait(m_monster->RunAI(m_player));
        //Render(); Sleep(1500);

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
    DrawBox(2, 20, 60, 8);
    DrawBox(63, 20, 25, 8);
    DrawString(4, 22, m_statusMessage);
}

void BattleManager::DrawActionSelectMenu()
{
    for (size_t i = 0; i < m_currentMenuOptions.size(); ++i) {
        std::wstring menuText = (i == m_currentMenuSelection) ? L"▶ " : L"  ";
        menuText += m_currentMenuOptions[i];
        DrawString(65, 22 + i, menuText);
    }
}

void BattleManager::DrawSkillSelectMenu()
{
    for (size_t i = 0; i < m_currentMenuOptions.size(); ++i) {
        std::wstring menuText = (i == m_currentMenuSelection) ? L"▶ " : L"  ";
        menuText += m_currentMenuOptions[i];
        DrawString(65, 22 + i, menuText);
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
        m_bPlayerWon = true;
    }
    else if (m_player->GetAbilityComponent()->GetAttributeSet()->HP.CurrentValue <= 0)
    {
        m_bIsBattleOver = true;
        m_bPlayerWon = false;
    }
}

void BattleManager::EndBattle()
{
    if (m_bPlayerWon)
    {
        Log(m_monster->Name + L"을(를) 쓰러뜨렸다! 승리!");
    }
    else // 도망간 경우가 아니면서 전투가 끝났을 때
    {
        Log(m_player->Name + L"은(는) 쓰러졌다...");
    }
    Render(); // 마지막 메시지 표시
    Sleep(3000);
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