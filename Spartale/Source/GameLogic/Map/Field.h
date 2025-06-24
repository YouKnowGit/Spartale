#pragma once
#include <vector>
#include <string>
#include <map>
#include <utility>

#include "Core/Global.h"
#include "Utils/ConsoleRenderer.h"

class Actor;

struct Portal
{
	int destMapID;
	int destX, destY;
};

class Field
{
public:
	Field();
	~Field();

	bool LoadMapFromFile(int mapId);

	void Draw(Actor* player, ConsoleRenderer& renderer) const;
	bool IsWalkable(int x, int y) const;
	TileType GetTileType(int x, int y);
	const Portal* GetPortalAt(int x, int y) const;

	// Getter
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	const std::vector<std::string>& GetEncounterList() const { return m_encounterList; }

private:
	int m_width;
	int m_height;

	std::vector<std::vector<TileType>> m_mapData;
	std::map<std::pair<int, int>, Portal> m_portals;
	std::vector<std::string> m_encounterList; // 맵에 출현하는 몬스터 ID 목록
};