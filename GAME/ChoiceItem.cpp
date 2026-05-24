#include "ChoiceItem.h"


//текст бафа в зависимости от предмета 
ChoiceBuff GetChoiceData(ItemType type)
{
	switch (type)
	{
		// damage
		case CRYSTALIS: return{ "Crystalis","+30 DMG" };
		case DESOLATOR: return{ "Desolator","+50 DMG" };
		case CLAYMORE: return{ "Claymore","+15 DMG" };
		case POARCHING: return{ "Porching Knife","+25 DMG" };
		case DAEDALUS: return{ "Daedalus","+88 DMG" };

					  //armor
		case CHAINMAIL: return{ "Chainmail","+3 ARMOR" };
		case CUIRASS: return{ "Assult Cuirass","+15 ARMOR" };
		case CRAGGY: return{ "Cragge Coat","+10 ARMOR" };
		case VANGUARD: return{ "Vanguard","+5 ARMOR" };

					  //hp
		case HEART: return{ "Heart of Tarrasque","+500 HP" };
		case VITALITY: return{ "Vitality Booster","+250 HP" };
		case WITLESS: return{ "Witless Shako","+350 HP" };
		case BRUTEHAD: return{ "Brute Had","+150 HP" };

					  //attack speed  (0.2 = 20%)
		case GLOVES: return{ "Gloves of Haste","+20 Attack Speed" };
		case MOONSHARD: return{ "Moon shard","+70 Attack Speed" };
		case BUTTERFLY: return{ "Butterfly","+50 Attack Speed" };
		default:        return { "Unknown Item", "????" };
	}
}

ChoiceItem::ChoiceItem()
{
}

ChoiceItem::~ChoiceItem()
{
}

void ChoiceItem::GenerateItemsChoice(ItemManager& itm)
{
	//пул предетов которые мы разрешаем давыать за раунд
	std::vector<ItemType> pool =
	{
		CRYSTALIS,DESOLATOR,CLAYMORE,POARCHING,DAEDALUS,

		CHAINMAIL,CUIRASS,CRAGGY,VANGUARD,

		HEART,VITALITY,WITLESS,BRUTEHAD,

		GLOVES,MOONSHARD,BUTTERFLY

	};

	// выбираем случайный индекст для первого предмета 
	int firstItem = GetRandomValue(0, pool.size() - 1);
	choiceItem[0] = itm.CreateItem(pool[firstItem]); //создаем предмет через ItemManager/h cpp 

	//выбираем второй индекст чтоб он не совпадал с первым 
	int secondItem = GetRandomValue(0, pool.size() - 1);
	while (firstItem == secondItem) secondItem = GetRandomValue(0, pool.size() - 1);

	choiceItem[1] = itm.CreateItem(pool[secondItem]); // создаем вторую шмотку

}

bool ChoiceItem::Update(Vector2 mousePos, ItemManager& itm, AllSound& sounds)
{
	//если нажали лкм 
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		// попал ли курсор на левую карточку
		if (CheckCollisionPointRec(mousePos, leftRect))
		{
			itm.AddItem(choiceItem[0].type); // добавляем айтем в инвентарь
			sounds.PlayEquipItem();
			return true;
		}

		// попал ли курсор на правую карточку
		if (CheckCollisionPointRec(mousePos, rightRect))
		{
			itm.AddItem(choiceItem[1].type); // добавляем айтем в инвентарь
			sounds.PlayEquipItem();
			return true;
		}
	}
	return false; //никто не кликнул продолжаем жджать
}

void ChoiceItem::Draw(Texture2D viborLeft, Texture2D viborRight, float timer)
{
	float widthDA = (float)GetScreenWidth();
	float heightDA = (float)GetScreenHeight();

	// определяем размер и позицию карточек(чуть левее и правее центра)
	leftRect = { widthDA / 2 - 420, heightDA / 2 - 250, 400,500 };
	rightRect = { widthDA / 2 + 20, heightDA / 2 - 250, 400,500 };


	//рисуем фоны 
	DrawTexturePro(viborLeft, { 0,0,(float)viborLeft.width,(float)viborLeft.height }, leftRect, { 0,0 }, 0, WHITE);
	DrawTexturePro(viborRight, { 0,0,(float)viborRight.width,(float)viborRight.height }, rightRect, { 0,0 }, 0, WHITE);


	//содержимое выборов этих
	DrawChoiceItemsUI(choiceItem[0], leftRect);
	DrawChoiceItemsUI(choiceItem[1], rightRect);

	//рисуем таймер выбора сверху по центру
	DrawText(TextFormat("TIME TO CHOOSE: %02.0f", timer), widthDA / 2 - 150, heightDA / 2 - 290, 30, GOLD);
}


void ChoiceItem::DrawChoiceItemsUI(Item& item, Rectangle cardRect)
{
	ChoiceBuff info = GetChoiceData(item.type);

	//рисуем иконку)центрируем ее)
	float iconScale = 0.45f;
	float iconX = cardRect.x + (cardRect.width / 2) - (60); // 60 +- половина ширины иконки 


	//рисуем иконки предмета (центрируем ее внутри карточки)
	DrawTextureEx(item.icon, { iconX, cardRect.y + 80 }, 0, iconScale, WHITE);


	//центр текста 
	int nameFontSize = 30;
	// считаем ширину текста названия в пикселях
	int nameWidth = MeasureText(info.name.c_str(), nameFontSize);

	//вычисляем X (центр карточки) - ( половина ширины текста)
	float nameX = cardRect.x + (cardRect.width / 2) - (nameWidth / 2);

	//рисуем текст из структур ChoiceBuff
	DrawText(info.name.c_str(), (int)nameX, (int)cardRect.y + 240, nameFontSize, GOLD); // название 

	//статы
	int statsFontSize = 25; 
	int statsWidth = MeasureText(info.atribute.c_str(), statsFontSize);

	//вычисляем X (центр карточки) - ( половина ширины текста)
	float statsX = cardRect.x + (cardRect.width / 2) - (statsWidth / 2);


	DrawText(info.atribute.c_str(), (int)statsX, (int)cardRect.y + 290, statsFontSize, GREEN); // статы

}

void ChoiceItem::AutoPickItems(ItemManager& itm)
{
	//если игрок проафекашил и ничо не выбрал то просто отдаем ему первый предмет
	itm.AddItem(choiceItem[0].type);
}

