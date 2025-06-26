#pragma once
#include <string>
#include <fstream>
#include <memory>
#include <vector>

class Player;

class SaveManager
{
public:
	SaveManager(Player& player);

	void SaveGame(const std::string& filename);
	void LoadGame(const std::string& filename);
	void EncryptDecryptFile(const std::string& filename, char key);

private:
	Player& m_player;
};