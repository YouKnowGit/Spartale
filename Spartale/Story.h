#pragma once
#include <iostream>
#include <string>

// ���丮 ���� ��� �Լ���
void Scene_Prologe(); // ���ѷα�(����)
void Scene_Begin();   // ������ ���� - �������� ���.
void Scene_Field();   // �ʵ� ù ���� ��
void Scene_TowerGate(); // ž�� �� �տ���
void Scene_FinalBoss(); // �������� ���� �� ���. ������ ���� ���� ����.
void Scene_Epiloge(); // ���ʷα�(����)

void typeEffect(std::string text, int delay = 30);