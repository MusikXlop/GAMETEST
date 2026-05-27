#pragma once
#include "raylib.h"
#include <map>
#include "Shop.h"
#include "Heroes.h"
#include "WaveEnemy.h"
#include "ItemManager.h"
#include "GamePhase.h"
#include "AllSound.h"

struct UnitSetka
{

	bool active = false; // занята ли клетка 
	int unitID = -1; //id модели из списка
	Model modelka; // ЛИЧНАЯ копия модели для этой клетки
	int team = 0; //команда 0 - наш 1 - не наш 
	float scaleHero = 1.0f;  //для енеми 


	bool isDead = false; // флаг смерти 

	//поз и анимация
	Vector2 gridPos = { -1.0f, -1.0f }; // логические координаты 0-7
	Vector3 worldPos; //позиция в 3д мире(для плавного перемещения между клетками)
	Vector3 spawnWorldPos; //поз дома куда вернется юнит после файта
	float rotation = 0.0f;// сброс в 0 поворот
	float animTime = 0.0f; // личный таймер анимации для каждого героя на поле
	
	

	//состояния   (игровая логика)
	//афк стояние,поиск противников, мув,атака. спелл вин помер 
	enum State { IDLE, SEARCH, MOVE, ATTACK, SPELL, WIN, DEAD };
	State currentState = IDLE; // текущее поведение юнита(афк стояние)
	UnitSetka* target = nullptr;// указатель на врага которого юнит хочет ударить 
	int lvl = 1; //текущий лвл
	int buyPrice = 0; //цена за которую купили(для продажи)


	Hero stats; //объект со статами (хп дамаг)


	//инфо панель
	std::string name; // name pers
	int baseDamage = 0; // base мой белый урон
	int bonusDamage = 0; //  + зеленый урон 
	int baseArmor = 0; // белая брона 
	int bonusArmor = 0; // + зеленая броня
	float attackSpeed = 1.0f; // скорость атаки 
	float bonusAS = 0.0f; // бонус к атак спиду


	//спелы
	bool hasSpell = false; // есть ли у героя спел 
	float skillCoolDown = 10.0f; // кд  спела в секах
	float skillTimer = 0.0f; // текущий прогрес перезарядки 


	//текстуры портрета и спела
	Texture2D portret; // портрет перса
	Texture2D spellicon; // спелл иконка 


	float attackTimer = 0.0f; //перезарядка атаки(атак спид)
	bool hasDealtDamage = false; // флаг нанесли ли мы урон за текущий взмах
	
	Item unitItems[1]; //слот для одного предмета



};

class Setka
{
public:
	Texture2D star1, star2, star3; // текстура звезд(лвлов)

	const float SIZE = 26.0f; //общ размер сетки 26
	const int COUNT = 8; // 8x8 фулл поле
	const float PADDING = 0.13f; //отступ чтоб не касалась краев клетки
	const float HIGHLIGHT_Y = 0.1f; // высота над полом 

	Setka();
	~Setka();

	void Draw();

	void DrawUnits(std::map<int, Animation>& library, Camera3D camera,Shop& shop);

	void DrawHightlight(Vector2 tile);// подсветка выделеной плиты


	bool HaveUnitOnPole(int id);



	template<typename T>
	void InitializeHeroStats(T& unit)//Теперь T может быть и UnitSetka и BenchSlot
	{
		//обнуление бонусов(иначе оно будет плюсоваться бесконечно)
		unit.bonusDamage = 0;
		unit.bonusArmor = 0;
		unit.bonusAS = 0;

		// Обязательно передаем ID юнита в его внутреннюю структуру статов
		unit.stats.id = unit.unitID;



		//присваиваем портрет из карты по его айди 
		//есть ли ваще картинка дл айди
		if (heroPortret.count(unit.unitID))
		{
			unit.portret = heroPortret[unit.unitID];
			//если картинка есть то копируем ссылку на нее в переменную юнита
		}


		// Присваиваем иконку спелла
		if (spellsIcons.count(unit.unitID))
		{
			unit.spellicon = spellsIcons[unit.unitID];
			unit.hasSpell = true; //теперь панель понимает что надо рисовать спел
		}
		else
		{
			unit.hasSpell = false;
			unit.skillCoolDown = 0.0f; // если у спелап нет кд то всегда 0
		}

		// 0-99 наши
		if (unit.unitID < 100)
		{
			//хп дамаг армор 
			switch (unit.unitID) 
			{
				case 0: // Dragon Knight
				{ 
					unit.name = "Dragon Knight";
					unit.skillCoolDown = 0.0f;
					if (unit.lvl == 1) unit.stats.StatsHero(unit.name, 700.0f, 70.0f, 10.0f, 1.3f); // HP, DMG, ARMOR, радиус аттаки
					else if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 1000.0f, 185.0f, 12.0f, 1.3f); // HP, DMG, ARMOR, радиус аттаки
				
					//все что выше 2го попадает сюда(и 3й и 10 если буду делать) это гарант что юнит всегда будем иметь статы
					else unit.stats.StatsHero(unit.name, 1300.0f, 255.0f, 13.0f, 1.3f); // HP, DMG, ARMOR, радиус аттаки
				
					//бонус к амротру 
					float passivkaDkArmor = (unit.lvl == 1) ? 5.0f : (unit.lvl == 2 ? 7.0f : 10.0f);
					unit.bonusArmor += passivkaDkArmor;

					break;
				}
				case 1: // Wraith King
				{
					unit.name = "Wraith King";
					unit.skillCoolDown = 7.0f;
					if (unit.lvl == 1)
					{
						unit.stats.StatsHero(unit.name, 650.0f, 70.0f, 5.0f, 1.3f);

					}
					else if (unit.lvl == 2)
					{
						unit.stats.StatsHero(unit.name, 1100.0f, 130.0f, 7.0f, 1.3f);

					}
					else
					{
						unit.stats.StatsHero(unit.name, 1350.0f, 190.0f, 8.0f, 1.3f);
					}
					break;
				}
				case 2: // Juggernaut
				{
					unit.name = "Juggernaut";
					unit.skillCoolDown = 4.0f;
					if (unit.lvl == 1)
					{
						unit.stats.StatsHero(unit.name, 600.0f, 65.0f, 6.0f, 1.3f);
					}

					else if (unit.lvl == 2)
					{
						unit.stats.StatsHero(unit.name, 900.0f, 240.0f, 9.0f, 1.3f);
					}

					else
					{
						unit.stats.StatsHero(unit.name, 1100.0f, 260.0f, 12.0f, 1.3f);
					}
					break;
				}
				
				case 3: // Drow Ranger
				{
					unit.name = "Drow Ranger";
					unit.skillCoolDown = 0.0f;
					if (unit.lvl == 1)
					{
						unit.stats.StatsHero(unit.name, 450.0f, 75.0f, 2.0f, 3.4f);
					}
					else if (unit.lvl == 2)
					{
						unit.stats.StatsHero(unit.name, 750.0f, 130.0f, 3.0f, 3.4f);
					}
					else
					{
						unit.stats.StatsHero(unit.name, 950.0f, 230.0f, 5.0f, 3.4f);
					}
					break;
				}

				case 4: // Axe
				{
					unit.name = "Axe";
					unit.skillCoolDown = 5.0f;
					if (unit.lvl == 1)
					{
						unit.stats.StatsHero(unit.name, 800.0f, 55.0f, 10.0f, 1.2f);
					}
					else if (unit.lvl == 2)
					{
						 unit.stats.StatsHero(unit.name, 1350.0f, 180.0f, 15.0f, 1.2f);
					}
					else 
					{
						unit.stats.StatsHero(unit.name, 1750.0f, 250.0f, 20.0f, 1.2f);

					}
						
					break;
				}
				case 50:
				{
					unit.name = "skeletGreenLVL1";
					unit.skillCoolDown = 0.0f;
					if (unit.lvl == 1) unit.stats.StatsHero(unit.name, 150.0f, 75.0f, 2.0f, 1.0f);
					else if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 200.0f, 100.0f, 3.0f, 1.0f);
					else  unit.stats.StatsHero(unit.name, 230.0f, 150.0f, 5.0f, 1.0f);
					break;
				}
					
				case 51:
				{
					unit.name = "skeletRedLVL2";
					unit.skillCoolDown = 0.0f;
					if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 220.0f, 130.0f, 3.0f, 1.0f);
					else   unit.stats.StatsHero(unit.name, 240.0f, 180.0f, 5.0f, 1.0f);
					break;
				}
					
				case 52:
				{
					unit.name = "skeletLVL3";
					unit.skillCoolDown = 0.0f;
					if (unit.lvl == 3) unit.stats.StatsHero(unit.name, 380.0f, 200.0f, 5.0f, 1.0f);
					break;
				}

			}
		}
		else // enemy
		{
			switch (unit.unitID)
			{
			case 100:
				unit.name = "Undying Zombie";
				unit.skillCoolDown = 0.0f;
				if (unit.lvl == 1)      unit.stats.StatsHero(unit.name, 250.0f, 40.0f, 1.0f, 1.0f);
				else if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 450.0f, 75.0f, 3.0f, 1.0f);
				else                    unit.stats.StatsHero(unit.name, 900.0f, 120.0f, 6.0f, 1.0f); 
				break;

			case 101:
				unit.name = "Dire Creep";
				unit.skillCoolDown = 0.0f;
				if (unit.lvl == 1)      unit.stats.StatsHero(unit.name, 280.0f, 50.0f, 3.0f, 1.2f);
				else if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 550.0f, 110.0f, 5.0f, 1.2f);
				else                    unit.stats.StatsHero(unit.name, 1050.0f, 210.0f, 8.0f, 1.2f);
				break;

			case 102:
				unit.name = "Ogre Frostmage";
				unit.skillCoolDown = 0.0f;
				if (unit.lvl == 1)      unit.stats.StatsHero(unit.name, 500.0f, 75.0f, 8.0f, 1.2f);
				else if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 750.0f, 140.0f, 10.0f, 1.2f);
				else                    unit.stats.StatsHero(unit.name, 1300.0f, 230.0f, 16.0f, 1.2f); 
				break;

			case 103:
				unit.name = "Radiant Creep";
				unit.skillCoolDown = 0.0f;
				if (unit.lvl == 1)      unit.stats.StatsHero(unit.name, 300.0f, 50.0f, 3.0f, 1.2f);
				else if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 580.0f, 115.0f, 5.0f, 1.2f);
				else                    unit.stats.StatsHero(unit.name, 1100.0f, 215.0f, 8.0f, 1.2f);
				break;

			case 104:
				unit.name = "Hellbear";
				unit.skillCoolDown = 0.0f;
				if (unit.lvl == 1)      unit.stats.StatsHero(unit.name, 400.0f, 100.0f, 3.0f, 1.3f);
				else if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 750.0f, 140.0f, 6.0f, 1.3f);
				else                    unit.stats.StatsHero(unit.name, 1450.0f, 220.0f, 11.0f, 1.3f);
				break;

			case 105:
				unit.name = "Ancient Golem";
				unit.skillCoolDown = 0.0f;
				if (unit.lvl == 1)      unit.stats.StatsHero(unit.name, 500.0f, 120.0f, 8.0f, 1.4f);
				else if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 950.0f, 180.0f, 12.0f, 1.4f);
				else                    unit.stats.StatsHero(unit.name, 2200.0f, 320.0f, 22.0f, 1.4f); 
				break;

			case 106:
				unit.name = "Radiant Creep";
				unit.skillCoolDown = 0.0f;
				if (unit.lvl == 1)      unit.stats.StatsHero(unit.name, 320.0f, 60.0f, 3.0f, 1.2f);
				else if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 600.0f, 125.0f, 5.0f, 1.2f);
				else                    unit.stats.StatsHero(unit.name, 1150.0f, 220.0f, 9.0f, 1.2f);
				break;

			case 107:
				unit.name = "Hellbear Smasher";
				unit.skillCoolDown = 0.0f;
				if (unit.lvl == 1)      unit.stats.StatsHero(unit.name, 500.0f, 120.0f, 2.0f, 1.4f);
				else if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 850.0f, 160.0f, 5.0f, 1.4f);
				else                    unit.stats.StatsHero(unit.name, 1700.0f, 320.0f, 10.0f, 1.4f); 
				break;

			case 108:
				unit.name = "Tusk Ghost";
				unit.skillCoolDown = 0.0f;
				if (unit.lvl == 1)      unit.stats.StatsHero(unit.name, 700.0f, 90.0f, 5.0f, 1.2f);
				else if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 950.0f, 160.0f, 8.0f, 1.2f);
				else                    unit.stats.StatsHero(unit.name, 1850.0f, 270.0f, 13.0f, 1.2f); 
				break;

			case 109:
				unit.name = "Tusk Skeleton";
				unit.skillCoolDown = 0.0f;
				if (unit.lvl == 1)      unit.stats.StatsHero(unit.name, 600.0f, 80.0f, 4.0f, 1.4f);
				else if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 1050.0f, 150.0f, 7.0f, 1.4f);
				else                    unit.stats.StatsHero(unit.name, 1600.0f, 260.0f, 11.0f, 1.4f);
				break;

			case 110:
				unit.name = "Ancient Dire";
				unit.skillCoolDown = 0.0f;
				if (unit.lvl == 1)      unit.stats.StatsHero(unit.name, 800.0f, 210.0f, 7.0f, 1.4f);
				else if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 1350.0f, 280.0f, 12.0f, 1.4f);
				else                    unit.stats.StatsHero(unit.name, 2600.0f, 360.0f, 25.0f, 1.4f); 
				break;

			case 111:
				unit.name = "Ancient Radiant";
				unit.skillCoolDown = 0.0f;
				if (unit.lvl == 1)      unit.stats.StatsHero(unit.name, 800.0f, 200.0f, 9.0f, 1.4f);
				else if (unit.lvl == 2) unit.stats.StatsHero(unit.name, 1300.0f, 270.0f, 16.0f, 1.4f);
				else                    unit.stats.StatsHero(unit.name, 2500.0f, 350.0f, 35.0f, 1.4f); 
				break;
			}
		}
		//чтоб инфо панель не показывала 0(для интерфейса)
		unit.baseDamage = (int)unit.stats.damage;
		unit.baseArmor = unit.stats.armor;


		//  Сброс таймеров (чтобы не было мгновенного ульта при спавне)
		unit.attackTimer = 0.0f;
		unit.skillTimer = 0.0f;
		unit.hasDealtDamage = false;
		unit.animTime = 0.0f; // чтоб анимация сначала начиналась 
	}


	//массив поля
	UnitSetka PoleHero[8][8]; 	//массив поля

	//передаем луч мыши получаем координаты (0-7)
	Vector2 GetPlitkaTile(Ray ray, GamePhase phase);

	float GetTileSIze() const { return tSize; }//возвращаем размер клетки

	UnitSetka ExtractUnit(int x, int y);
	void InsertUnit(int x, int y, UnitSetka unit);

	UnitSetka* FindNearestEnemy(int attackerX, int attackerY, int attackerTeam);
	
	void UpdateBattle(int currentRound, ItemManager& items, std::map<int, Animation>& library, AllSound& sounds);
	void DrawHpHero(Camera3D camera);

	void SaveStartPos();

	void RespawnUnitsAfterBattle();

	bool checkEndBattle();

	void SpawmEnemyWave(int round, Shop& shop);

	void LoadUnitsProfile();

	Vector3 GetTileWorldPos(int x, int y);


	void SpawnSummons(int id, Vector2 tile, int team, int level, std::map<int, Animation>& library);

	Vector2 FindEmptyTileNear(Vector2 pos); //пустые клетки ищем 
private:
	Model model;
	float tSize; // размер одной клетки (26/8  =3.25)


	//карта для хранения портретов персов  (ключ это unitId) знач  это текстура 
	std::map<int, Texture2D> heroPortret;
	std::map<int, Texture2D> spellsIcons; // спелы
};