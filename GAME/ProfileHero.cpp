#include "ProfileHero.h"

ProfileHero::ProfileHero()
{
}

ProfileHero::~ProfileHero()
{
	UnloadTexture(BackGroundInfo);
	UnloadTexture(iconDamage);
	UnloadTexture(iconArmor);
	UnloadTexture(iconAttackSpeed);

	UnloadFont(profileFont);
	UnloadFont(profileFontForHero);
	UnloadTexture(star1);
	UnloadTexture(star2);
	UnloadTexture(star3);
}

void ProfileHero::LoadRes()
{
	BackGroundInfo = LoadTexture("fonmeny/right_bar_backer_1_item_icons_psd.png");
	iconDamage = LoadTexture("starslvl/attack_damage.png");
	iconArmor = LoadTexture("starslvl/armor.png");
	iconAttackSpeed = LoadTexture("starslvl/icon_attack_speed3_psd.png");

	profileFont = LoadFontEx("fonts/UbuntuMono-BoldItalic.ttf", 32, 0, 250);
	profileFontForHero = LoadFontEx("fonts/UbuntuMono-BoldItalic.ttf", 32, 0, 250);



	star1 = LoadTexture("starslvl/star_rank1_psd.png");
	star2 = LoadTexture("starslvl/star_rank2_psd.png");
	star3 = LoadTexture("starslvl/star_rank3_psd.png");
}

void ProfileHero::Draw(UnitSetka* unit)
{
	if (unit == nullptr || !unit->active) return;

	//мое бек граунд инфо (прямоугольник панели слева инвентаря)
	//растянул с 248х280 до 250 550
	Rectangle panelRect = { (float)GetScreenWidth() - 430, 250,250,540 };

	// основной фон (рисовка)
	DrawTexturePro(BackGroundInfo, { 0,0,(float)BackGroundInfo.width,(float)BackGroundInfo.height },
		panelRect, { 0,0 }, 0, WHITE);

	// портрет перса (центрируем его по горизонтали внутри бекграунда)
	float portretX = panelRect.x + (panelRect.width / 2 - unit->portret.width / 2);
	DrawTexture(unit->portret, portretX, panelRect.y + 20, WHITE);

	//имя и лвл
	DrawTextEx(profileFontForHero, unit->name.c_str(), { panelRect.x + 20, panelRect.y + 110 }, 24, 1, WHITE);
	
	//лвл 
	float starY = panelRect.y + 135;

	//используем unit-lvl 
	if (unit->lvl == 1)
	{
		DrawTextureEx(star1, { panelRect.x + 20, starY }, 0.0f, 1.0f, WHITE);
	}
	else if (unit->lvl == 2)
	{
		DrawTextureEx(star2, { panelRect.x + 20, starY }, 0.0f, 1.0f, WHITE);
		DrawTextureEx(star2, { panelRect.x + 55, starY }, 0.0f, 1.0f, WHITE);
	}
	else
	{
		DrawTextureEx(star3, { panelRect.x + 20, starY }, 0.0f, 1.0f, WHITE);
		DrawTextureEx(star3, { panelRect.x + 55, starY }, 0.0f, 1.0f, WHITE);
		DrawTextureEx(star3, { panelRect.x + 90, starY }, 0.0f, 1.0f, WHITE);
	}

	float hpBarY = panelRect.y + 175;
	float hpBarWidth = 210; 
	float hpBarHeight = 25;

	//фон полоски (темно серый)
	DrawRectangle(panelRect.x + 20, hpBarY, hpBarWidth, hpBarHeight, DARKGRAY);

	//проценты заполнения
	float hpPercent = (float)unit->stats.hp / unit->stats.maxhp;
	if (hpPercent < 0) hpPercent = 0;

	//полоска хп зеленая
	DrawRectangle(panelRect.x + 20, hpBarY, hpBarWidth * hpPercent, hpBarHeight, GREEN);

	//текст внутри полоски (текущее/макс) 
	const char* hpText = TextFormat("%d / %d", (int)unit->stats.hp, (int)unit->stats.maxhp);
	Vector2 hpTextSize = MeasureTextEx(profileFontForHero, hpText, 18, 1);
	DrawTextEx(profileFontForHero, hpText, { panelRect.x + 20 + (hpBarWidth / 2 - hpTextSize.x / 2), hpBarY + 4 }, 18, 1, WHITE);


	int statsoffSetY = 40; // еще смещение 

	int totalDamage = (int)unit->stats.damage;
	int displayBaseDamage = totalDamage - unit->bonusDamage;

	//урон (baseDamage и bonusDamage)
	DrawTextureEx(iconDamage, { panelRect.x + 20, panelRect.y + 185 + statsoffSetY }, 0, 0.4f, WHITE);
	
	//базовая
	DrawTextEx(profileFontForHero, TextFormat("%d", displayBaseDamage),
		{ panelRect.x + 60, panelRect.y + 190 + statsoffSetY }, 20, 1, WHITE);
	
	//если есть бонус то рисуем 
	if (unit->bonusDamage > 0)
	{
		Vector2 baseDmgSize = MeasureTextEx(profileFontForHero, TextFormat("%d", displayBaseDamage), 20, 1);
		
		
		DrawTextEx(profileFontForHero, TextFormat(" +%d", (int)unit->bonusDamage),
			{ panelRect.x + 60 + baseDmgSize.x + 5, panelRect.y + 190 + statsoffSetY }, 20, 1, GREEN);

	}



	int totalArmor = (int)unit->stats.armor;
	int displayBaseArmor = totalArmor - unit->bonusArmor;

	//броня 
	DrawTextureEx(iconArmor, { panelRect.x + 20, panelRect.y + 230 + statsoffSetY }, 0, 0.4f, WHITE);
	DrawTextEx(profileFontForHero, TextFormat("%d", displayBaseArmor), { panelRect.x + 60, panelRect.y + 235 + statsoffSetY }, 20, 1, WHITE);	if (unit->bonusArmor > 0)
	{
		Vector2 baseArmorSize = MeasureTextEx(profileFontForHero, TextFormat("%d", displayBaseArmor), 20, 1);
		DrawTextEx(profileFontForHero, TextFormat("+ %d", (int)unit->bonusArmor), { panelRect.x + 60 + baseArmorSize.x + 5,
			panelRect.y + 235 + statsoffSetY }, 20, 1, GREEN);

	}


	//базовая скорость аттаки
	float totalAtackSpeed = (int)unit->attackSpeed;
	float displayBaseAS = totalAtackSpeed;

	//attack speed
	DrawTextureEx(iconAttackSpeed, { panelRect.x + 20, panelRect.y + 275 + statsoffSetY },0,0.9f, WHITE);
	DrawTextEx(profileFontForHero, TextFormat("%.2f", displayBaseAS), { panelRect.x + 60, panelRect.y + 280 + statsoffSetY }, 20, 1, WHITE);	if (unit->bonusAS > 0.0f)
	{
		Vector2 baseAttackSpeedSize = MeasureTextEx(profileFontForHero, TextFormat("%.2f", displayBaseAS), 20, 1);
		
		DrawTextEx(profileFontForHero, TextFormat(" +%.2f", unit->bonusAS), 
			{ panelRect.x + 60 + baseAttackSpeedSize.x + 5, panelRect.y + 280 + statsoffSetY }, 20, 1, GREEN);
	}

	//спелы 
	if (unit->hasSpell)
	{
		DrawText("Spell:", panelRect.x + 20, panelRect.y + 335 + statsoffSetY, 18, GRAY);

		//координаты иконки
		Vector2 spellPos = { panelRect.x + 20, panelRect.y + 355 + statsoffSetY };

		//сама иконка 
		DrawTexture(unit->spellicon, spellPos.x, spellPos.y, WHITE);

		//логика кд
		//считаем скока сек осталось ждать
		float timeLeft = unit->skillCoolDown - unit->skillTimer;

		//если время еще есть(не готова)
		if (timeLeft > 0.05f) //0.05 что мерцания не было
		{
			//затемняем иконку (полу прозрачный квадрат поверх) 
			DrawRectangle(spellPos.x, spellPos.y, unit->spellicon.width, unit->spellicon.height,Fade(BLACK,0.6f));


			//вычисляем текст таймера(целое число в секундах)
			const char* cooldownText = TextFormat("%d", (int)ceil(timeLeft));
			//ceil(timeleft)-  округление в большую сорону

			//ценетрируе поверх иконки 
			int fontSize = 25;
			int textWidth = MeasureText(cooldownText, fontSize);

			//рисуем число в секундах белым по центру 
			DrawText(cooldownText,
				spellPos.x + (unit->spellicon.width / 2) - (textWidth / 2),
				spellPos.y + (unit->spellicon.height / 2) - (fontSize / 2),
				fontSize, WHITE);
		}
	}

	//логика описания 
	if (unit->hasSpell)
	{
		//создаем область проверки при наведении 
		Rectangle skillRect =
		{
			panelRect.x + 20,
			panelRect.y + 355 + statsoffSetY,
			(float)unit->spellicon.width,
			(float)unit->spellicon.height
		};


		if (CheckCollisionPointRec(GetMousePosition(), skillRect))
		{
			const char* skillName = "Soon...";
			const char* desc = "Soon..."; //описание 

			// выбираем описание по айди
			switch (unit->unitID)
			{
				case 0:
				{
					skillName = "Dragon Blood";
					desc = "Armor Bonus: 5/7/10";
					break;
				}
				case 1:
				{
					skillName = "Vampiric Spirit";
					desc = "Max Skeleton Charges: 1/2/3";
					break;
				}
				case 2:
				{
					skillName = "Blade Dance";
					desc = "Critical Damage: 2/3/4";
					break;
				}
				case 3:
				{
					skillName = "Marksmanship";
					desc = "Pierces through the enemy's \ndefenses,ignoring their base armor \nProc Chance: 20% ";
					break;
				}
				case 4:
				{
					skillName = "Counter Helix";

					desc = "Axe will perform a helix counter \nattack, dealing Physical damage\nto all nearby enemies.\nDamage: 1.2x attack";

					break;
				}

			}
			//рамка для описания 
			float descWidth = 350;
			float descHeight = 150;

			//смещение влево 
			float descTipX = panelRect.x - descWidth - 10;
			float descTipY = skillRect.y - 20;

			//подложка 
			DrawRectangle(descTipX, descTipY, descWidth, descHeight, ColorAlpha(BLACK, 0.9f));
			DrawRectangleLinesEx({ descTipX, descTipY, descWidth,descHeight }, 2, GRAY);

			//названия спела (оранжевое)
			DrawTextEx(profileFont, skillName, { descTipX + 15, descTipY + 15 }, 20, 1, ORANGE);

			//описание спела(белое)
			DrawTextEx(profileFont, desc, { descTipX + 15, descTipY + 45 }, 17, 1, RAYWHITE);

			//ЕСЛИ есть кд то выводим его снизу
			DrawTextEx(profileFont, TextFormat("Cooldown: %.1fs", unit->skillCoolDown), 
				{ descTipX + 15, descTipY + descHeight - 25 }, 16, 1, SKYBLUE);
		}
	}
}




