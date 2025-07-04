#include "GameLogic/BattleManager.h"
#include "GameLogic/Units/Player.h"
#include "GameLogic/Units/Monster.h"
#include "GameLogic/DataManager.h"
#include "GameLogic/Items/ItemData.h"
#include "Framework/Inventory/InventoryComponent.h"
#include "Framework/AbilitySystem/AttributeSet.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/GameplayAbility.h"
#include "Utils/ConsoleUtils.h"
#include "Utils/ConsoleRenderer.h"
#include "Utils/StringUtils.h"

#include <algorithm>
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

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
    m_navigateSoundPath = L"Sounds/UI/pausemenu_select.wav"; // 메뉴 이동 효과음
    m_confirmSoundPath = L"Sounds/UI/pausemenu_confirm.wav";   // 메뉴 선택 효과음
    m_escSoundPath = L"Sounds/UI/mainmenu_select.wav"; // 메뉴 이동 효과음
    PlayIntroAnimation();
}

EBattleResult BattleManager::Run()
{
    
    while (!m_bIsBattleOver)
    {
        m_monster->GetAbilityComponent()->GetAttributeSet()->MP = FAttributeData(5000);
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

    InventoryComponent* inventory = m_player->GetInventory();
    if (!inventory) return;

    // 메뉴 선택 상태일 때만 위/아래/엔터/ESC 키가 작동
    if (m_battleState == EBattleState::PlayerActionSelect || m_battleState == EBattleState::PlayerSkillSelect || m_battleState == EBattleState::ItemInven)
    {
        if (key == 224) // 방향키
        {
            if (m_currentMenuOptions.empty())
            {
                PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
                return;
            }
            PlaySound(m_navigateSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            key = _getch();
            if (key == 72) // 위
                m_currentMenuSelection = (m_currentMenuSelection == 0) ? m_currentMenuOptions.size() - 1 : m_currentMenuSelection - 1;
            if (key == 80) // 아래
                m_currentMenuSelection = (m_currentMenuSelection + 1) % m_currentMenuOptions.size();
        }
        else if (key == 13) // 엔터: 선택 확정
        {
            PlaySound(m_confirmSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            m_battleState = EBattleState::ExecutingPlayerAction;
        }
        else if (key == 27) // ESC: 뒤로 가기
        {
            PlaySound(m_escSoundPath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
            if (m_battleState == EBattleState::PlayerSkillSelect)
                m_battleState = EBattleState::PlayerActionSelect;

            if (m_battleState == EBattleState::ItemInven)
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
        PlaySound(NULL, NULL, SND_PURGE);  // 이전사운드 중지
        PlaySound(meet, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOSTOP); // 사운드 재생
        if (m_monster->GetID() == "Ancient_Dragon")
        {
            LogAndWait(m_monster->Name + L"을 마주했다.");
            LogAndWait(L"현재까지의 진행 상황이 저장되었습니다.");
        }
        else 
        {
            LogAndWait(L"야생의 " + m_monster->Name + L"이(가) 나타났다!");
        }
        m_battleState = EBattleState::PlayerActionSelect; // 바로 플레이어 선택으로 전환
        break;

    case EBattleState::PlayerActionSelect:
        m_statusMessage = L"무엇을 할까?";
        m_currentMenuOptions = { L"공격", L"방어", L"아이템", L"도망가기" };

        // 턴 처음 부분에서 방어 여부 초기화
        m_player->GetAbilityComponent()->GetAttributeSet()->bIsDefending = false;
        m_monster->GetAbilityComponent()->GetAttributeSet()->bIsDefending = false;
        break;

    case EBattleState::PlayerSkillSelect:
    {
        m_bIsItemMenu = false;
        m_statusMessage = L"사용할 스킬을 선택하세요.";
        m_currentMenuOptions.clear();
        AbilitySystemComponent* pASC = m_player->GetAbilityComponent();
        const auto& equippedAbilities = pASC->GetEquippedAbilities();
        for (const auto& skill : equippedAbilities)
        {
            if (skill) m_currentMenuOptions.push_back(skill->AbilityName + L" (MP:" + std::to_wstring((int)skill->ManaCost) + L")");
            else m_currentMenuOptions.push_back(L"(비어있음)");
        }
        break;
    }
    case EBattleState::ItemInven:
    {
        m_bIsItemMenu = true;
        m_statusMessage = L"사용할 아이템을 선택하세요.";
        m_currentMenuOptions.clear();
        potions.clear();
        InventoryComponent* inventory = m_player->GetInventory();
        if (!inventory) return;

       
        for (int i = 0; i < inventory->GetUsedSlotCount(); i++)
        {
            const InventorySlot* slot = inventory->GetSlotAtIndex(i);
            if (!slot)   continue;
            if (slot->ItemID == "consume_potion_01" || slot->ItemID == "consume_potion_02" ||
                slot->ItemID == "consume_potion_03" || slot->ItemID == "consume_potion_04")
            {
                potions.push_back(i);

                const ItemData* data = DataManager::GetInstance().GetItemData(slot->ItemID);
                if(data)    m_currentMenuOptions.push_back((data->Name)+ L"(" + std::to_wstring(slot->Quantity) +L"개)");
                else m_currentMenuOptions.push_back(L"-");
            }
        }
    }
    break;

    case EBattleState::ExecutingPlayerAction:
    {
        int choice = m_currentMenuSelection;
        m_currentMenuSelection = 0; // 메뉴 선택 인덱스 초기화

        if (!m_currentMenuOptions.empty() && m_currentMenuOptions[0] == L"공격") // 현재 메뉴가 '행동 선택' 메뉴였다면
        {
            if (choice == 0) { // 공격
                m_battleState = EBattleState::PlayerSkillSelect;
            }
            else if (choice == 1) { // 방어
                m_player->GetAbilityComponent()->GetAttributeSet()->bIsDefending = true;
                LogAndWait(m_player->Name + L"은(는) 방어 태세를 갖췄다!");
                m_bIsItemMenu = false;
                m_battleState = EBattleState::TurnEnd;
            }
            else if (choice == 2) { // 아이템
                bool bHasItem = false;

                InventoryComponent* inventory = m_player->GetInventory();
                for (int i = 0; i < inventory->GetUsedSlotCount(); i++)
                {
                    const InventorySlot* slot = inventory->GetSlotAtIndex(i);
                    if (!slot)   continue;
                    if (slot->ItemID == "consume_potion_01" || slot->ItemID == "consume_potion_02" ||
                        slot->ItemID == "consume_potion_03" || slot->ItemID == "consume_potion_04")
                    {
                        potions.push_back(i);

                        const ItemData* data = DataManager::GetInstance().GetItemData(slot->ItemID);
                        if (data)
                        {
                            bHasItem = true;
                            m_currentMenuOptions.push_back((data->Name) + L"(" + std::to_wstring(slot->Quantity) + L"개)");
                        }
                        else m_currentMenuOptions.push_back(L"-");
                    }
                }
                if (bHasItem)    m_battleState = EBattleState::ItemInven;
                else
                {
                    LogAndWait(L"사용할 수 있는 아이템이 없습니다.");
                    m_battleState = EBattleState::PlayerActionSelect;
                }
            }
            else if (choice == 3) { // 도망가기

                m_bIsItemMenu = false;
                if (m_monster->GetID() == "Ancient_Dragon")
                {
                    LogAndWait(L"이 녀석을 상대로는 도망갈 수 없다..");
                    m_battleState = EBattleState::TurnEnd;
                }
                else if (AttemptToFlee())
                {
                    // 성공
                    PlaySound(NULL, NULL, SND_PURGE);
                    PlaySound(Runway, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOSTOP); // 도망 성공시 출력 사운드
                    LogAndWait(L"성공적으로 도망쳤다!");
                    Sleep(800);
                    m_battleResult = EBattleResult::PlayerFled;
                    m_bIsBattleOver = true;
                }
                else
                {
                    // 실패
                    PlaySound(NULL, NULL, SND_PURGE);
                    PlaySound(RunwayFailure, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOSTOP); // 도망 성공시 출력 사운드
                    LogAndWait(L"도망에 실패했다!");
                    Sleep(800);
                    m_battleState = EBattleState::TurnEnd;
                }
            }
        }
        else if (m_bIsItemMenu)
        {
            //아이템 사용
            if (potions.empty() || m_currentMenuOptions.empty())
            {
                LogAndWait(L"사용할 수 있는 아이템이 없다.");
                m_battleState = EBattleState::PlayerActionSelect;
                m_bIsItemMenu = false;
                break;
            }

            int choice = m_currentMenuSelection;
            m_currentMenuSelection = 0; 

            if (choice < 0 || choice >= potions.size())
            {
                m_battleState = EBattleState::PlayerActionSelect;
                m_bIsItemMenu = false;
                break;
            }
            int realItemIndex = potions.at(choice);

            const InventorySlot* slot = m_player->GetInventory()->GetSlotAtIndex(realItemIndex);
            if (slot && slot->Quantity > 0)
            {
                LogAndWait(m_player->Name + L"은(는) " + slot->pItemData->Name + L"을(를) 사용했다 !");

                m_player->GetInventory()->UseItem(realItemIndex, m_player);
                m_player->GetInventory()->RemoveItem(realItemIndex, 1);
                PlaySound(Potion, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);

                Render();

                if (slot->ItemID == "consume_potion_01" || slot->ItemID == "consume_potion_02")
                    LogAndWait(m_player->Name + L"은(는) 체력을 회복했다!");
                else
                    LogAndWait(m_player->Name + L"은(는) 마나를 회복했다!");

                m_battleState = EBattleState::TurnEnd;
                m_bIsItemMenu = false;
            }
            else
            {
                m_battleState = EBattleState::PlayerActionSelect;
                m_bIsItemMenu = false;
            }
        }
        else
        { // 스킬 사용
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
        if (!m_bIsOne && m_monster->GetID() == "Ancient_Dragon" && m_monster->GetAbilityComponent()->GetAttributeSet()->HP.CurrentValue < 300)
        {
            m_bIsOne = true;
            LogAndWait(m_monster->Name + L"이 몸을 일으켜세운다.");
            PlaySound(NULL, NULL, SND_PURGE);
            PlaySound(BossAwake, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOSTOP);// 보스 부활 사운드
            LogAndWait(m_monster->Name + L"이 깨어났다.");
            m_monster->Name = L"깨어난 데스페라도";
            AttributeSet* monStats = m_monster->GetAbilityComponent()->GetAttributeSet();
            monStats->Level = 20;
            monStats->HP = FAttributeData(1500);
            monStats->Strength = FAttributeData(70);
            monStats->Agility = FAttributeData(25);
            monStats->Intelligence = FAttributeData(80);
            monStats->Defence = FAttributeData(60);
            monStats->MagicResistance = FAttributeData(60);
            monStats->AdjustDependentAttributes();

            Render();
        }

        m_monster->GetAbilityComponent()->GetAttributeSet()->MP = FAttributeData(5000);
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
    else if (m_battleState == EBattleState::PlayerSkillSelect || m_battleState == EBattleState::ItemInven) // 아이템창, 스킬창은 같은 함수 공유
        DrawSkillSelectMenu();


    m_renderer.Render();
}

void BattleManager::DrawUI()
{
    AttributeSet* playerAttr = m_player->GetAbilityComponent()->GetAttributeSet();
    AttributeSet* monsterAttr = m_monster->GetAbilityComponent()->GetAttributeSet();


    auto DrawBox = [&](int x, int y, int width, int height) { // 전투UI 박스
        int adjustedWidth = width + 2; 

        m_renderer.Draw(x, y, L'┌'); m_renderer.Draw(x + adjustedWidth - 1, y, L'┐'); 
        m_renderer.Draw(x, y + height - 1, L'└'); m_renderer.Draw(x + adjustedWidth - 1, y + height - 1, L'┘'); 
        // 가로선 
        for (int i = 1; i < adjustedWidth - 1; ++i) { 
            m_renderer.Draw(x + i, y, L'─'); 
            m_renderer.Draw(x + i, y + height - 1, L'─'); 
        }
        // 세로선 
        for (int i = 1; i < height - 1; ++i) { 
            m_renderer.Draw(x, y + i, L'│'); 
            m_renderer.Draw(x + adjustedWidth - 1, y + i, L'│'); 
        }
    };

    // 몬스터 정보
    DrawBox(45, 2, 30, 6);
    m_renderer.DrawString(47, 3, m_monster->Name);
    m_renderer.DrawString(69, 3, L"Lv. " + std::to_wstring(monsterAttr->Level));
    std::wstring barStrMonsterHP = DrawStatBar(L"HP", monsterAttr->HP.CurrentValue, monsterAttr->HP.BaseValue, 20, barColorHP); // 개선 코드 [ 색상 변경 ] [ MonsterHP ]
    m_renderer.DrawString(47, 4, barStrMonsterHP, barColorHP);
    m_renderer.DrawString(55, 5, std::to_wstring((int)monsterAttr->HP.CurrentValue) + L" / " + std::to_wstring((int)monsterAttr->HP.BaseValue));

    // 플레이어 정보
    DrawBox(2, 12, 30, 7);
    m_renderer.DrawString(4, 13, m_player->Name);
    m_renderer.DrawString(26, 13, L"Lv. " + std::to_wstring(playerAttr->Level));
    std::wstring barStrPlayerHP = DrawStatBar(L"HP", playerAttr->HP.CurrentValue, playerAttr->HP.BaseValue, 20, barColorHP); // 개선 코드 [ 색상 변경 ] [ PlayerHP ]
    m_renderer.DrawString(4, 14, barStrPlayerHP, barColorHP);
    m_renderer.DrawString(12, 15, std::to_wstring((int)playerAttr->HP.CurrentValue) + L" / " + std::to_wstring((int)playerAttr->HP.BaseValue));
    std::wstring barStrPlayerMP = DrawStatBar(L"MP", playerAttr->MP.CurrentValue, playerAttr->MP.BaseValue, 20, barColorMP); // 개선 코드 [ 색상 변경 ] [ PlayerMP ]
    m_renderer.DrawString(4, 16, barStrPlayerMP, barColorMP); 
    m_renderer.DrawString(12, 17, std::to_wstring((int)playerAttr->MP.CurrentValue) + L" / " + std::to_wstring((int)playerAttr->MP.BaseValue));

    // 메시지 박스
    DrawBox(2, 20, 79, 8);
    DrawBox(82, 20, 25, 8);
    m_renderer.DrawString(4, 22, m_statusMessage);
}

void BattleManager::DrawActionSelectMenu()
{
    for (size_t i = 0; i < m_currentMenuOptions.size(); ++i) {
        std::wstring menuText = (i == m_currentMenuSelection) ? L"▶  " : L"   ";
        menuText += m_currentMenuOptions[i];
        m_renderer.DrawString(84, 22 + i, menuText);
    }
}

void BattleManager::DrawSkillSelectMenu()
{
    for (size_t i = 0; i < m_currentMenuOptions.size(); ++i) {
        std::wstring menuText = (i == m_currentMenuSelection) ? L"▶  " : L"   ";
        menuText += m_currentMenuOptions[i];
        m_renderer.DrawString(84, 22 + i, menuText);
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
        PlaySound(NULL, NULL, SND_PURGE);
        PlaySound(win, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOSTOP); // 승리시 출력 사운드
    }
    else if (m_player->GetAbilityComponent()->GetAttributeSet()->HP.CurrentValue <= 0)
    {
        m_bIsBattleOver = true;
        m_battleResult = EBattleResult::PlayerLost; // 패배 상태로 설정
        PlaySound(NULL, NULL, SND_PURGE);  
        PlaySound(Lose, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOSTOP); // 패배시 출력 사운드
    }
}

void BattleManager::EndBattle()
{
    //전투 종료 시 지속효과 클리어
    m_player->GetAbilityComponent()->ClearAllActiveEffects();
    m_monster->GetAbilityComponent()->ClearAllActiveEffects();

    switch (m_battleResult)
    {

        case EBattleResult::PlayerWon:
        {
            if (m_monster->GetID() == "Ancient_Dragon") // 상대: 드래곤, 상태: PlayerWon = 엔딩
            {
                PlaySound(NULL, NULL, SND_PURGE);  
                PlaySound(MainTheme, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOSTOP);
                std::wstring rewardMessage = m_monster->Name + L"을(를) 쓰러뜨렸다. . . ";
                LogAndWait(rewardMessage);
                LogAndWait(L"축하드립니다. 게임을 클리어하셨습니다 !");
                LogAndWait(L"© 2025 언리얼 3-4기 Yee조. All rights reserved.");
                LogAndWait(L"제작: 김준우");
                LogAndWait(L"제작: 정현수");
                LogAndWait(L"제작: 김기인");
                LogAndWait(L"제작: 곽규환");
                LogAndWait(L"제작: 김민구");
                LogAndWait(L"제작: 최윤호");
                LogAndWait(L"Special thanks to Gemini, ChatGPT, Copilot 기타 등등");
                LogAndWait(L"게임을 플레이해주셔서 감사합니다.");
            }
            else 
            {
                AttributeSet* playerStats = m_player->GetAbilityComponent()->GetAttributeSet();
                AttributeSet* monsterStats = m_monster->GetAbilityComponent()->GetAttributeSet();
                if (!playerStats || !monsterStats) break;

                // --- 보상 계산 ---
                const float BaseExpPerLevel = 50.0f;
                const float BaseGoldPerLevel = 33.0f;
                std::uniform_real_distribution<float> rewardMultiplier(0.8f, 1.2f);

                int expGained = static_cast<int>(BaseExpPerLevel * monsterStats->Level * rewardMultiplier(m_rng));
                int goldGained = static_cast<int>(BaseGoldPerLevel * monsterStats->Level * rewardMultiplier(m_rng));

                // --- 보상 지급 ---
                if(playerStats->Level < 10)    playerStats->Experience.CurrentValue += expGained;
                playerStats->Gold += goldGained;

                // 몬스터 처치 및 보상 획득 메시지를 먼저 출력
                std::wstring rewardMessage;
                rewardMessage = m_monster->Name + L"을(를) 쓰러뜨렸다! " + std::to_wstring(goldGained) + L" G를 획득했다!";
                LogAndWait(rewardMessage);

                if (playerStats->Level < 10) rewardMessage = m_player->Name + L"는 경험치 " + std::to_wstring(expGained) + L" 을(를) 획득했다!";
                else    rewardMessage = m_player->Name + L"은(는) 더 이상 경험치를 획득할 수 없다.";
                LogAndWait(rewardMessage);


                // 레벨업 체크
                AbilitySystemComponent* playerASC = m_player->GetAbilityComponent();
                bool bLeveledUp = playerASC->CheckAndProcessLevelUp();

                // 레벨업
                if (bLeveledUp)
                {
                    std::wstring levelUpMessage = m_player->Name + L"은(는) 레벨 업 했다!";
                    LogAndWait(levelUpMessage);

                    std::wstring skillName = playerASC->GetGrantedAbility(playerASC->GetAttributeSet()->Level - 1)->AbilityName;
                    levelUpMessage = m_player->Name + L"은(는) " + skillName + L"을(를) 배웠다!";
                    LogAndWait(levelUpMessage);
                }

                std::uniform_int_distribution<int> dropChance(1, 100);
                if (dropChance(m_rng) <= 30) // 드랍률 30%
                {
                    const MonsterData* monsterData = DataManager::GetInstance().GetMonsterData(m_monster->GetID());

                    const std::vector<std::string>& lootTable = monsterData->lootTable;

                    if (!lootTable.empty())
                    {
                        std::uniform_int_distribution<int> itemIndex(0, lootTable.size() - 1);
                        const std::string& droppedItemId = lootTable[itemIndex(m_rng)];

                        m_player->GetInventory()->AddItem(droppedItemId, 1);

                        const ItemData* itemData = DataManager::GetInstance().GetItemData(droppedItemId);

                        std::wstring dropMessage = m_monster->Name + L"이(가) 무언가를 떨어뜨렸다.";
                        LogAndWait(dropMessage);

                        std::wstring itemGetMessage = L"[" + itemData->Name + L"] 1개를 획득했다!";
                        LogAndWait(itemGetMessage);
                    }
                }
            }
            

            break;
        }
        case EBattleResult::PlayerLost:
            LogAndWait(m_player->Name + L"은(는) 쓰러졌다...");
            Sleep(2500);
            break;
        case EBattleResult::PlayerFled:
            break;
        default:
            break;
    }
}
// 전투 씬 체력 및 마나색 입력 함수
std::wstring BattleManager::DrawStatBar(const std::wstring& label, float current, float max, int barLength, WORD& outColor) const {
    if (current < 0) current = 0;
    float ratio = (max > 0) ? (current / max) : 0;
    int filledLength = static_cast<int>(ratio * barLength);

    std::wstring bar = label + L" [";
    for (int i = 0; i < barLength; ++i)
        bar += (i < filledLength) ? L"■" : L" ";
    bar += L"]";

    // 색상 설정
    if (label == L"HP") outColor = FOREGROUND_RED | FOREGROUND_INTENSITY;
    else if (label == L"MP") outColor = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    else outColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // 기본 흰색

    return bar;
}

void BattleManager::PlayIntroAnimation()
{
    PlaySound(NULL, NULL, SND_PURGE);
    PlaySound(encounter, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOSTOP); // 인카운터 사운드

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < m_renderer.GetWidth() * m_renderer.GetHeight(); ++j) {
            m_renderer.Draw(j % m_renderer.GetWidth(), j / m_renderer.GetWidth(), L' ', 0xFF); 
        }
        m_renderer.Render();
        Sleep(80);

        for (int j = 0; j < m_renderer.GetWidth() * m_renderer.GetHeight(); ++j) {
            m_renderer.Draw(j % m_renderer.GetWidth(), j / m_renderer.GetWidth(), L' ', 0x0F);
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