#pragma once
#include <string>
#include <memory>

// 전방 선언
class Item;

class ItemFactory
{
public:
	static std::unique_ptr<Item> CreateItem(const std::string& itemID);
};