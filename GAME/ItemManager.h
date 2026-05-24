#pragma once
#include "raylib.h"
#include <vector>
#include <string>
#include "AllSound.h"

class Setka;
class Bench;
enum ItemType {
	EMPTY,
	CRYSTALIS, DESOLATOR, CLAYMORE, POARCHING,DAEDALUS, // damage
	CHAINMAIL, CUIRASS, CRAGGY, VANGUARD,        // armore
	HEART, VITALITY, WITLESS, BRUTEHAD,            // hp
	GLOVES, MOONSHARD, BUTTERFLY     // attack speed 

};

struct Item
{
	ItemType type = EMPTY;
	Texture2D icon;
	std::string name;
	float bonusValue = 0.0f; // хп армат урон 

};

class ItemManager
{
public:
	ItemManager();
	~ItemManager();


	void LoadItems(); 

	void DrawInventory(Vector2 startPos);
	// добавляем камеру в аргументы  чтоб GetWorldToScreen работал верно
	void UpdateMove(std::vector<struct UnitSetka*>& allUnits, Camera3D camera, AllSound& sounds);


	void AddItem(ItemType type); // Вызывается при дропе лута


	void DrawDropZones(std::vector<struct UnitSetka*>& allUnits, Camera3D camera);


	Rectangle RaschetItemsForUnit(UnitSetka* unit, Camera3D camera);

	

	void ItemsDrawInHero(class Setka& arena, class Bench& bench, struct UnitSetka* handUnit, bool isDraggingHero, Camera3D camera);


	void ApplyItemBuffs(UnitSetka* unit, Item& it); // применяем бафы (когда свап) 

	void RemoveItemBuffs(UnitSetka* unit, Item& it); //  снимаем старую шмотки убираем бафы(когда свап) 


	
	Item CreateItem(ItemType type);
private:
	std::vector<Item> inventory;
	int maxSlots = 8; //макс шмоток в инвентре

	// для drag and drop 
	int draggedItemIndex = -1; //индекст если тянем из инвентаря
	bool isDragging = false; //процесс перетаскивания

	bool isDraggingFromUnit = false; // тяним ли мы сейчас с героя ш мотку 
	UnitSetka* sourceUnit = nullptr; // с какого именно героя потянули 
	Item tempItem; // временное хранилищи данных предмета


	Texture2D emptyLoad; //empty_psd

	//текстуры  (дамаг)
	Texture2D texCrystalis;
	Texture2D texDesolator;
	Texture2D texClainmore;
	Texture2D texPoarching;
	Texture2D texDaedalus;
	
	//броня
	Texture2D texChainmail;
	Texture2D texCuirass;
	Texture2D texCraggyCoat;
	Texture2D texVanguard;

	//хп 
	Texture2D texHeart;
	Texture2D texVitalityBooster;
	Texture2D texWitless;
	Texture2D texbruteHat;

	//атак спид
	Texture2D texGlove;
	Texture2D texMoonShard;
	Texture2D texButterfly;



};