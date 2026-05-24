#pragma once
#include <string>



//массив имен 
const std::vector<std::string> heroNames =
{
	"Dragon Knight",
	"Wraith King",
	"Juggernaut",
	"Drow Ranger",
	"Axe"
	//"Sven"
};

struct Hero
{
	int id = -1; // id перса (чтоб отличать одного от другого)
	int type = 0; // 0 - дк (1 некст герой к примеру)
	int lvl = 1; // лвл
	
	float hp;   //текущее хп меняем в бою
	float maxhp; // лимит хп
	float damage; //урон за тычку 
	float armor; //армор 
	float lerpHP; // для визуала ХП (отнимания для красоты)

	std::string name; //heroname 

	float range; // 3-5 ренж  1.0 - 1.5 ближники (мб поменяю потом)

	//данные при покупне перса 1го лвлва 
	void StatsHero(std::string heroName,float startHp, float startDamage,float startArmor, float startRange)
	{
		name = heroName;
		maxhp = startHp; 
		hp = maxhp;
		damage = startDamage; 
		armor = startArmor;
		lerpHP = maxhp; // изначально равно макс хп 
		range = startRange; // редж аттаки
	}

	//лвл ап 
	void lvlUpHero(float newMaxHp, float newDamage, float newArmor)
	{
		if (lvl < 3)
		{
			lvl++; 
			maxhp = newMaxHp;
			hp = maxhp; // гарант фулл хп при апе лвла
			damage = newDamage;
			armor = newArmor;

		}
	}
	//АРМОР МАТЕША
	void Armore(float takeDamage)
	{
		// armor / 100  (превращаем 5 в 0.05)
		// типо 1.0f - 0.05 = 0.95/100 = 95% урона получаем
		float damageFormula = 1.0f - (armor / 100.0f);

		// чтоб броня 100% не лечила героя делаем лимит (минимум 1% урона пройдет)
		if (damageFormula < 0.01f) damageFormula = 0.01f;


		hp -= takeDamage * damageFormula;
		if (hp < 0) hp = 0;
	}
};