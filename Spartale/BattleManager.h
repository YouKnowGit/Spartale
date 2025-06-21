#pragma once
#include <string>

// 전방 선언
class Actor;
class Player;
class Monster; 

class BattleManager {
public:
    BattleManager(Player* player, Monster* monster);
    void Run();

private:
    void ProcessPlayerTurn();
    void ProcessEnemyTurn();
    void CheckBattleStatus();
    void EndBattle();
    void Log(const std::wstring& message);
    void Draw();
    std::wstring DrawStatBar(const std::wstring& label, float current, float max, int barLength);
    void PlayIntroAnimation();

private:
    Player* m_player;
    Monster* m_monster;

    bool m_bIsBattleOver;
    bool m_bPlayerWon;
    int m_CurrentTurn;
    std::wstring m_statusMessage;
};