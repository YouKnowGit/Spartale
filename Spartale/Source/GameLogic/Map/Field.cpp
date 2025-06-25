#include "GameLogic/Map/Field.h"
#include "GameLogic/Units/Player.h"
#include "Utils/ConsoleUtils.h"

#include <iostream>
#include <fstream> // 파일 입출력
#include <json.hpp> // JSON 파싱을 위한 라이브러리


using namespace std;
using namespace ConsoleUtils;
using json = nlohmann::json;

Field::Field() : m_width(0), m_height(0) {}
Field::~Field() {}

// 파일에서 맵을 로드하는 함수
bool Field::LoadMapFromFile(int mapId)
{
    // 데이터 초기화
    //m_npcs.clear();
    m_portals.clear();
    m_mapData.clear();
    m_encounterList.clear();

    // mapFilePath 를 통해 mapData 를 읽어오기
    std::string filename = "Data/Map.json";
    std::ifstream mapFile(filename);
    if (!mapFile.is_open()) {
        std::cerr << "Error: Could not open map file: " << filename << std::endl;
        return false;
    }

    // JSON 파싱
    json jsonData;
    try {
        // 파일 스트림을 json 객체에 직접 전달하여 파싱
        mapFile >> jsonData;
    }
    catch (json::parse_error& e) {
        // JSON 형식이 잘못되었을 경우, 라이브러리가 예외를 발생시킴
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    }

    mapFile.close();

    const auto& maps = jsonData["maps"];
    auto it = std::find_if(maps.begin(), maps.end(), [mapId](const json& map) {
        return map.contains("mapID") && map["mapID"] == mapId;
        });

    if (it == maps.end()) {
        std::cerr << "Error: Map ID " << mapId << " not found in map file!" << std::endl;
        return false;
    }

    const json& selectedMap = *it;

    // Header 정보
    m_width = selectedMap["header"]["width"];
    m_height = selectedMap["header"]["height"];

    // Tiles 초기화
    const auto& tiles_json = selectedMap["tiles"];
    m_mapData.assign(m_height, std::vector<TileType>(m_width));
    for (int y = 0; y < m_height; ++y) {
        const std::string& row_str = tiles_json[y];
        for (int x = 0; x < m_width; ++x) {
            char tileChar = (x < row_str.length()) ? row_str[x] : ' ';
            switch (tileChar) {
            case '#': m_mapData[y][x] = TileType::WALL;     break;
            case '*': m_mapData[y][x] = TileType::BUSH;     break;
            case 'P': m_mapData[y][x] = TileType::PORTAL;   break;
            case '~': m_mapData[y][x] = TileType::WOOD;     break; // 맵에서 ♠를 ~로 입력해서 바꿨습니다
            case 't': m_mapData[y][x] = TileType::CROSS;    break;
            case 'N': m_mapData[y][x] = TileType::NPC;      break;
            case '&': m_mapData[y][x] = TileType::TOTEM;    break;
            case 'B': m_mapData[y][x] = TileType::BOSS;     break;
            case '■': m_mapData[y][x] = TileType::BOX;      break;
            case '$': m_mapData[y][x] = TileType::NPC_Shop; break;
            case '!': m_mapData[y][x] = TileType::NPC_Heal; break;
            case '@': m_mapData[y][x] = TileType::NPC_Skill; break;
            case '.':
            default:  m_mapData[y][x] = TileType::EMPTY; break;
            }
        }
    }


    // 포탈 정보
    const auto& portals_json = selectedMap["portals"];
    for (const auto& portal_info : portals_json) {
        if (portal_info.contains("x") && portal_info.contains("y") &&
            portal_info.contains("destMapID") &&
            portal_info.contains("destX") && portal_info.contains("destY"))
        {
            Portal newPortal;
            newPortal.destMapID = portal_info["destMapID"];
            newPortal.destX = portal_info["destX"];
            newPortal.destY = portal_info["destY"];
            m_portals[{portal_info["x"], portal_info["y"]}] = newPortal;
        }
    }

    // 몬스터 목록
    if (selectedMap.contains("encounterList")) {
        for (const auto& monsterId : selectedMap["encounterList"]) {
            m_encounterList.push_back(monsterId);
        }
    }

    // 디버깅 로그 (주석 처리)
    //std::cout << "Map loaded: " << std::endl;
    //std::cout << "Dimensions: " << m_width << " x " << m_height << std::endl;
    //std::cout << "Portal count: " << m_portals.size() << std::endl;

    return true;
}
void Field::Draw(Actor* player, ConsoleRenderer& renderer) const
{
    // 맵 타일 그리기
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            wchar_t charToDraw = L' ';
            switch (m_mapData[y][x]) {
            case TileType::EMPTY:   charToDraw = L'·'; break;
            case TileType::WALL:    charToDraw = L'▒'; break;
            case TileType::BUSH:    charToDraw = L'∗'; break; // 수풀은 2칸 차지하므로 별도 처리 필요
            case TileType::PORTAL:  charToDraw = L'回'; break;
            case TileType::WOOD:    charToDraw = L'♠'; break;
            case TileType::CROSS:   charToDraw = L't'; break;
            case TileType::NPC:     charToDraw = L'N'; break;
            case TileType::TOTEM:   charToDraw = L'&'; break;
            case TileType::BOSS:    charToDraw = L'B'; break;
            case TileType::BOX:     charToDraw = L'■'; break;
            case TileType::NPC_Shop:    charToDraw = L'$'; break;
            case TileType::NPC_Skill:   charToDraw = L'@'; break;
            case TileType::NPC_Heal:    charToDraw = L'!'; break;
            }
            renderer.Draw(x * 2, y, charToDraw);
            if (m_mapData[y][x] == TileType::BUSH) { // 수풀은 2칸
                renderer.Draw(x * 2 + 1, y, L'∗');
            }
        }
    }

    // 플레이어 그리기
    Player* p = static_cast<Player*>(player);
    gotoxy(p->GetX() * 2, p->GetY());
    switch (p->GetDirection())
    {
    case Direction::UP:     renderer.Draw(player->CurrentLocation.X * 2, player->CurrentLocation.Y, L'▲'); break;
    case Direction::DOWN:  renderer.Draw(player->CurrentLocation.X * 2, player->CurrentLocation.Y, L'▼'); break;
    case Direction::LEFT:   renderer.Draw(player->CurrentLocation.X * 2, player->CurrentLocation.Y, L'◀'); break;
    case Direction::RIGHT:  renderer.Draw(player->CurrentLocation.X * 2, player->CurrentLocation.Y, L'▶'); break;
    }
}

// 해당 타일로 이동 가능한지 확인하는 함수
bool Field::IsWalkable(int x, int y) const
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return false;
    if (m_mapData[y][x] == TileType::WALL) return false;
    if (m_mapData[y][x] == TileType::WOOD) return false;
    if (m_mapData[y][x] == TileType::TOTEM) return false;
    if (m_mapData[y][x] == TileType::CROSS) return false;
    if (m_mapData[y][x] == TileType::BOSS) return false;
    if (m_mapData[y][x] == TileType::NPC_Heal) return false;
    if (m_mapData[y][x] == TileType::NPC_Shop) return false;
    if (m_mapData[y][x] == TileType::NPC_Skill) return false;
    return true;
}

const Portal* Field::GetPortalAt(int x, int y) const
{
    auto it = m_portals.find({ x, y });
    if (it != m_portals.end())
    {
        return &it->second;
    }
    return nullptr;
}

TileType Field::GetTileType(int x, int y)
{
    // 맵 경계를 벗어나면 충돌을 방지하기 위해 WALL로 간주
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
    {
        return TileType::WALL;
    }
    // 맵 경계 안이라면 해당 좌표의 타일 타입을 반환
    return m_mapData[y][x];
}