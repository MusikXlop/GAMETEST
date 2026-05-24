#pragma once
#include "raylib.h"
#include <vector>
#include <string>
#include <map> //для удобного хранения пары ID - ТЕКСТУРА
#include "AllSound.h"

//просто знай шо эти класы есть
class Setka;
class Bench;
//чтоб  #include не перекидываться 


struct Animation
{
	Model model; // 3д сетка перса(геометрия)
	ModelAnimation* animations; //список всех анимаций (* адресс первого элемента)
	int animsCount; // колво анимаций

	float scaleHero; // размер перса


	//индексы анимаций 
	int idleIndex; // номер анимации
	int walkIndex; //ходьба
	int attackIndex; //атака
	int deathIndex; //смерть 
	int winIndex; //вин
	int spellIndex; //спелы
};

struct ShopSlot
{
	int UnitID; 
	int price;
	bool isSolid;
	Rectangle bounds; // область на экране для клика
	float animFrameCounter = 0; // Счетчик кадров для 3D анимации(в магазе)

};

class Shop
{
public:
	Shop(int& gold);
	~Shop();
	//ссылки на уровень и опыт   // логика клинов
	void Update(Vector2 mousePos, int& playerlvl, int& playerXP, int* xpTable, 
		int& playerGold, Bench& gameSlot, Setka& arena,
		Camera3D camera, Texture2D benchTexture, AllSound& sounds);


	void Draw(int playerGold, int playerlvl, int playerXP, int* xpTable, 
		Camera3D& camera3d, Bench& banchslot, Setka& arena);// магаз отрисовка(и камера) 

	//метод отрисовки кнопки и бара опыта
	void DrawXpBar(int xp, int lvl, int* table);

	void LoadHeroModel(int type, const char* path, const char* idleName,float scale);

	//возвращаем ссылку на камеру (магазина)
	Camera3D& GetHeroCamera() { return heroCamera; }

	//возвращаем ссылку на библиотеку моделей
	std::map<int, Animation>& getLibrary() { return library; }


	//возвращаем текстуры кольца
	Texture2D getSlotRing() { return collisionSlot; } //скамейка 


	//геттеры для текстур продаж 
	Texture2D getPlusMoneta() { return pluskMonete; }
	Texture2D getSellZone() { return sellZone; }
	Texture2D getDeletePers() { return deletePers; }
	Texture2D getBtnCoin() { return btnCoin; }


	void Reroll(int& playerGold, bool free = false);// смена героя
private:
	bool isOpen = false; 
	bool isLocked = false;

	const int Reroll_cost = 2;
	const float font_size_coin = 20.0f;
	const int MAX_SHOP_SLOTS = 3; // макс слот

	Font shopFont;
	Texture2D LoadAndTrack(const char* path);
	//текстуры кнопок
	Texture2D btnOpen, btnClose, btnReroll, btnLockOpen, btnLockClosed, 
		btnCoin, slotring,collisionSlot, fonMagaza, pluskMonete, sellZone, deletePers, btnXp;

	std::vector<Texture2D> allTextures;
	std::vector<ShopSlot> slots; 
	Rectangle rerollBtn, lockBtn, toggleBtn, xpBtnRect;

 

	std::map<int, Animation> library;
	/* std::map: короче это типо словаря он  выводит номер
	и мы получаем даныне модели */

	Camera3D heroCamera;//отдельная камера магаза(для героев)


};
