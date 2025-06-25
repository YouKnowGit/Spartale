#pragma once
#include <string>
#include <fstream>
#include <memory>

class Player;

class SaveManager
{
public:
	SaveManager(Player& player);

	void SaveGame(const std::string& filename) const;
	void LoadGame(const std::string& filename);

private:
	Player& m_player;
};