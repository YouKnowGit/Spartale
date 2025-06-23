#pragma once
#include "ItemBase.h"
class HealthPotion : public ItemBase
{
public:
    HealthPotion();
    virtual void Use(Actor* user) override;
};