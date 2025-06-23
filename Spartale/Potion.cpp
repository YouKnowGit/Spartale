#include "Player.h"
#include "Potion.h"

Potion::Potion(const std::string& name, int healAmount) : name(name), healAmount(healAmount) { 
}

void Potion::use(Player& player) const {
	
}