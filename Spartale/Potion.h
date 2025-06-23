#pragma once
#include <memory>
#include "Player.h"

class Potion {
private:
	std::string name;
	int healAmount;

public:
	Potion(const std::string& name, int healAmount);

	void use(Player& player) const;

	std::string getName() const;
	int getHealAmount() const;
};