#include "Player.h"
#include "Potion.h"
#include <iostream>

Potion::Potion(const std::string& name, int healAmount) 
	: name(name), healAmount(healAmount) { 
}

void Potion::use(Player& player) const {
	std::cout << "���� ��� " << name << std::endl;
	//player.heal(healAmount);
}

std::string Potion::getName() const {
	return name;
}

int Potion::getHealAmount() const {
	return healAmount;
}