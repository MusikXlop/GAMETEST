#pragma once
#include "raylib.h"
#include <map> //для удобного хранения пары ID - ТЕКСТУРА
#include "Shop.h"
#include <vector>
#include "Setka.h"
#include "AllSound.h"


//struct BenchSlot
//{
//
//	int unitID = -1; // -1 = пусто ( id перса эт)
//	
//	bool active = false;
//	int animFrameCounter = 0; // Счетчик кадров для 3D анимации(в слоте)
//	int lvl = 1; //lvl 1 2 3 
//	int buyPrice = 0; // цена покупки
//	float animTime = 0.0f;
//
//
//	Hero stats; 
//};


class Bench
{

	friend class ItemManager; // Разрешаем контроллеру доступ к slots
	friend class HeroController; // Разрешаем контроллеру доступ к slots
public:
	Bench();
	~Bench();

	// загрузка
	void Load();

	void Clear();

	void Update(std::map<int, Animation>& library);// Передаем библиотеку для доступа к анимациям

	void Draw(Texture2D collisionSlot, Camera3D camera3d, std::map<int, Animation>& library,
		int& playerGold, Texture2D pluskMonete, Texture2D fonDelete, Texture2D deletePers, Texture2D btnCoin,
		Setka& arena, Vector2 currentTile, int dragPrice = 0);


	//возвращает прямоугольный хитбокс слота
	Rectangle GetSlotHitbox(int index); // хитбокс слота по индексу


	bool AddHero(int unitID, int price, Setka& arena, Camera3D camera, Texture2D tex);// для магаза


	bool AddHero(UnitSetka SaveAllHero, Camera3D camera, Texture2D tex);// для перемещений


	bool HaveUnit(int UnitID); // проверка на наличие юнита(подсветка)

	void checkMerge(Setka& arena, AllSound& sounds); // lvlup ( с ссылкой на арену)

	int GetSellPrice(int index);// продажа персов 

	static const int MAX_BENCH_SLOTS = 8; // колво статов
	//static  принадлежит классу можно использовать как (пример мой slots[MAX_BENCH_SLOTS])

	/* для отрисовки рамки 
	SetSelectedSlot(int id) устанавливаем какой щас в фокусе(какой я взял) 
	если айди id = -1 то снимаю выделение
	это над для рамки и для панели продаж*/
	void SetSelectedSlot(int id) { selectedId = id; isSelected = (id != -1); }
	//спрашиваем ламку кто у нас ща выделен (юзаем для контроллера чтоб понять продавать чи тащить перса)
	int GetSelectedId() { return selectedId; }
	//вкл/выкл режим невидимки для юнита в слота (когда я тащу перса функция говорит лаке перестать рисовать
	//героя в этом слоте он ща летит за мышкой
	void SetDragging(bool dragging) { isDragging = dragging; }

	//говорит занят ли слот или нет
	//(эт над для избегания ошибок если я схвачу пустоту(пофишено) ) 
	bool IsSlotActive(int index) {
		if (index >= 0 && index < 8) {
			return slots[index].active;
		}
		return false;
	}
	//хитбокс продажи 
	Rectangle GetSellZone() { return sellZone; }


	//метод удаления юнита(вызываем при продаже)
	void RemoveHero(int index)
	{
		if (index >= 0 && index < MAX_BENCH_SLOTS)
		{
			slots[index].active = false;
			slots[index].unitID = -1;
			slots[index].buyPrice = 0;
		}
	}

	int GetClickedSlot(Vector2 mousePos); // вернет индекс слота 0-7 или -1
	UnitSetka TakeUnit(int index, std::map<int, Animation>& library); // забирает юнита из слота


	void UpdateUnitWorldPos(int index, Camera3D camera, Texture2D collisionSlot)
	{
		if (index < 0 || index >= MAX_BENCH_SLOTS) return;

		//(collisionSlot.width * 1.5f * 0.5f);  половина ширины текстуры с учетом 1.5 размера
		float centerX = startX + (index * spacing) + (collisionSlot.width * 1.5f * 0.5f);
		float centerY = startY + (collisionSlot.height * 1.5f * 0.5f);

		//луч из камеры через точку на экране
		Ray ray = GetScreenToWorldRay({ centerX,centerY }, camera);


		//дистанция полоски пола (t)растояние от камеры
		float distance = (-1.3f - ray.position.y) / ray.direction.y;

		slots[index].worldPos.x = ray.position.x + ray.direction.x * distance; //x
		slots[index].worldPos.y = ray.position.z + ray.direction.z * distance; //z

	}


	//для того чтоб зырить на юнитов в слотах
	UnitSetka& GetUnit(int index) { return slots[index]; }



private:
	UnitSetka slots[MAX_BENCH_SLOTS]; // 8 мест потом поменяю мб
	float startX = 25.0f;//100 
	float startY = 1000.0f; //поз внизу 850
	float spacing = 235.0f;



	Texture2D star1;
	Texture2D star2;
	Texture2D star3;


	int selectedId = -1; // индекс перса(-1 если никто не выбрал)
	bool isDragging = false; //флаг (тащим ли мы перса) тру = да
	bool isSelected = false; //первый клик сделан = подсвечиваем

	//значение прямоугольника x50 y50 отступ  100 350 высота 
	Rectangle sellZone = { 50,300,100,350 };

};