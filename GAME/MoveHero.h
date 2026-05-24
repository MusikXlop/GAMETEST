#pragma once
#include "raylib.h"
#include "Setka.h"
#include "Slot.h"
#include "AllSound.h"

class HeroController
{
public:
	HeroController();
	
	//главный метод который будет вызываться в мейне каждый кадр
	void Update(Setka& arena, Bench& bench, Ray ray, std::map<int, Animation>& library, int& playerGold, 
		int playerlevel, int currentPhase, Camera3D camera, 
		Texture2D collisionSlotTexture, ItemManager& items, AllSound& sounds);


	//метод для отрисовки героя в руке(в мышке) чтоб он летат за мышкой
	void DrawInHand(Ray ray, std::map<int, Animation>& library);

	Vector2 selectedTile; //координаты выбранной клетки на поле (для рамки)
	bool isTileSelected; // флаг выбрано ли что-то на поле
	bool isDragging; //состояние держим мы ща героя в руке

	std::vector<struct UnitSetka*> getAllUnits();

	//чтение ток 
	//для того чтоб знать держим ли мы его мышкой в воздухе если true то принудительно сбрасываем
	bool IsDragging()const { return isDragging; } 
	//сообщает откуда был взят юнит (если поле то возвращаем на старую клетку,бенч то на свободный бенч)
	bool IsFromField() const { return fromField; }
	//возвращает коордиаты X Y клетки на поле с которой игрок поднял юнита(чтоб знать адрес возврата)
	Vector2 GetStartTile() const { return startTile; }
	//возвращает сам объект UnitSetka со всеми характеристиками
	UnitSetka GetHandUnit() const {return handUnit;} //возвращает копию 


	//метод для принудительного сброса состояния 
	void SbrossDragging() { isDragging = false; }


	void ResetSelection()
	{
		isTileSelected = false;
		selectedTile = { -1, -1 };
		
	}

	UnitSetka* GetSelectedUnit(Setka& arena, Bench& bench); // возвращаем адрес юнита стоящего на этой плитке


	//возвращает адресс
	UnitSetka* GetHandUnitPtr(){ return &handUnit; } //указатель на героя котортого мы несем 
private:

	bool fromField;  //откуда взяли( true с поля false со скамьи 
	int startSlotIndex; //если со скаьи то идекс слота 
	Vector2 startTile; //если с поля несем то координаты берем x y 
	//для возврата юнита на место (одну оставили вторую дубликат удалили)


	UnitSetka handUnit; //герой которого мы несем
	
	// указатели дл того чтоб HeroController помнл с какими объектами он работал в ласт раз
	Setka* lastArena = nullptr; 
	Bench* lastBench = nullptr;
};