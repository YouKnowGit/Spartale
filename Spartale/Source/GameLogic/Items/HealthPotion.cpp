// HealthPotion.cpp
#include "HealthPotion.h"
#include "Actor.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include <iostream>
using namespace std;
HealthPotion::HealthPotion()
    : ItemBase(L"", L"HP 50 ")
{
}
inline void HealthPotion::Use(Actor* user)
{
    if (!user) return;
    AbilitySystemComponent* asc = user->GetAbilitySystemComponent();
    AttributeSet* attr = asc->GetAttributeSet();
    // +50 HP ȿ���� ����
    GameplayEffect effect(attr, EAttributeOperation::Add, 50.0f);
    // ASC�� ȿ�� ������ ��û
    asc->ApplyGameplayEffect(&effect);
    // �ֿܼ��� ������ �޽��� ��� (���� ���ӿ����� �� ������ BattleManager�� UIManager�� ���)
    wcout << L"\n" << m_name << L". " << m_description << endl;
}