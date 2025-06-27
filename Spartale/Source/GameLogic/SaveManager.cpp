#include "GameLogic/SaveManager.h"
#include "GameLogic/Units/Player.h"
#include "Utils/StringUtils.h"

SaveManager::SaveManager(Player& player) : m_player(player) {}

void SaveManager::SaveGame(const std::string& filename)
{
	std::ofstream saveFile(filename);
	if (!saveFile.is_open()) return;

	m_player.Save(saveFile);
	saveFile.close();
	EncryptDecryptFile(filename, 1);
}

void SaveManager::LoadGame(const std::string& filename)
{
	EncryptDecryptFile(filename, 1);
	std::ifstream loadFile(filename);
	if (!loadFile.is_open()) return;

	m_player.Load(loadFile);
	loadFile.close();
	EncryptDecryptFile(filename, 1);
}
void SaveManager::EncryptDecryptFile(const std::string& filename, char key)
{
	std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);

	if (!file.is_open())	return;

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(fileSize);
	file.read(buffer.data(), fileSize);

	for (char& byte : buffer)	byte ^= key;

	file.seekp(0, std::ios::beg);
	file.write(buffer.data(), fileSize);

	file.close();
}