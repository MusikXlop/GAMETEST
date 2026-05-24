#pragma once
#include <raylib.h>
#include "Setka.h"

class ProfileHero
{
public:
	ProfileHero();
	~ProfileHero();

	void LoadRes();
	void Draw(UnitSetka* unit);

private:
	Texture2D BackGroundInfo;
	Texture2D iconDamage;
	Texture2D iconArmor;
	Texture2D iconAttackSpeed;

	Font profileFont;
	Font profileFontForHero;

	Texture star1;
	Texture star2;
	Texture star3;
};