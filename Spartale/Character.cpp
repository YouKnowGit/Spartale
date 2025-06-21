#include "Character.h"
#include <iostream>


Character::Character(const std::string& name, int level)
	: name(name), level(level), health(100),experience(0), expToLevelUp(100){}


void Character::setStats(int str, int dex, int intel)
{
	stats.strength = str;
	stats.dexterity = dex;
	stats.intelligence = intel;
}

const CharacterStats& Character::getstats() const {

	return stats;
}
void Character::displayStatus()const 
{
	std::cout << "ÀÌ¸§: " << name << "\n";
	std::cout << "·¹º§: " << level << "\n";
	std::cout << "Èû: " << stats.strength << "\n";
	std::cout << "¹ÎÃ¸: " << stats.dexterity << "\n";
	std::cout << "Áö´É: " << stats.intelligence << "\n";
		
		

}

void Character::levelUp()
{
	level++;
	health += 20;
	experience -= expToLevelUp;
	
}

void Character::gainExperience(int exp) 
{
	experience += exp;
	while (experience >= expToLevelUp) 
	{
		levelUp();
	}
}


