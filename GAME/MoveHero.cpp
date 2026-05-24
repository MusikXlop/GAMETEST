#include "MoveHero.h"
#include "Setka.h"
#include "Slot.h"
#include "raylib.h"
#include "raymath.h" //чтоб врубить Clamo
#pragma comment(lib, "opengl32.lib") // подключаем библиотерку (для glClear)
#define GL_DEPTH_BUFFER_BIT 0x00000100 //очищаем только глубину Z 
extern "C" void glClear(unsigned int mask); //говорим компилятору шо оно написано на С
#include "GamePhase.h"


HeroController::HeroController()
{
	isDragging = false; //состояние держим мы ща героя в руке
	fromField = false;  //откуда взяли  true с поля false со скамьи 
	startSlotIndex = -1; //если со скаьи то идекс слота 
	startTile = { -1, -1 };  //если с поля несем то координаты берем x y 


	selectedTile = { -1,-1 }; //в начале ничего не выбрано 
	isTileSelected = false; //рамки нет
}


void HeroController::Update(Setka& arena, Bench& bench, Ray ray, std::map<int, Animation>& library,
	int& playerGold, int playerlevel, int currentPhase, 
	Camera3D camera, Texture2D collisionSlotTexture, ItemManager& items, AllSound& sounds)
{
	lastArena = &arena; // запоминаем адрес сетки
	lastBench = &bench; //запоминаем адрес скамьи


	Vector2 currentTile = arena.GetPlitkaTile(ray, (GamePhase)currentPhase); // узнаем над какой клеткой мышь

	Vector2 mousePos = GetMousePosition();



	bool battleMode = (currentPhase == BATTLE);
	
	//захват лкм 
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !isDragging)
	{
		if (battleMode)
		{
			float closestDis = FLT_MAX;
			bool hitAnthing = false;
			Vector2  hitTile = { -1,-1 };
			float hitHeight = 1.5f;

			//проверка активных юнитов на доске
			for (int x = 0; x < 8; x++)
			{
				for (int y = 0; y < 8; y++)
				{
					UnitSetka& unit = arena.PoleHero[x][y];
					
					if (unit.active && !unit.isDead)
					{
						//проверяем столкновение луча с world pos 
						RayCollision collision = GetRayCollisionSphere(ray, 
							{unit.worldPos.x,hitHeight, unit.worldPos.y}, //центр сферы(1.0) 
							2.0f); // радиус 




						if (collision.hit && collision.distance < closestDis)
						{
							closestDis = collision.distance;
							hitTile = { (float)x, (float)y };
							hitAnthing = true;
						}
					}
				}
			}
			if (hitAnthing)
			{
				selectedTile = hitTile; // нашли в какой ячейке лежит этот юнит в памяти 
				isTileSelected = true;
				bench.SetSelectedSlot(-1);
				return; //выходим так как цель найдена
			}
		}


		//проверка по зоне продажи если юнит выбран 
		//пробуем взять с  поля  (если юнит на лавке уже выделен)
			/*Продажа кликом если в лавке кто-то выбран (в рамке) и мы нажали на корзину то продаем его */
		if (bench.GetSelectedId() != -1 && CheckCollisionPointRec(mousePos, bench.GetSellZone()))
		{
			sounds.PlaySellHero();

			playerGold += bench.GetSellPrice(bench.GetSelectedId()); // добавляем золото в казну
			bench.RemoveHero(bench.GetSelectedId()); // очищаем данные слота на лавке
			bench.SetSelectedSlot(-1); // убираем выделение (панель закроется)
			return; //выход чтоб клик не сработал над объектов под кнопкой
		}


		//клик по полю 
		if (currentTile.x != -1)
		{

			//проверка наличия юнита
			//переобразуем координаты вектора2 в целые числа(индексы массива)
			int tx = (int)currentTile.x;
			int ty = (int)currentTile.y;


			//если в этой клетке кто-то стоит 
			if (arena.PoleHero[tx][ty].active)
			{
				//если мы уже выбрали этого героя(второй клик по нему) то начинаем тащить 
				if (isTileSelected && selectedTile.x == currentTile.x && selectedTile.y == currentTile.y)
				{
					handUnit = arena.ExtractUnit(tx, ty); //вынимаем героя из сетки в руку
					startTile = currentTile; //запоминаем откуда взяли героя 
					fromField = true; //помечаем как герой взят с поле а не из магаза 
					isDragging = true; //включаем режим перетаскивания 
					isTileSelected = false; //снимаем рамку так как он в руке 
					bench.SetDragging(true); //для продажи 
				}
				else
				{
					//если это первый клик то ставим рамку выделение
					selectedTile = currentTile;
					isTileSelected = true;
					sounds.PlayClickHero();
					bench.SetSelectedSlot(-1); // если на лавке кто-то был выделен  (снимаем там)
					
				}
			}
			else
			{
				//клик по пустой клетке поля
				isTileSelected = false;
				bench.SetSelectedSlot(-1);
			}
		}
		//если клик по лавке(скамье)
		else
		{
			int benchIdx = bench.GetClickedSlot(mousePos); // находим индекст(проверям попали ли из 1-8 слотов)
			if (benchIdx != -1 && bench.IsSlotActive(benchIdx))
			{
				//если кликнули по уже выделенному слоту то берем героя в руку
				if (bench.GetSelectedId() == benchIdx)
				{
					startSlotIndex = benchIdx; //запоминает из какого слота на лавке взяли юнита


					handUnit = bench.TakeUnit(benchIdx, library);//забираем его из слота 
					
					isDragging = true;//состояние держим мы ща героя в руке (da)
					
					bench.SetSelectedSlot(-1);
					isTileSelected = false; //выбран ли кто-то на поле (рамки нет)
					fromField = false; // взят с лавки 
					bench.SetDragging(true);// лавка перестает рисовать его в слоте
				}
				else
				{
					//если первый клик по слоту то выделяем его (рамка будет и  продажа)
					bench.SetSelectedSlot(benchIdx);
					isTileSelected = false;
				}
			}
			else
			{
				//если кликнули в любое другое место то сбрасываем все 
				isTileSelected = false;
				bench.SetSelectedSlot(-1);
			}
		}

	}
	//сброс (отпускаем лкм)
	if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && isDragging)
	{
		isDragging = false;//состояние держим мы ща героя в руке (net)
		bench.SetDragging(false); //лавка может рисовать персов если они там появляются 
		bench.SetSelectedSlot(-1); // отрисовка рамки


		//запрет во время игры 
		if (currentPhase == BATTLE)
		{
			//если во время файта навелись на корзину то удаляем 
			if (CheckCollisionPointRec(mousePos, bench.GetSellZone()))
			{
				int sellPrice = handUnit.buyPrice;
				if (handUnit.lvl > 1) sellPrice -= 1; // Штраф 1 голда для 2 и 3 уровня


				playerGold += sellPrice; //возвращаем деньги за покупку


				bench.SetSelectedSlot(-1); // очищаем интерфейс 
				return; //герой просто удаляется 
			}

			//если не продажа то принудительный возврат
			if (fromField)
			{
				// если с поля на занятый бенч то вытесненого ставим на поле
				arena.InsertUnit((int)startTile.x, (int)startTile.y, handUnit);
				arena.InitializeHeroStats(arena.PoleHero[(int)startTile.x][(int)startTile.y]);
			}
			else if (startSlotIndex != -1)
			{
				//если между слотом и бенчем то меняем местами
				bench.slots[startSlotIndex] = handUnit;
				bench.slots[startSlotIndex].active = true;

				//обновляем поз шмотки ток при возвращте в слот во время файта
				bench.UpdateUnitWorldPos(startSlotIndex, camera, collisionSlotTexture);
			}
			TraceLog(LOG_INFO, "BATTLE ACTIVE: Unit returned to start position");
			return; // выходим из функции чтоб код ниже (установка на поле не выполнился)
		}

		//вариант А перетаскиваем если дотащили до корзины и отпустили там 
		if (CheckCollisionPointRec(mousePos, bench.GetSellZone()))
		{
			int sellPrice = handUnit.buyPrice;
			if (handUnit.lvl > 1) sellPrice -= 1; // Штраф 1 голда для 2 и 3 уровня

			playerGold += sellPrice; //возвращаем деньги за покупку
			bench.SetSelectedSlot(-1); // очищаем интерфейс 
			sounds.PlaySellHero();

			return; //герой просто удаляется 
		}



		//Б ставим на метку 
		if (currentTile.x != -1)
		{
			int tx = (int)currentTile.x;
			int ty = (int)currentTile.y;

			//проверка пуста ли клетке 
			bool isTargetEmpty = !arena.PoleHero[tx][ty].active;

			// Если мы не с поля (значит из магазина/бенча) и хотим поставить на поле
			if (!fromField)
			{
				//считаем скок моих юнитов на поле
				int currenctUnitsOnPole = 0;
				for (int x = 0; x < 8; x++)
				{
					for (int y = 4; y < 8; y++) //моя половина поля
					{
						if (arena.PoleHero[x][y].active) currenctUnitsOnPole++;
					}
				}
				//запрет только если (клетка пустая и лимит уже забит)
				if (isTargetEmpty && currenctUnitsOnPole >= playerlevel)
				{
					// Добавляем камеру и текстуру, чтобы шмотки обновили координаты
					bench.AddHero(handUnit, camera, collisionSlotTexture); //возвращаем  на любое место в бенче
					isDragging = false; // сброс флаг переноса
					bench.SetDragging(false); // И сказать лавке снова рисовать юнита
					
					return;
				}
				//если клетка занята то игнорим лимит так как один уйдет а другой встанет чел
			}
			
			//если клетка пустая то ставим
			if (!arena.PoleHero[(int)currentTile.x][(int)currentTile.y].active)
			{
				//ставим на место старого
				arena.InsertUnit(tx, ty, handUnit); //просто ставим перса 
				sounds.PlayPlaceOnField();
				arena.InitializeHeroStats(arena.PoleHero[tx][ty]); //хп(разделения эти)



				// БАФФЫ (если они есть)
				if (arena.PoleHero[tx][ty].unitItems[0].type != EMPTY) {
					items.ApplyItemBuffs(&arena.PoleHero[tx][ty], arena.PoleHero[tx][ty].unitItems[0]);
				}
				else
				{
					TraceLog(LOG_WARNING, "INFO: No item on hero at [%d, %d]", tx, ty);

				}
			}
			else
			{
				//логика свапа (поле полое или поле бенч)
				//забираем юнита который уже стоит на  этой клетке
				UnitSetka targetUnit = arena.ExtractUnit(tx, ty); //убираем кто мешает

				sounds.PlaySwapUnit();


				//ставим нашего его на место нашего 
				arena.InsertUnit(tx, ty, handUnit);

				arena.InitializeHeroStats(arena.PoleHero[tx][ty]); //хп(разделения эти)
				

				// БАФФЫ ДЛЯ НОВОГО
				if (arena.PoleHero[tx][ty].unitItems[0].type != EMPTY) {
					TraceLog(LOG_INFO, "SUCCESS (SWAP): Item applied to handUnit");
					items.ApplyItemBuffs(&arena.PoleHero[tx][ty], arena.PoleHero[tx][ty].unitItems[0]);
				}
				//пытаемся вернуть вытесненого героя (targetUnit) обратно
				if (fromField)
				{
					//если мы пришли с другой клетки поля то ставим тула его (поле поле)
					arena.InsertUnit((int)startTile.x, (int)startTile.y, targetUnit);
					arena.InitializeHeroStats(arena.PoleHero[(int)startTile.x][(int)startTile.y]);

					UnitSetka& u = arena.PoleHero[(int)startTile.x][(int)startTile.y];
					if (u.unitItems[0].type != EMPTY)
					{
						items.ApplyItemBuffs(&u, u.unitItems[0]);
					}
				}
				else // бенч  поле (вытесняем на бенч)
				{
					//возвращаем вытесненого героя в старый слот бенча
					if (startSlotIndex != -1)
					{
						bench.slots[startSlotIndex] = targetUnit;
						bench.slots[startSlotIndex].active = true;
						bench.UpdateUnitWorldPos(startSlotIndex, camera, collisionSlotTexture);
					}
					else
					{
						bench.AddHero(targetUnit,camera,collisionSlotTexture);
					}
				}
			}
		}
		//Вариант В отпустили мимо поня ( на бенч или в пуститу)
		else
		{
			int targetBenchIdx = bench.GetClickedSlot(mousePos);

			if (targetBenchIdx != -1)
			{
				if (!bench.slots[targetBenchIdx].active)
				{
					//в пустой слот бенча 
					bench.slots[targetBenchIdx] = handUnit;
					bench.slots[targetBenchIdx].active = true;
					//ставаим в пустой слот бенча 
					bench.UpdateUnitWorldPos(targetBenchIdx, camera, collisionSlotTexture);

					sounds.PlaySwapUnit();
				}
				else
				{
					//свап внутри бенча или с поля на бенч
					UnitSetka targerUnit = bench.slots[targetBenchIdx];
					bench.slots[targetBenchIdx] = handUnit;
					bench.slots[targetBenchIdx].active = true;
					//обновляем шмотку для того кого поставили
					bench.UpdateUnitWorldPos(targetBenchIdx, camera, collisionSlotTexture);

					if (fromField)
					{
						// если с поля на занятый бенч то вытесненого ставим на поле
						arena.InsertUnit((int)startTile.x, (int)startTile.y, targerUnit);
						arena.InitializeHeroStats(arena.PoleHero[(int)startTile.x][(int)startTile.y]);
					}
					else if (startSlotIndex != -1)
					{
						//если между слотом и бенчем то меняем местами
						bench.slots[startSlotIndex] = targerUnit;
						bench.slots[startSlotIndex].active = true;
						// Обновляем шмотку для того кого вынесли ( в старый слот)
						bench.UpdateUnitWorldPos(startSlotIndex, camera, collisionSlotTexture);
					}
					sounds.PlaySwapUnit();
				}
			}
			else
			{
				// в пустоту (возврат)
				if (fromField)
				{
					arena.InsertUnit((int)startTile.x, (int)startTile.y, handUnit);
					arena.InitializeHeroStats(arena.PoleHero[(int)startTile.x][(int)startTile.y]);
				}
				else if (startSlotIndex != -1)
				{
					bench.slots[startSlotIndex] = handUnit;
					bench.slots[startSlotIndex].active = true;
					// Возврат в старый слот при промахе мимо поля/бенча
					bench.UpdateUnitWorldPos(startSlotIndex, camera, collisionSlotTexture);
				}
				sounds.PlaySwapUnit();
			}
		}
	}
}


void HeroController::DrawInHand(Ray ray, std::map<int, Animation>& library)
{
	//проверем есть ли такой герой в билиотеке и берем настройки ( если есть то рисуем)
	if (!library.count(handUnit.unitID)) return;



	Animation& data = library[handUnit.unitID];

	//дистанция от камеры для одинакового размера
	float handDist = 15.0f;

	//игровая 3д позиция
	Vector3 handPos = {
		ray.position.x + ray.direction.x * handDist, //x
		ray.position.y + ray.direction.y * handDist, //высота Y
		ray.position.z + ray.direction.z * handDist // глубина Z подбирается автоматически по лучу
	};


	//ограничения по глубине Z и по ширине X 
	handPos.z = Clamp(handPos.z, -50.0f, 250.0f); //верх граница
	// -10 граница ближе к камере -60 глубина поля
	handPos.x = Clamp(handPos.x, -150.0f, 150.0f); //бок




	//очищаем глубину (GL_DEPTH_BUFFER_BIT  стрирает только Z буфер)
	glClear(GL_DEPTH_BUFFER_BIT); 
	//



	// используем scale шоб юнит не был мелкий/биг 
	DrawModelEx(handUnit.modelka, handPos, { 0,1,0 }, handUnit.rotation,
		{ data.scaleHero,data.scaleHero,data.scaleHero }, WHITE);
}

std::vector<struct UnitSetka*> HeroController::getAllUnits()
{
	std::vector<UnitSetka*> all;


	// если Update еще ни разу не вызвался то возвращаем пустой список 
	if (!lastArena || !lastBench) return all;

	//добавляем тех кто на бенче 
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			//если в клетке кто-то есть то добавляем  адресс этого юнита 
			if (lastArena->PoleHero[x][y].active)
			{
				all.push_back(&lastArena->PoleHero[x][y]);
			}
		}
	}

	//сбрасываем юнитов с бенча
	for (int i = 0; i < 8; i++)
	{
		//если в слоте на скамье кто-то есть 
		if (lastBench->slots[i].active)
		{
			all.push_back(&lastBench->slots[i]);
		}
	}
	return all;
	
}

//UnitSetka* возвращает указатель это позволяет  изменить или читать данные героя(хп броню и тд)
// Setka& arena, Bench& bench чтоб метод  заглянул туда и увидел героев
UnitSetka* HeroController::GetSelectedUnit(Setka& arena, Bench& bench)
{
	//если тыщим героя то возвращаем адрес того кого тащим
	if (isDragging) return &handUnit;


	//Проверяем если выбрана плитка на поле 
	if (isTileSelected && selectedTile.x != -1)
	{
		//обращаемся к двумерному массиву поля PoleHero по координатам х y 
		// & берем  адрес конкретно жтого юнита в памяти и сохраняем его в unit
		UnitSetka* unit = &arena.PoleHero[(int)selectedTile.x][(int)selectedTile.y];

		//unit->active стоит ли там кто-то (если есть возвращаем адрес)
		if (unit->active) return unit;
	}

	//если на поле никто не выбран то проверяем бенч
	int benchIdx = bench.GetSelectedId(); //индекст выбранного слота 0-7
	if (benchIdx >= 0 && benchIdx < 8)
	{
		UnitSetka* unit = &bench.slots[benchIdx];

		//unit->active стоит ли там кто-то (если есть возвращаем адрес)
		if (unit->active) return unit;
	}
	return nullptr;
}


