#pragma once

#include <string>
#include <vector>

// ���� ����
class Player;
class Monster;
class ConsoleRenderer;

// ������ ���� ���¸� ��Ȯ�ϰ� ��Ÿ���� ������
enum class EBattleState
{
    Intro,              // ���� ���� �޽��� ǥ��
    PlayerActionSelect,   // �÷��̾� �ൿ ���� (����, ���...)
    PlayerSkillSelect,    // �÷��̾� ��ų ����
    ExecutingPlayerAction, // �÷��̾� �ൿ ���� �� ��� ǥ��
    EnemyTurn,            // �� �� ����
    TurnEnd,              // �� ���� (���� ȿ�� �� ó��)
    BattleOver            // ���� ���� �޽��� ǥ��
};

class BattleManager {
public:
    BattleManager(Player* player, Monster* monster, ConsoleRenderer& renderer);
    ~BattleManager() = default;

    // �� �Լ��� ȣ���ϸ� ������ ���۵ǰ� ���� ������ ��� ���� ó��
    void Run();

private:
    // ���� �������� ȣ��Ǵ� �ٽ� �Լ���
    void ProcessInput(); // Ű���� �Է� ó��
    void Update();       // ���¿� ���� ���� ���� ������Ʈ
    void Render();       // ���� ���¸� ȭ�鿡 �׸���

    // �׸��� ���� �Լ���
    void DrawUI();
    void DrawActionSelectMenu();
    void DrawSkillSelectMenu();

    // ��Ÿ ���� �Լ���
    void Log(const std::wstring& message);
    void LogAndWait(const std::wstring& message);

    void EndBattle();
    std::wstring DrawStatBar(const std::wstring& label, float current, float max, int barLength) const;
    void CheckBattleStatus();

    // ���ڿ� �׸��⸦ ���� ��ƿ��Ƽ
    void DrawString(int x, int y, const std::wstring& str) const;

	void PlayIntroAnimation();

private:
    // �ٽ� ��� ����
    Player* m_player;
    Monster* m_monster;
    ConsoleRenderer& m_renderer; // ������ �޾Ƽ�, �������� ����

    // ���� ���� ���� ����
    EBattleState m_battleState;
    bool m_bIsBattleOver;
    bool m_bPlayerWon;
    int m_CurrentTurn;

    // UI �� �޽��� ���� ����
    std::wstring m_statusMessage;
    int m_currentMenuSelection;
    std::vector<std::wstring> m_currentMenuOptions;
};