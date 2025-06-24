#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

// ���丮 ���� ��� �Լ���
void Scene_Prologue(); // ���ѷα�(����)
void Scene_Begin();   // ������ ���� - �������� ���.
void Scene_Field();   // �ʵ� ù ���� ��
void Scene_TowerGate(); // ž�� �� �տ���
void Scene_FinalBoss(); // �������� ���� �� ���. ������ ���� ���� ����.
void Scene_Epiloge(); // ���ʷα�(����)

void typeEffect(std::string text, int delay = 30);

void waitForEnter() {
	std::cout << "\n[Enter�� ���� ����ϼ���...]";
	std::cin.ignore();
	std::cin.get();
	std::cout << std::endl;
}

void typeEffect(std::string text, int delay = 30) {
	for (char c : text) {
		std::cout << c << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}
	std::cout << std::endl;
}