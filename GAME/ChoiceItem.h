#pragma once
#include "raylib.h"
#include "ItemManager.h"
#include <vector>
#include "AllSound.h"
//хранение текстового описания предмета 
struct ChoiceBuff
{
	std::string name;
	std::string atribute; 
};


class ChoiceItem
{
public:
	ChoiceItem(); // тут будут текстуры 
	~ChoiceItem(); // выгрузка 

	// для генерации случаных 2 айтема из пула шмоток
	void GenerateItemsChoice(ItemManager& itm);

	//проверкает клики мыши по картинкам выбора 
	bool Update(Vector2 mousePos, ItemManager& itm, AllSound& sounds);

	//рисуем на весь экран (выбор иконки шмоток и таймер)
	void Draw(Texture2D viborLeft, Texture2D viborRight, float timer);

	//если не успел выбрать за время то автоматов выбирается 
	void AutoPickItems(ItemManager& itm);

private:
	Item choiceItem[2]; // 2 шмотки на выбор
	Rectangle leftRect;  // хитбокс левой 
	Rectangle rightRect;// хитбокс правой 

	// чтоб на одну картинку нарисовать
	void DrawChoiceItemsUI(Item& item, Rectangle cardRect);

		
};