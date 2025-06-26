#pragma once
#include "Core/Global.h"
#include <string>
#include <vector>
#include <random>

// 전방 선언
class Player;
class Monster;
class ConsoleRenderer;

// 전투의 현재 상태를 명확하게 나타내는 열거형
enum class EBattleState
{
    Intro,              // 전투 시작 메시지 표시
    PlayerActionSelect,   // 플레이어 행동 선택 (공격, 방어...)
    PlayerSkillSelect,    // 플레이어 스킬 선택
    ExecutingPlayerAction, // 플레이어 행동 실행 및 결과 표시
    EnemyTurn,            // 적 턴 실행
    TurnEnd,              // 턴 종료 (지속 효과 등 처리)
    BattleOver            // 전투 종료 메시지 표시
};

class BattleManager {
public:
    BattleManager(Player* player, Monster* monster, ConsoleRenderer& renderer);
    ~BattleManager() = default;

    // 이 함수를 호출하면 전투가 시작되고 끝날 때까지 모든 것을 처리
    EBattleResult Run();

private:
    // 메인 루프에서 호출되는 핵심 함수들
    void ProcessInput(); // 키보드 입력 처리
    void Update();       // 상태에 따른 게임 로직 업데이트
    void Render();       // 현재 상태를 화면에 그리기

    // 그리기 헬퍼 함수들
    void DrawUI();
    void DrawActionSelectMenu();
    void DrawSkillSelectMenu();

    // 기타 헬퍼 함수들
    void Log(const std::wstring& message);
    void LogAndWait(const std::wstring& message);

    void EndBattle();
    std::wstring DrawStatBar(const std::wstring& label, float current, float max, int barLength) const;
    void CheckBattleStatus();
    bool AttemptToFlee();

    void PlayIntroAnimation();

private:
    // 핵심 멤버 변수
    Player* m_player;
    Monster* m_monster;
    ConsoleRenderer& m_renderer; // 참조로 받아서, 소유하지 않음

    // 전투 상태 관리 변수
    EBattleState m_battleState;
    EBattleResult m_battleResult; // 전투 결과를 저장할 변수

    bool m_bIsOne = false;
    bool m_bIsBattleOver;
    int m_CurrentTurn;

    // UI 및 메시지 관련 변수
    std::wstring m_statusMessage;
    int m_currentMenuSelection;
    std::vector<std::wstring> m_currentMenuOptions;

    // 난수 생성 엔진을 멤버 변수로 추가
    std::mt19937 m_rng;

    const wchar_t* m_navigateSoundPath;
    const wchar_t* m_confirmSoundPath;
    const wchar_t* m_escSoundPath;
    const wchar_t* meet = L"Sounds/Meet.wav";
    const wchar_t* win = L"Sounds/Win.wav";
    const wchar_t* encounter = L"Sounds/enemy_encounter.wav";
    const wchar_t* Lose = L"Sounds/Lose.wav";
    const wchar_t* Runway = L"Sounds/Runway.wav";
    const wchar_t* RunwayFailure = L"Sounds/RunwayFailure";
};