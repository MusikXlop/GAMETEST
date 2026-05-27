#include "ItemManager.h"
#include "Setka.h"
#include "raymath.h"
#include "Slot.h"


ItemManager::ItemManager()
{
	inventory.assign(maxSlots, { EMPTY });
	draggedItemIndex = -1;
	isDragging = false;



	//текстуры  (дамаг)
	texCrystalis = { 0 };
	texDesolator = { 0 };
	texClainmore = { 0 }; 
	texPoarching = { 0 };
	texDaedalus = { 0 };

	//броня
	texChainmail = { 0 };
	texCuirass = { 0 };
	texCraggyCoat = { 0 };
	texVanguard = { 0 };

	//хп 
	texHeart = { 0 };
	texVitalityBooster = { 0 };
	texWitless = { 0 };
	texbruteHat = { 0 };

	//атак спид
	texGlove = { 0 };
	texMoonShard = { 0 };
	texButterfly = { 0 };


}

ItemManager::~ItemManager()
{

	UnloadTexture(emptyLoad);

	//текстуры  (дамаг)
	UnloadTexture(texCrystalis);
	UnloadTexture(texDesolator);
	UnloadTexture(texClainmore);
	UnloadTexture(texPoarching);
	UnloadTexture(texDaedalus);

	//броня
	UnloadTexture(texChainmail);
	UnloadTexture(texCuirass);
	UnloadTexture(texCraggyCoat);
	UnloadTexture(texVanguard);

	//хп 
	UnloadTexture(texHeart);
	UnloadTexture(texVitalityBooster);
	UnloadTexture(texWitless);
	UnloadTexture(texbruteHat);

	//атак спид
	UnloadTexture(texGlove);
	UnloadTexture(texMoonShard);
	UnloadTexture(texButterfly);



	// выгрузка всех иконок(предметов в инвентаре)
	for (auto& item : inventory)
	{
		if (item.icon.id != 0)
		{

		}
	}
}

void ItemManager::LoadItems()
{
	emptyLoad = LoadTexture("assets/empty_psd.png");

	//текстуры  (дамаг)
	texCrystalis = LoadTexture("items/crystalis_psd.png");
	texDesolator = LoadTexture("items/desolator_psd.png");
	texClainmore = LoadTexture("items/claymore_psd.png");
	texPoarching = LoadTexture("items/poaching_knife_psd.png");
	texDaedalus = LoadTexture("items/daedalus_psd.png"); 

	//броня
	texChainmail = LoadTexture("items/chainmail_psd.png");
	texCuirass = LoadTexture("items/assault_cuirass_psd.png");
	texCraggyCoat = LoadTexture("items/craggy_coat_psd.png");
	texVanguard = LoadTexture("items/vanguard_psd.png");

	//хп 
	texHeart = LoadTexture("items/heart_psd.png");
	texVitalityBooster = LoadTexture("items/vitality_booster_psd.png");
	texWitless = LoadTexture("items/witless_shako_psd.png"); 
	texbruteHat = LoadTexture("items/brute_hat_psd.png"); 

	//атак спид
	texGlove = LoadTexture("items/gloves_of_haste_psd.png"); 
	texMoonShard = LoadTexture("items/moon_shard_psd.png"); 
	texButterfly = LoadTexture("items/butterfly_psd.png");
}



Item ItemManager::CreateItem(ItemType type)
{
	Item it;
	it.type = type;
	switch (type)
	{
		// damage
	case CRYSTALIS: it.icon = texCrystalis; it.name = "Crystalis"; it.bonusValue = 30.0f; break;
	case DESOLATOR: it.icon = texDesolator; it.name = "Desolator"; it.bonusValue = 50.0f; break;
	case CLAYMORE: it.icon = texClainmore; it.name = "Claymore"; it.bonusValue = 15.0f; break;
	case POARCHING: it.icon = texPoarching; it.name = "Porching Knife"; it.bonusValue = 25.0f; break;
	case DAEDALUS: it.icon = texDaedalus; it.name = "Daedalus"; it.bonusValue = 88.0f; break;
	
		//armor
	case CHAINMAIL: it.icon = texChainmail; it.name = "Chainmail"; it.bonusValue = 3.0f; break;
	case CUIRASS: it.icon = texCuirass; it.name = "Assult Cuirass"; it.bonusValue = 15.0f; break;
	case CRAGGY: it.icon = texCraggyCoat; it.name = "Cragge Coat"; it.bonusValue = 10.0f; break;
	case VANGUARD: it.icon = texVanguard; it.name = "Vanguard"; it.bonusValue = 5.0f; break;
		//hp
	case HEART: it.icon = texHeart; it.name = "Heart of Tarrasque"; it.bonusValue = 500.0f; break;
	case VITALITY: it.icon = texVitalityBooster; it.name = "Vitality Booster"; it.bonusValue = 250.0f; break;
	case WITLESS: it.icon = texWitless; it.name = "Witless Shako"; it.bonusValue = 350.0f; break;
	case BRUTEHAD: it.icon = texbruteHat; it.name = "Brute Had"; it.bonusValue = 150.0f; break;
		//attack speed  (0.2 = 20%)
	case GLOVES: it.icon = texGlove; it.name = "Gloves of Haste"; it.bonusValue = 0.2f; break;
	case MOONSHARD: it.icon = texMoonShard; it.name = "Moon shard "; it.bonusValue = 0.7f; break;
	case BUTTERFLY: it.icon = texButterfly; it.name = "Butterfly"; it.bonusValue = 0.5f; break;
	}
	return it;
}




void ItemManager::AddItem(ItemType type)
{
	for (int i = 0; i < maxSlots; i++) // идем по 8 слотам 
	{
		if (inventory[i].type == EMPTY) //ищем первый пустой слот 
		{
			inventory[i] = CreateItem(type); //создаем предмет через свич(который выше) и кладем в слот
			break; // нашли место = выход
		}
	}
}




Rectangle ItemManager::RaschetItemsForUnit(UnitSetka* unit, Camera3D camera)
{
	float visotaY = 1.8f; //ВЫСОТА 

	/* берем позицию ног героя unit->worldPos
			и сощдаем новую 3д точку targetPos поднимая ее на 2.8 мемтра вверх*/
	Vector3 targetPos3D = {
		unit->worldPos.x, // x
		visotaY,   // y высота 
		unit->worldPos.y  //z глубина
	};

	//превращаем 3д точку в 2д пиксели
	Vector2 screenPos = GetWorldToScreen(targetPos3D, camera);


	//размер иконки 
	float size = 30.0f;

	float smeshenie = 15.0f; // дополнительное смещение

	//отрисовка (центрирование)
	//смещаем влево на половину ширину чтоб было по центру
	//возвращаем финальный Rectangle ссмещением + 7
	return {
		screenPos.x - (size / 2.0f),
		screenPos.y - (size / 2.0f) + smeshenie, // дополнительное смещение
		size,size };
}


void ItemManager::ItemsDrawInHero(Setka& arena, Bench& bench, UnitSetka* handUnit, bool isDraggingHero, Camera3D camera)
{
	//создаем временный список отрисовки внутри метода
	std::vector<UnitSetka*> allUnits;

	//собираем всех с поля 
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
			if (arena.PoleHero[x][y].active) allUnits.push_back(&arena.PoleHero[x][y]);
	//кидаем все адреса живых персов в allUnits 

	for (int i = 0; i < 8; i++)
	{
		if (bench.slots[i].active) allUnits.push_back(&bench.slots[i]);
	} 	//кидаем все адреса живых персов в allUnits 

	//рисувем шмот для тех кто стоит на местах
	for (auto* unit : allUnits)
	{
		// рисуем только если юнит активен и в нашей типо и у него есть предмет
		//и если это не тот юнит  с которого мы тянем предмет
		if (unit->active && unit->unitItems[0].type != EMPTY)
		{
			if (isDragging && isDraggingFromUnit && unit == sourceUnit) continue; // пропускаеем отрисовку в мышке (в руках)

			Rectangle itemRect = RaschetItemsForUnit(unit, camera);

			Rectangle sourceRec{ 0,0, (float)unit->unitItems[0].icon.width, (float)unit->unitItems[0].icon.height };
			DrawTexturePro(unit->unitItems[0].icon, sourceRec, itemRect, { 0,0 }, 0, WHITE);

		}
	}
	//рисуем шмотку на герое которого тащу 
	if (isDraggingHero && handUnit != nullptr && handUnit->unitItems[0].type != EMPTY)
	{

		Rectangle itemRect = RaschetItemsForUnit(handUnit, camera);

		Rectangle sourceRec{ 0,0, (float)handUnit->unitItems[0].icon.width, (float)handUnit->unitItems[0].icon.height };
		DrawTexturePro(handUnit->unitItems[0].icon, sourceRec, itemRect, { 0,0 }, 0, WHITE);
	}
}



void ItemManager::DrawDropZones(std::vector<struct UnitSetka*>& allUnits, Camera3D camera)
{
	//если мы ничего не тащим то зоны рисовать не надо 
	if (!isDragging) return;


	for (auto* unit : allUnits)
	{
		// рисуем зоны только для тиммейтов(наше поле)
		if (unit->active && unit->team == 0)
		{
			
			//получаем координнаты через общ функцию 
			Rectangle destRec = RaschetItemsForUnit(unit, camera);

			//что рисуем(исходый прямоугольник берем всю текстуру)
			Rectangle sourceRec = { 0, 0, (float)emptyLoad.width, (float)emptyLoad.height };
			// рисуем 
			// origin{ 0,0 } значит что точка отсчета верхний левый угол destRec
			DrawTexturePro(emptyLoad, sourceRec, destRec, { 0,0 }, 0, WHITE);
		}
	}
}




void ItemManager::DrawInventory(Vector2 startPos) //интерфейс
{
	DrawText("Items:", startPos.x, startPos.y - 35, 25, GOLD); // заголовок

	for (int i = 0; i < maxSlots; i++)
	{
		float x = startPos.x + (i % 2) * 85; // вычисляем Х (сетка в 2 колонки)
		float y = startPos.y + (i / 2) * 85;// Вычисляем Y

		DrawCircleV({ x + 35, y + 35 }, 35, BLACK); // center float ( черная обводка)
		DrawCircleV({ x + 35, y + 35 }, 32, DARKGRAY);// серый фон слота

		//если слот не пустой и мы его ща не тащим мышкой (рисуем предмет в слоте если ща шмотка не перетаскивается)
		if (inventory[i].type != EMPTY && (!isDragging || isDraggingFromUnit ||draggedItemIndex != i))
		{
			DrawTextureEx(inventory[i].icon, { x,y }, 0.0f, 0.27f, WHITE);//рисувем иконку
			//0.27 scale
		}
	}

	//прилипание к мышке
	if (isDragging)
	{
		Vector2 mousePose = GetMousePosition();

		//если тащим шмот то рисуем иконку прям под курсором 
		if (isDraggingFromUnit)
		{
			// если тянем  из героя шмот куда-то то рисуем  tempItem ( -35 половина размера)       
			DrawTextureEx(tempItem.icon, { mousePose.x - 35, mousePose.y - 35 }, 0.0f, 0.27f, WHITE);
		}
		else if (draggedItemIndex != -1)
		{
			//если шмот из инвентаря куда-то то рисуем предмет из инвентаря 
			DrawTextureEx(inventory[draggedItemIndex].icon, { mousePose.x - 35,mousePose.y - 35 }, 0.0f, 0.27f, WHITE);
		}
	}


}




void ItemManager::ApplyItemBuffs(UnitSetka* unit, Item& it)
{
	if (it.type == CRYSTALIS || it.type == DESOLATOR || it.type == CLAYMORE
		|| it.type == POARCHING || it.type == DAEDALUS)
	{
		unit->bonusDamage += (int)it.bonusValue; // для зеленого текста
		unit->stats.damage += (int)it.bonusValue; // для игры 
	}


	else if (it.type == CHAINMAIL || it.type == CUIRASS || it.type == CRAGGY ||
		it.type == VANGUARD)
	{
		unit->bonusArmor += (int)it.bonusValue; // для зеленого текста 
		unit->stats.armor += (int)it.bonusValue;
		TraceLog(LOG_INFO, "BUFF APPLIED: Unit %s now has bonusArmor: %d", unit->name.c_str(), unit->bonusArmor);
	}
	else if (it.type == HEART || it.type == VITALITY || it.type == WITLESS ||
		it.type == BRUTEHAD)
	{
		unit->stats.maxhp += (int)it.bonusValue; // увеличиваем макс хп
		unit->stats.hp += (int)it.bonusValue; // плюсуем HP
	}
	else if (it.type == GLOVES || it.type == MOONSHARD || it.type == BUTTERFLY)
	{
		unit->bonusAS += it.bonusValue; // плюсуем армор
		TraceLog(LOG_INFO, "BUFF APPLIED: Unit %s now has bonusAS: %.2f", unit->name.c_str(), unit->bonusAS);
	}

}

void ItemManager::RemoveItemBuffs(UnitSetka* unit, Item& it)
{
	if (it.type == CRYSTALIS || it.type == DESOLATOR || it.type == CLAYMORE
		|| it.type == POARCHING || it.type == DAEDALUS)
	{
		unit->bonusDamage -= (int)it.bonusValue; // плюсуем урон ( для панели)
		unit->stats.damage -= (int)it.bonusValue; // для файте 
		TraceLog(LOG_INFO, "REMOVE DMG: %d", unit->bonusDamage);


	}
	else if (it.type == CHAINMAIL || it.type == CUIRASS || it.type == CRAGGY ||
		it.type == VANGUARD)
	{
		unit->bonusArmor -= (int)it.bonusValue; // плюсуем армор
		unit->stats.armor -= (int)it.bonusValue;
		TraceLog(LOG_INFO, "BUFF REMOVED: Unit %s now has bonusArmor: %d", unit->name.c_str(), unit->bonusArmor);
	}
	else if (it.type == HEART || it.type == VITALITY || it.type == WITLESS ||
		it.type == BRUTEHAD)
	{
		unit->stats.maxhp -= (int)it.bonusValue; // увеличиваем макс хп
		unit->stats.hp -= (int)it.bonusValue; // плюсуем HP
	}
	else if (it.type == GLOVES || it.type == MOONSHARD || it.type == BUTTERFLY)
	{
		unit->bonusAS -= it.bonusValue; // плюсуем армор   ( для панели)
		TraceLog(LOG_INFO, "BUFF REMOVE: Unit %s now has bonusAS: %.2f", unit->name.c_str(), unit->bonusAS);

	}
}


// drag and drop
void ItemManager::UpdateMove(std::vector<struct UnitSetka*>& allUnits, Camera3D camera, AllSound& sounds)
{
	Vector2 mousePose = GetMousePosition();
	Vector2 invBasePos = { (float)GetScreenWidth() - 170, 300.0f }; // точка где находится отрисовка инвентаря


	//лкм берем 
	if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
	{
		for (int i = 0; i < maxSlots; i++)
		{
			float x = invBasePos.x + (i % 2) * 85; // вычисляем Х (сетка в 2 колонки)
			float y = invBasePos.y + (i / 2) * 85;// Вычисляем Y

			//проверяем попала ли мышка в квадрат слота
			if (CheckCollisionPointRec(mousePose, { x,y,70,70 }) && inventory[i].type != EMPTY)
			{
				isDragging = true; // включаем режим персонажа
				isDraggingFromUnit = false; //тянем из сумки 
				draggedItemIndex = i; // запоминаем какой слот тащим
				return;
			}
		}
		//проверяем клик по героям на поле 
		for(auto* unit : allUnits)
		{
			if (unit->active && unit->team == 0 && unit->unitItems[0].type != EMPTY)
			{
				Rectangle itemRect = RaschetItemsForUnit(unit, camera);
				if (CheckCollisionPointRec(mousePose, itemRect))
				{
					isDragging = true; // включаем режим персонажа
					isDraggingFromUnit = true; //тянем с юнита
					sourceUnit = unit; //с какого героя потянули
					//сохраняем предмет в временную переменную
					tempItem = unit->unitItems[0];
					return;
				}
			}
		}
	}
	if (isDragging)
	{
		//если отпустил лкм
		if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
		{


			bool placed = false;

			//определяем что мы ваще тащим 
			Item itemToPlace = isDraggingFromUnit ? tempItem : inventory[draggedItemIndex];


			//пока держим предмет то рисуем (круг для героея куда вставить прекдмет)
			for (auto* unit : allUnits)
			{
				if (unit->active && unit->team == 0)// толлько для живых союзников
				{
					//получаем тот же прямоугольник что и при рисовании кругов моих 
					Rectangle itemZone = RaschetItemsForUnit(unit, camera);



					// проверяем попал ли курсор в квадрат itemZone ( зону юнита)
					if (CheckCollisionPointRec(mousePose, itemZone))
					{
						//зашли сюда значит хотим положить шмотку на этого героя
						//неважно пустой он или нет

						/*короче если ты взял шмотку потащил ее понял что не хоч перетаскиваать и снова вернул ее то ничо не делаем*/
						if (isDraggingFromUnit && unit == sourceUnit) break;


						//  если на герое  уже что-то есть то делаем свап
						if (unit->unitItems[0].type != EMPTY)
						{

							// снимаем старый предмет и убираем его баффы 
							RemoveItemBuffs(unit, unit->unitItems[0]);

							Item oldTargetItem = unit->unitItems[0]; // запоминаем какие бафы были 

							if (isDraggingFromUnit)
							{
								// обмен шмотками(свап) перс перс 
								RemoveItemBuffs(sourceUnit, itemToPlace); // снимаем бафы  с первого перса
								sourceUnit->unitItems[0] = oldTargetItem; // отдаем ему шмотку с второго
								ApplyItemBuffs(sourceUnit, oldTargetItem); // бафаем первого юнита новой шмоткой
							} 
							else
							{
								// или старая шмотка в инвентарь
								inventory[draggedItemIndex] = oldTargetItem;
							}
						}
						else //если пустое 
						{
							if (isDraggingFromUnit)
							{
								//просто перетаскиваем с перса на перса (с которого перенесли ПУСТ БУДЕТ)
								RemoveItemBuffs(sourceUnit, itemToPlace);
								sourceUnit->unitItems[0] = { EMPTY };
							}
							else
							{
								// просто переносим из инвентаря 
								inventory[draggedItemIndex] = { EMPTY };
							}
						} 
						// отдаем предмет цели
						//в любом случае был пустой слот или освободили свапом кладем новую шмотку
						unit->unitItems[0] = itemToPlace; // кладем предмет тот что в руке

						//применяем бафы новой шмотки 
						ApplyItemBuffs(unit, unit->unitItems[0]);
						sounds.PlaySwapUnit();
						placed = true;
						break;
					}
				}
			}
			//проверяем инвентерь ( если бросили шмотку с героя обратно в инвентарь)
			if (!placed && isDraggingFromUnit)
			{
				for (int i = 0; i < maxSlots; i++)
				{
					float x = invBasePos.x + (i % 2) * 85; // вычисляем Х (сетка в 2 колонки)
					float y = invBasePos.y + (i / 2) * 85;// Вычисляем Y

					//проверка находится ли мышка над квадратом (слотом 70х70)
					if (CheckCollisionPointRec(mousePose, { x,y,70,70 }))
					{
						// елси слот инвентаря над которым мы опустили мыш пустой 
						if (inventory[i].type == EMPTY)
						{
							//удаляем бафы с героя
							RemoveItemBuffs(sourceUnit, itemToPlace);
							//очищаем слот предмета у негоя 
							sourceUnit->unitItems[0] = { EMPTY };
							//копирует перетаскиваемый предмет в этот слот инвентаря
							inventory[i] = itemToPlace;
							//помечаю шо шмотка в инвентаре
							placed = true;
						}
						// если в инвентаре лежит какая-то другая шмотка (свап)
						else
						{
							//снимаем беффы старой шммотки с героя 
							RemoveItemBuffs(sourceUnit, itemToPlace);
							//временно запоминаем шмотку которая уже лежала в жтом слоте
							Item oldInvItem = inventory[i];

							//ложим нашу шмотку (которая летит в инвентарь которую мы свапаем )
							inventory[i] = itemToPlace;

							//отдаем герою ту шмотку которая лежала в слоте(которую перетащили)
							sourceUnit->unitItems[0] = oldInvItem;

							//начисляем бафы герою от новой шмотки
							ApplyItemBuffs(sourceUnit, oldInvItem);

							placed = true; //шмотка в инвентаре
						}
						sounds.PlaySwapUnit();

						break;
					}
				}
			}
			//сброс всех флагов
			isDragging = false; //перетаскивание вырубаю 
			isDraggingFromUnit = false; // сброс флага что шмотка была взята с героя
			sourceUnit = nullptr; // забывает указатель на перса источник 
			draggedItemIndex = -1;  //сброс индекса предмета из инвентаря
		}
	}
}
