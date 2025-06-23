#include "ItemBase.h"
// ItemBase Ŭ������ ������ ����
ItemBase::ItemBase(std::wstring name, std::wstring description)
    : m_name(name), m_description(description)
{
}

std::wstring ItemBase::GetName() const
{
    return m_name;
}

std::wstring ItemBase::GetDescription() const
{
    return m_description;
}