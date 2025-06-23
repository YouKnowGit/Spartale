#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

// 스토리 관련 장면 함수들
void Scene_Prologe(); // 프롤로그(시작)
void Scene_Begin();   // 게임의 시작 - 마을에서 깨어남.
void Scene_Field();   // 필드 첫 진입 후
void Scene_TowerGate(); // 탑의 문 앞에서
void Scene_FinalBoss(); // 보스와의 조우 후 대사. 끝나면 최종 전투 진입.
void Scene_Epiloge(); // 에필로그(엔딩)

void typeEffect(std::string text, int delay = 30);

void waitForEnter() {
	std::cout << "\n[Enter를 눌러 계속하세요...]";
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