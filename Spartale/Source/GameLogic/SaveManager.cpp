#include "GameLogic/SaveManager.h"
#include "GameLogic/Units/Player.h"
#include "Utils/StringUtils.h"

SaveManager::SaveManager(Player& player) : m_player(player) {}

void SaveManager::SaveGame(const std::string& filename) const
{
	std::ofstream saveFile(filename);
	if (!saveFile.is_open()) return;

	m_player.Save(saveFile);
	saveFile.close();
}

void SaveManager::LoadGame(const std::string& filename)
{
	std::ifstream loadFile(filename);
	if (!loadFile.is_open()) return;

	m_player.Load(loadFile);
	loadFile.close();
}