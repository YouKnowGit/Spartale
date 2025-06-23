#pragma once
/*********************************************/
/* �� ������ �ڵ忡 ���� �������� �������� ����*/
/**********************************************/

// �� ���� ��ġ�� ��Ÿ���� ������ ����ü
struct FMapLocation
{
    int X = 0;
    int Y = 0;
};

// �����÷��� ����Ʈ�� ���� ����� �����ϴ� ������
enum class EEffectApplication
{
    Instant,    // ��� ����
    Duration,   // ���� �ð� ���� ����
    Infinite    // ���� ����
};

// �������� �Ӽ��� �����ϴ� ������
enum class EDamageType
{
    None,
    Physical, // ����
    Magical,  // ����
    True      // ����
};

// �����÷��� ����Ʈ�� ���� ��Ŀ� ���� ���� �ð� ������ �����ϴ� ������
enum class EModifierOperation
{
    Add,       // ���ϱ� (�⺻)
    Multiply,  // ���ϱ�
    Override   // �����
};

// Field ������ Ÿ�� Ÿ���� �����ϴ� ������
enum class TileType
{
    EMPTY = 0,
    WALL = 1,
    BUSH = 2,
    PORTAL = 3
};

// ������ ���� ���¸� ��Ÿ���� ������
enum class EGameState
{
    World,
    Battle,
    MainMenu,
    PauseMenu,
    Iventory,
    Quit
};