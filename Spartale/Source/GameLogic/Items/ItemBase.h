
#pragma once
#include <string>
using namespace std;
class Actor; // ���� ����
class ItemBase
{
    std::wstring Name;

public:
    ItemBase(wstring name, wstring description);
    ItemBase(const std::wstring& name) : Name(name) {}
    
    virtual ~ItemBase() {}
    virtual void Use(Actor* user) = 0;
    wstring GetName() const;
    wstring GetDescription() const;

    virtual bool IsEquippable() const { return false; }  // 기본은 false
protected:
    wstring m_name;
    wstring m_description;
};