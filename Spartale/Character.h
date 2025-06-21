#pragma once
#include <string>

using namespace std;

struct CharacterStats {
	int strength = 0;
	int dexterity = 0;
	int intelligence = 0;

};
class Character
{
private:

	std::string name;
	int level;
	int health;
	int experience;
	int expToLevelUp;

	CharacterStats stats;

public:
	

	Character(const std::string& name, int lelvel);
	void displayStatus() const;
	void levelUp();
	void gainExperience(int amount);

	void setStats(int str, int dex, int intel);
	const CharacterStats& getstats() const;
	
};