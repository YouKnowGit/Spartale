#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

// 스토리 스크립트를 진행하는 함수
void Scene_Prologue(); // 프롤로그(오프닝)
void Scene_Begin();   // 주인공 기절 후 마을에서 깨어남(게임 플레이 시작 직전).
//void Scene_Field();   // 필드에서 시작(일단은 사용 중단)
void Scene_TowerGate(); // 탑에서 올라가기 직전
void Scene_FinalBoss(); // 최상층에서 용과 조우함. 끝나면 최종 보스전.
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