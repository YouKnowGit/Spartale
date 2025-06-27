#include <iostream>
#include <memory>
#include <fcntl.h>
#include <io.h>
#include <Windows.h>

#include "Utils/ConsoleUtils.h"
#include "Core/GameWorld.h"
#include "GameLogic/Units/Player.h"
#include "GameLogic/DataManager.h"
#include "GameLogic/MainMenu.h"
#include "GameLogic/SaveManager.h"
#include "GameLogic/Skills/SkillFactory.h"
#include "Framework/AbilitySystem/AbilitySystemComponent.h"
#include "Framework/AbilitySystem/GameplayAbility.h"

void PrintString(const std::wstring& text, SHORT x, SHORT y);

using namespace std;

int main()
{

    _setmode(_fileno(stdin), _O_U16TEXT);

    ConsoleUtils::ShowConsoleCursor(false);

    DataManager::GetInstance().LoadMonsterData("Data/Monsters.json");
    DataManager::GetInstance().LoadSkillData("Data/Skills.json");
	DataManager::GetInstance().LoadItemData("Data/Item.json");
	DataManager::GetInstance().LoadShopData("Data/Shop.json");
    
    EGameState currentState = EGameState::MainMenu;
    ConsoleRenderer renderer;
    renderer.Initialize();

    while (currentState != EGameState::Quit)
    {
        switch (currentState)
        {
            case EGameState::MainMenu:
            
            {
                MainMenu menu(renderer);
                currentState = menu.Run();
                break;
            }
            case EGameState::World:
            {
                std::wstring name; 
                SetConsoleOutputCP(CP_UTF8);

                system("cls");
                PrintString(L"주인공의 이름을 입력해주세요: ", 30, 15);

                ConsoleUtils::ShowConsoleCursor(true);

                getline(std::wcin, name);

                ConsoleUtils::ShowConsoleCursor(false);

                auto player = make_unique<Player>(name);

                player->GetAbilityComponent()->EquipAbility(0, player->GetAbilityComponent()->GetGrantedAbility(0));

                // 시작 아이템으로 HP 포션을 3개와 청동 검을 추가합니다.
                //
                player->GetInventory()->AddItem("consume_potion_01", 3);
                player->GetInventory()->AddItem("equip_weapon_sword_01", 1);
                GameWorld world(std::move(player));

                world.Run();

                currentState = EGameState::MainMenu;
                break;
            }
            case EGameState::LoadGame:
            {
                auto loadPlayer = make_unique<Player>(L" ");
                SaveManager sm(*loadPlayer);
                sm.LoadGame("save.txt");

                GameWorld world(std::move(loadPlayer));
                world.Run();
                currentState = EGameState::MainMenu;
                break;
            }
        }
    }

    renderer.Clear();
    std::wstring exitMessage = L"게임을 종료합니다.";
    for (int i = 0; i < exitMessage.length(); ++i)
    {
        renderer.Draw(30 + i * 2, 12, exitMessage[i]);
    }
    renderer.Render();
    Sleep(2000);
    ConsoleUtils::ShowConsoleCursor(true);

    return 0;
}
void PrintString(const std::wstring & text, SHORT x, SHORT y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;

    COORD pos = { x, y };
    SetConsoleCursorPosition(hConsole, pos);
    WriteConsoleW(hConsole, text.c_str(), (DWORD)text.length(), &written, nullptr);
}
