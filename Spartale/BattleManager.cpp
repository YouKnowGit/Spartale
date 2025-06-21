#include "BattleManager.h"
#include "Player.h"
#include "Monster.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbility.h"
#include "ConsoleUtils.h"

#include <iostream>
#include <windows.h>
#include <conio.h>

using namespace std;
using namespace ConsoleUtils;

BattleManager::BattleManager(Player* player, Monster* monster)
    : m_player(player), m_monster(monster),
    m_bIsBattleOver(false), m_bPlayerWon(false), m_CurrentTurn(1)
{
    m_statusMessage = L"야생의 " + m_monster->Name + L"이(가) 나타났다!";
}

void BattleManager::Run() {
    ShowConsoleCursor(false);
    //PlayIntroAnimation();

    Log(m_statusMessage);
    Sleep(1500);

    while (!m_bIsBattleOver) {
        Draw();
        ProcessPlayerTurn();
        CheckBattleStatus();
        if (m_bIsBattleOver) break;

        Draw();
        ProcessEnemyTurn();
        CheckBattleStatus();
        if (m_bIsBattleOver) break;

        m_CurrentTurn++;
    }

    EndBattle();
}

void BattleManager::ProcessPlayerTurn()
{
    m_player->GetAbilityComponent()->GetAttributeSet()->bIsDefending = false;
    Log(L"무엇을 할까?");

    vector<wstring> options = { L"공격", L"방어", L"아이템", L"도망가기" };
    int choice = SelectMenuVertical(options, 65, 22);

    switch (choice)
    {
    case 0: // 공격
    {
        Log(L"사용할 스킬을 선택하세요.");
        AbilitySystemComponent* pASC = m_player->GetAbilityComponent();
        const auto& equippedAbilities = pASC->GetEquippedAbilities();
        vector<wstring> skillOptions;

        for (const auto& skill : equippedAbilities)
        {
            if (skill) skillOptions.push_back(skill->AbilityName + L" (MP:" + to_wstring((int)skill->ManaCost) + L")");
            else skillOptions.push_back(L"(비어있음)");
        }
        skillOptions.push_back(L"뒤로가기");

        int skillChoice = SelectMenuVertical(skillOptions, 65, 21);

        // '뒤로가기'를 선택하지 않은 경우
        if (skillChoice < skillOptions.size() - 1)
        {
            std::wstring resultLog = pASC->TryActivateAbility(skillChoice, m_monster);

            // 받은 로그를 BattleManager의 Log 함수를 통해 UI에 표시
            Log(resultLog);

            Sleep(2000); // 결과를 볼 시간
        }
        else // '뒤로가기' 선택 시
        {
            ProcessPlayerTurn(); // 행동 선택으로 다시 돌아감
        }
        break;
    }
    case 1: // 방어
        m_player->GetAbilityComponent()->GetAttributeSet()->bIsDefending = true;
        Log(m_player->Name + L"은(는) 방어 태세를 갖췄다!");
        Sleep(1500);
        break;
    case 2: // 아이템
        Log(L"아이템 가방이 비어있습니다!");
        Sleep(1500);
        break;
    case 3: // 도망가기
        Log(L"성공적으로 도망쳤다!");
        m_bIsBattleOver = true;
        Sleep(1500);
        break;
    }
}

void BattleManager::ProcessEnemyTurn()
{
    Log(m_monster->Name + L"의 턴!");
    Sleep(1500);
    Log(m_monster->RunAI(m_player));
    Sleep(2000);
}

void BattleManager::Log(const wstring& message)
{
    m_statusMessage = message;
    Draw();
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
    else if (m_bIsBattleOver) // 도망간 경우가 아니면서 전투가 끝났을 때
    {
        Log(m_player->Name + L"은(는) 쓰러졌다...");
    }
    Sleep(3000);
}

void BattleManager::Draw() {
    clearScreen();
    gotoxy(0, 0);

    AttributeSet* playerAttr = m_player->GetAbilityComponent()->GetAttributeSet();
    AttributeSet* monsterAttr = m_monster->GetAbilityComponent()->GetAttributeSet();

    DrawBox(45, 2, 30, 6);
    gotoxy(47, 3); wcout << m_monster->Name << L"     Lv. " << monsterAttr->Level;
    gotoxy(47, 4); wcout << DrawStatBar(L"HP", monsterAttr->HP.CurrentValue, monsterAttr->HP.BaseValue, 15);
    gotoxy(49, 5); wcout << (int)monsterAttr->HP.CurrentValue << L" / " << (int)monsterAttr->HP.BaseValue;

    DrawBox(2, 12, 30, 7);
    gotoxy(4, 13); wcout << m_player->Name << L"      Lv. " << playerAttr->Level;
    gotoxy(4, 14); wcout << DrawStatBar(L"HP", playerAttr->HP.CurrentValue, playerAttr->HP.BaseValue, 15);
    gotoxy(6, 15); wcout << (int)playerAttr->HP.CurrentValue << L" / " << (int)playerAttr->HP.BaseValue;
    gotoxy(4, 16); wcout << L"MP : " << (int)playerAttr->MP.CurrentValue << L" / " << (int)playerAttr->MP.BaseValue << wstring(15, L' ');

    DrawBox(2, 20, 60, 8);
    DrawBox(63, 20, 25, 8);
    gotoxy(4, 22); wcout << wstring(58, L' ');
    gotoxy(4, 22); wcout << m_statusMessage;

    for (int i = 0; i < 6; ++i) {
        gotoxy(64, 21 + i);
        wcout << wstring(23, L' ');
    }
}
void BattleManager::PlayIntroAnimation()
{
    ShowConsoleCursor(false);
    clearScreen();

    // 1. 화면 깜빡임 효과 (3번)
    for (int i = 0; i < 3; ++i)
    {
        system("color F0");
        Sleep(60);
        system("color 0F");
        Sleep(60);
    }

    // 2. 위아래로 갈라지는 효과
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 핸들을 직접 가져옴
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int screenWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int screenHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // gotoxy를 사용하기보다 직접 핸들을 제어하는 것이 더 안정적
    auto SetCursorPos = [&](int x, int y) {
        SetConsoleCursorPosition(hConsole, { (SHORT)x, (SHORT)y });
        };

    // 먼저 화면 전체를 블록으로 채움
    for (int y = 0; y < screenHeight; ++y)
    {
        SetCursorPos(0, y);
        for (int x = 0; x < screenWidth; ++x)
        {
            wcout << L"█";
        }
    }

    // 중앙에서부터 위아래로 한 줄씩 공백으로 만들며 갈라지는 효과
    int centerY = screenHeight / 2;
    wstring blankLine(screenWidth, L' '); // 공백으로 채워진 한 줄

    for (int i = 0; i <= centerY; ++i)
    {
        if (centerY - i >= 0) {
            SetCursorPos(0, centerY - i);
            wcout << blankLine;
        }
        if (centerY + i < screenHeight) {
            SetCursorPos(0, centerY + i);
            wcout << blankLine;
        }
        Sleep(25);
    }
    clearScreen();
}

std::wstring BattleManager::DrawStatBar(const std::wstring& label, float current, float max, int barLength) {
    if (current < 0) current = 0;
    float ratio = (max > 0) ? (current / max) : 0;
    int filledLength = static_cast<int>(ratio * barLength);
    wstring bar = label + L" [";
    for (int i = 0; i < barLength; ++i) {
        if (i < filledLength) bar += L"■";
        else bar += L" ";
    }
    bar += L"]";
    return bar;
}