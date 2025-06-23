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
bool Field::LoadMapFromFile(const std::string& mapFilePath)
{
    // 데이터 초기화
    //m_npcs.clear();
    m_portals.clear();
    m_mapData.clear();

    // mapFilePath 를 통해 mapData 를 읽어오기
    std::ifstream mapFile(mapFilePath);
    if (!mapFile.is_open()) {
        std::cerr << "Error: Could not open map file: " << mapFilePath << std::endl;
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

    // 파싱된 JSON 데이터에서 맵 정보를 추출
    // Header 정보 추출
    m_width = jsonData["header"]["width"];
    m_height = jsonData["header"]["height"];

    // Tiles 정보 추출
    m_mapData.assign(m_height, std::vector<TileType>(m_width));
    const auto& tiles_json = jsonData["tiles"];
    for (int y = 0; y < m_height; ++y) {
        const std::string& row_str = tiles_json[y];
        for (int x = 0; x < m_width; ++x) {
            char tileChar = (x < row_str.length()) ? row_str[x] : ' ';
            switch (tileChar) {
            case '#': m_mapData[y][x] = TileType::WALL; break;
            case '*': m_mapData[y][x] = TileType::BUSH; break;
            case 'P': m_mapData[y][x] = TileType::PORTAL; break;
            case '.':
            default:  m_mapData[y][x] = TileType::EMPTY; break;
            }
        }
    }

    // Portals 정보 추출
    const auto& portals_json = jsonData["portals"];
    for (const auto& portal_info : portals_json) {
        int x = portal_info["x"];
        int y = portal_info["y"];

        // portal_info 객체에 필요한 키들이 모두 있는지 확인
        if (portal_info.contains("destMapID") &&
            portal_info.contains("destX") &&
            portal_info.contains("destY"))
        {
            // 키가 모두 존재할 때만 Portal 객체를 만들어 값을 할당
            Portal newPortal;
            newPortal.destMapID = portal_info["destMapID"];
            newPortal.destX = portal_info["destX"];
            newPortal.destY = portal_info["destY"];
            m_portals[{x, y}] = newPortal;
        }
        else
        {
            // 키가 하나라도 없으면 에러 메시지 출력
            std::cerr << "Error: Portal data at (" << x << "," << y << ") is missing required keys in JSON!" << std::endl;
            system("pause"); // 디버깅을 위해 일시 정지
        }
    }
    mapFile.close();

    // 디버깅 로그
    std::cout << "Map loaded: " << mapFilePath << std::endl;
    std::cout << "Dimensions: " << m_width << " x " << m_height << std::endl;
    std::cout << "Portal count: " << m_portals.size() << std::endl;
    
    //system("pause");

    return true;
}
void Field::Draw(Actor* player, ConsoleRenderer& renderer) const
{
    // 맵 타일 그리기
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            wchar_t charToDraw = L' ';
            switch (m_mapData[y][x]) {
            case TileType::EMPTY:  charToDraw = L'·'; break;
            case TileType::WALL:   charToDraw = L'▒'; break;
            case TileType::BUSH:   charToDraw = L'∗'; break; // 수풀은 2칸 차지하므로 별도 처리 필요
            case TileType::PORTAL: charToDraw = L'回'; break;
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
    /*
    for (NPC* npc : m_npcs)
    {
        gotoxy(npc->GetX() * 2, npc->GetY());
        switch (npc->GetType())
        {
        case NPCType::HEALER:   wcout << L"†"; break;
        case NPCType::SHOP_ITEM: wcout << L"●"; break;
        case NPCType::SHOP_SKILL: wcout << L"◈"; break;
        case NPCType::BOSS:
            // 글자를 출력하기 전에 색상 변경
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
            wcout << L"▼";
            // 글자 출력 후 즉시 원래 색상으로 복원
            SetConsoleTextAttribute(hConsole, saved_attributes);
            break;
        }
    }
    
    Player* p = static_cast<Player*>(player);
    gotoxy(p->GetX() * 2, p->GetY());
    switch (p->GetDirection())
    {
    case Direction::UP:     wcout << L"▲"; break;
    case Direction::DOWN:   wcout << L"▼"; break;
    case Direction::LEFT:   wcout << L"◀"; break;
    case Direction::RIGHT:  wcout << L"▶"; break;
    }

    // 디버그용 플레이어 위치 출력
    gotoxy(0, m_height + 1);
    wcout << L"Player Position: (" << p->GetX() << L", " << p->GetY() << L")   ";
    */
}

// 해당 타일로 이동 가능한지 확인하는 함수
bool Field::IsWalkable(int x, int y) const
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return false;
    if (m_mapData[y][x] == TileType::WALL) return false;
    // if (GetNPCAt(x, y) != nullptr) return false;
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