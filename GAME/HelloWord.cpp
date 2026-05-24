#include <raylib.h>
#include "Setka.h"
#include "Shop.h"
#include "Slot.h"
#include "MoveHero.h"
#include "ItemManager.h"
#include "ChoiceItem.h"
#include "ProfileHero.h"
#include "GamePhase.h"
#include "AllSound.h"


GamePhase currentPhase = PREPARATION; //текущая наша фаза

// Таймер для автоматического перехода между фазами
float phaseTimer = 30.0f; //переход между фазами 30 сек
float battleStartDelay = 0.0f; // шляпа для паузы перед началом раунда



//логика запуска сражения
void StartBattle(Setka& arena, std::map<int, Animation>& library, ItemManager& items,AllSound& sounds)
{
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++){
			if (arena.PoleHero[x][y].active) {

				UnitSetka& unit = arena.PoleHero[x][y];

				//вызываем метом из класса setka
				arena.InitializeHeroStats(unit);

				//накладываем бафы предмета заново
				//проверка есть ли шмотка в слоте 
				if (unit.unitItems[0].type != EMPTY)
				{
					items.ApplyItemBuffs(&unit, unit.unitItems[0]);
				}

				//сонхронизируем базу 
				unit.baseDamage = (int)unit.stats.damage - unit.baseDamage;
				unit.baseArmor = (int)unit.stats.armor - unit.baseArmor;


				//присваиваем модель из библиотеки по айди 
				int id = arena.PoleHero[x][y].unitID;
				if (library.find(id) != library.end())
				{
					arena.PoleHero[x][y].modelka = library[id].model;
				}

				//игроки снизу (y > 3)0 1 2 3     враги сверху (y <=3)  4 5 6 7 
				arena.PoleHero[x][y].team = (y > 3) ? 0 : 1;
				arena.PoleHero[x][y].currentState = UnitSetka::SEARCH;
				arena.PoleHero[x][y].isDead = false;
				arena.PoleHero[x][y].attackTimer = 0.0f; // Обнуляем
				if (arena.PoleHero[x][y].hasSpell) // если есть спел у юнита 
				{
					arena.PoleHero[x][y].skillTimer = 0.0f; // то обнуляем cd способности 
				}

			}
		}
	}
	currentPhase = BATTLE;
	battleStartDelay = 4.0f; //делей 4 сек перед файтом
	sounds.PlayBattleMusic(); //батл музыка 

	sounds.PlayRoundStart(); 
}



int main()
{
	InitWindow(0, 0, "Dadada");
	InitAudioDevice();
	ToggleFullscreen();
	Camera3D camera = { 0 };
	/*
	позиция камеры
	X  по центру ширины поля 
	Y  высоко чтоб видеть всю доску сверху
	Z  отодвигаем назад чтоб доска влезла в кадр
	*/
	camera.position = { 0.0f,50.0f,42.0f }; 
	
	/*
	цель камеры 
	смотрим в центр доски но чуть-чуть смещаем цель назад на Z 3 
	чтоб наклонить камеру и доска была по центру экрана
	*/
	camera.target = { 0.0f,0.0f,5.0f };
	
	//вверх камеры по оси Y   
	camera.up = { 0.0f,1.0f,0.0f };

	//угол обзора
	camera.fovy = 40.0f;


	camera.projection = CAMERA_PERSPECTIVE;
	Model altar = LoadModel("setka/altarfinall233.glb"); // задний фон 
	Texture2D FullHp = LoadTexture("hp/icon_health_lg_psd.png");
	Texture2D HalfHp = LoadTexture("hp/icon_health_mid_lg_psd.png");
	Texture2D LowHp = LoadTexture("hp/icon_health_low_lg_psd.png");

	Texture2D LeftChoice = LoadTexture("fonmeny/debut_bg_loadout_psd.png");
	Texture2D RightChoice = LoadTexture("fonmeny/debut_bg_psd.png");
	Font mainFont = LoadFontEx("fonts/UbuntuMono-BoldItalic.ttf", 32, 0, 250);


	int playerGold = 100;
	int playerHP = 100; //hp
	int currentRound = 1; // round 
	float finishTimer = 0.0f;

	int playerLevel = 1; // текущй лвл (скок мест)
	int playerXp = 0; //текущимй опыт 


	int itemsNow = 0; //сколько раз игрок уже получил шмотку за раунды
	const int MAX_ITEMS = 8;

	/*таблица опыта скок над чтоб получить +1 слот на 
	лвл               0 1 2  3 4   5 6  7*/
	int xpTable[] = { 0,2,6,12,20,30,42,52};
	Shop gameShop(playerGold);
	AllSound sounds;  
	sounds.Load(); //загрузка саундов


	


	gameShop.LoadHeroModel(0, "res/dkFinal.glb", "dk_idle", 0.45f);
	gameShop.LoadHeroModel(1, "res/WkFinal.glb", "wk_idle", 0.23f);
	gameShop.LoadHeroModel(2, "res/juggFinal.glb", "jugg_idle", 0.43f);
	gameShop.LoadHeroModel(3, "res/DrowFinal1.glb", "drow_idle", 0.30f);
	gameShop.LoadHeroModel(4, "res/axeFinal5.glb", "idle_axe", 0.28f);
	//gameShop.LoadHeroModel(5, "res/sven2.glb", "Sven_idle",0.35f);

	gameShop.LoadHeroModel(50, "res/greenSKeletWKFinal.glb", "idle_skelet1", 0.40f);
	gameShop.LoadHeroModel(51, "res/RedSkeletWkFinal.glb", "skelet_idle", 0.40f);
	gameShop.LoadHeroModel(52, "res/skeletWKSkeletFinal.glb", "skelet_idleWk", 1.00f);


	gameShop.LoadHeroModel(100, "enemyteam/zombie.glb", "zombie_idle", 0.30f);

	gameShop.LoadHeroModel(101, "enemyteam/direCreep3.glb", "creepDire_idle", 0.30f);

	gameShop.LoadHeroModel(102, "enemyteam/bluieOgr.glb", "BleuOgr_idle", 0.50f);

	gameShop.LoadHeroModel(103, "enemyteam/RadiantCreep2.glb", "creepRadiantDwa_idle", 0.30f);

	gameShop.LoadHeroModel(104, "enemyteam/YellowBear.glb", "YelowBear_idle", 0.30f);
		
	gameShop.LoadHeroModel(105, "enemyteam/bigGolem.glb", "bigGolem_idle", 0.30f);

	gameShop.LoadHeroModel(106, "enemyteam/creedRadiant.glb", "creep_idle", 0.30f);

	gameShop.LoadHeroModel(107, "enemyteam/redBear.glb", "redBear_idle", 0.27f);

	gameShop.LoadHeroModel(108, "enemyteam/tuskGhost.glb", "tuskGhost_idle", 0.30f);

	gameShop.LoadHeroModel(109, "enemyteam/tuskSkelet.glb", "tuskSkelet_idle", 0.30f);

	gameShop.LoadHeroModel(110, "enemyteam/ancientDire.glb", "ancientDire_idle", 0.35f);

	gameShop.LoadHeroModel(111, "enemyteam/ancientRadiant.glb", "ancientRadiant_idle", 0.50f);
	Bench banchslot;
	Setka arena;
	ProfileHero profileHero;


	arena.LoadUnitsProfile();

	ItemManager items;

	ChoiceItem choiceItemBuffs;
	
	float choicePhateTimer = 0.0f;
	bool nagrada = false; //генерация нью шмоток

	banchslot.Load();
	items.LoadItems();
	profileHero.LoadRes();
	HeroController controller;


	//для отображения 
	int lastGoldEarned = 0;
	int lastDamageTaken = 0;
	bool isWin = false;

	SetTargetFPS(60);
	sounds.PlayPrepareMusic(); //при  старте врубает 




	//берем список всех юнитов и с пола и с бенча
	std::vector<UnitSetka*> allUnits;
	while (!WindowShouldClose())
	{
		//логика магазина 
		// передаем поз мышки и ссылку на зоото
		Vector2 mousePos = GetMousePosition();

		allUnits = controller.getAllUnits();//тут память переиспользуется просто(

		/* GetScreenToWorldRay   создает луч который  выстреливает из 
        камеры сквозь курсор в глубь экрана  */
		Ray ray = GetScreenToWorldRay(mousePos, camera);
		




		float dt = GetFrameTime();// Получаем время кадра для таймера

		sounds.UpdateMusic(); //фон музыка 

		Texture2D benchRing = gameShop.getSlotRing();

		switch (currentPhase)
		{
			case PREPARATION:

			{
				phaseTimer -= dt; // Уменьшаем время закупа



				static bool gornPlayed = false;
				//вызов горна за 8 сек до старта 
				if (phaseTimer <= 8.0f)
				{

					if (!gornPlayed) 
					{ 
						sounds.PlaySiren(); 
						gornPlayed = true; 
					}
				}



				/* 1 поз мышки  2 текущий  3 лвл текущий опыт
				4 массив с таблицей опыта   5 золото ссылка на бенч  6 ссылкав на сетку*/
				gameShop.Update(mousePos,playerLevel,playerXp,xpTable ,
					playerGold, banchslot, arena,camera, benchRing,sounds);
				
				//контролле(захват героев выделение клеток свапы
				controller.Update(arena, banchslot, ray, gameShop.getLibrary(), 
					playerGold, playerLevel, PREPARATION,camera, benchRing,items,sounds);

				items.UpdateMove(allUnits, camera,sounds);

				//условие либо тайм вышел или ентер нажать
				if (phaseTimer <= 0.0f || IsKeyReleased(KEY_ENTER))
				{
					gornPlayed = false; // сбрасываем для некст раунда

					arena.SpawmEnemyWave(currentRound, gameShop);// spawn enemy
					arena.SaveStartPos(); // запоминаем где кто стоял
					StartBattle(arena, gameShop.getLibrary(), items, sounds); // переключаем на фазу файта
					phaseTimer = 60.0f; //  минуту файта делаем
				}
				break;
			}
			case BATTLE:
			{
				//ЕСЛИ таймер паузы еще тикает 
				if (battleStartDelay > 0.0f)
				{
					battleStartDelay -= dt; 
					//в это время UpdateBattle не вызываем юниты просто стоят в IDLE
				}
				else
				{

					//обновляем логику боя(движения атаки)
					arena.UpdateBattle(currentRound, items, gameShop.getLibrary(),sounds);

					phaseTimer -= dt;
				}


				//контролле(захват героев выделение клеток свапы
				controller.Update(arena, banchslot, ray, gameShop.getLibrary(), 
					playerGold, playerLevel, BATTLE, camera, benchRing,items,sounds);

				// ВАЖНО: здесь НЕ вызываем heroController.Update, 
				// поэтому игрок не может поднять героя мышкой во время драки.



				// Проверяем закончился ли бой 
				if (arena.checkEndBattle() || phaseTimer <= 0.0f)
				{
					currentPhase = FINISH;
					finishTimer = 0.0f;
				}
				break;
			}
			case FINISH:

				// если таймер только начался выполняем расчеты один раз
				if (finishTimer == 0.0f)
				{
					// проверка урона по игроки
					int aliveEnemies = 0;
					for (int x = 0; x < 8; x++)
					{
						// Проверяем верхнюю половину доски (y от 0 до 3)
						for (int y = 0; y < 4; y++)
						{
							// Если клетка активна и враг не помер — он выжил
							if (arena.PoleHero[x][y].active && !arena.PoleHero[x][y].isDead &&
								arena.PoleHero[x][y].team == 1)
								aliveEnemies++;
						}
					}

					//если енеми выжил то получаем минус хп
					if (aliveEnemies > 0) //ЛОГИКА поражения
					{
						isWin = false; //флаг для отрисовки надписи поражения 
						lastDamageTaken =  3 + aliveEnemies; //3 + скока типов осталось енеми
						playerHP -= lastDamageTaken; // отнимаем хп один раз
						lastGoldEarned = 2; //так скажем утешительный приз типу
						playerGold += lastGoldEarned;
						sounds.PlayMatchLose();
					}
					else
					{
						isWin = true; // мы мин 
						lastDamageTaken = 0;
						lastGoldEarned = 5 + (currentRound / 2); // 5 + номер раунда пополам 
						playerGold += lastGoldEarned;
						sounds.PlayWinStreak();
					}

					//проверка если игрок умер уходим в gameover 
					if (playerHP <= 0)
					{
						playerHP = 0;
						currentPhase = GAMEOVER;
						sounds.PlayMatchLose();
						break; //выходим из кейса финиш чтоб код ниже не выполнялся 
					}


					// 2. Вызываем реролл магазина (твоя новая логика с isLocked сработает тут)
					gameShop.Reroll(playerGold, true);
					arena.RespawnUnitsAfterBattle(); //respawn heroes
				}


				//начинаем тикать таймер 
				finishTimer += dt;

				//ждем 4 сек перед переходом к закупке
				if (finishTimer >= 4.0f)
				{
					finishTimer = 0.0f; // сбрасываем для некст раунада 
					currentRound++;// Переходим к следующей волне
					controller.ResetSelection(); // сброс рамки перед нью раундом

					// кратное 3 (3  6 и т д) и мы еще не набрали 8 шмоток
					if (currentRound % 4 == 0 && itemsNow < MAX_ITEMS)
					{
						currentPhase = CHOICE_ITEMS; // уходим на выбор шмоток
						choicePhateTimer = 15.0f; // 15 sec
						nagrada = false; //сброс флага для генерации нью шмоток

						sounds.PlayPrepareMusic();
					}
					else
					{

						phaseTimer = 30.0f; //тайм на закупку
						currentPhase = PREPARATION; 			// возвращаем фазу подготовки

						sounds.PlayPrepareMusic();
					}
				}
				break;
			case CHOICE_ITEMS:
			{
				choicePhateTimer -= dt;

				//генерация шмоток толькоо один раз при входе в фазу 
				if (!nagrada)
				{
					choiceItemBuffs.GenerateItemsChoice(items);
					nagrada = true;
				}

				//проверяем клик игрока
				if (choiceItemBuffs.Update(mousePos, items, sounds))
				{
					itemsNow++; // засчитываем полученные шмотки
					currentPhase = PREPARATION;
					phaseTimer = 30.0f;
				}

				//если время вышло то автовыбор 
				if (choicePhateTimer <= 0.0f)
				{
					choiceItemBuffs.AutoPickItems(items);
					itemsNow++;
					currentPhase = PREPARATION;
					phaseTimer = 30.0f;
				}

				break;
			}
		
			case GAMEOVER:
			{
				// логика GAMEOVER (ждем нажатия пробела)
				if (IsKeyPressed(KEY_SPACE))
				{
					//присваиваем значения новым перменным  чтобы менять ОРИГИНАЛЬНЫЕ переменные
					playerGold = 5;
					playerHP = 100; //hp
					currentRound = 1; //сброс раунда 
					finishTimer = 0.0f;
					itemsNow = 0; //сброс шмоток 
					playerXp = 0; //сброс экспы
					playerLevel = 1; //сброс лвлва 
					// Таймер для автоматического перехода между фазами
					phaseTimer = 30.0f; //переход между фазами 30 сек
					gameShop.Reroll(playerGold, true); // Даем новые карты для новой игры
					
					//фулл очистка поля от всех юнитов 
					for (int x = 0; x < 8; x++)
					{
						for (int y = 0; y < 8; y++)
						{
							arena.PoleHero[x][y].active = false;
							arena.PoleHero[x][y].unitID = -1;
							arena.PoleHero[x][y].target = nullptr;

							//очистка предметов на поле 
							arena.PoleHero[x][y].unitItems[0].type = EMPTY;
						
						}
					}
					banchslot.Clear(); // очищаем бенч и сброс рамки и цену

					controller.ResetSelection(); // чтоб рамка не осталась от прошлого раунда

					sounds.PlayPrepareMusic(); //после запуска нью игры

					// Переключаем фазу только когда всё почистили
					currentPhase = PREPARATION;
				}
			}
			break;
		}


		BeginDrawing();
		ClearBackground(BLACK);
	
		BeginMode3D(camera);
		//окружение и сцена
			DrawModel(altar, { 0.0f,-0.1f,0.0f }, 1.0f, WHITE);
			arena.Draw();
			


			// Рисуем хитбоксы юнитов в бою (только для отладки)
			//if (currentPhase == BATTLE) {
			//	for (int x = 0; x < 8; x++) {
			//		for (int y = 0; y < 8; y++) {
			//			if (arena.PoleHero[x][y].active && !arena.PoleHero[x][y].isDead) {

			//				// Используй те же значения, что и в логике клика!
			//				float debugHeight = 1.5f; // Было 1.0f
			//				float debugRadius = 2.0f; // Было 1.2f

			//				DrawSphereWires(
			//					{ arena.PoleHero[x][y].worldPos.x, debugHeight, arena.PoleHero[x][y].worldPos.y },
			//					debugRadius, 14, 14, LIME // Увеличил детализацию до 14, чтоб шар был круглее
			//				);
			//			}
			//		}
			//	}
			//}

			//динамическая подсветка(подсказка при перетаскивании)
			if (controller.isDragging)
			{
				Vector2 hoverTile = arena.GetPlitkaTile(ray, currentPhase);

				if (hoverTile.x != -1)
				{
					arena.DrawHightlight(hoverTile); //синий квадрат
				}
			}

			//юниты которые уже стоят на поле
			arena.DrawUnits(gameShop.getLibrary(), camera, gameShop);



			EndMode3D();

			// Рисуем подсветку только если контроллер говорит что клетка выбрана(после клика)
			if (controller.isTileSelected)
			{
				arena.DrawHightlight(controller.selectedTile);
			}

			//получаем активного юнита (неважно с поля он или с бенча)
			UnitSetka* selectedUnit = controller.GetSelectedUnit(arena, banchslot);


			//если кто-то выбран то рисуем панель
			if (selectedUnit != nullptr && selectedUnit->active)
			{
				profileHero.Draw(selectedUnit);
			}

			if (currentPhase == PREPARATION)
			{
				int currentUnits = 0; //юниты
				//считаем ток своих юнтов ниже на 4 ряда сетки
				for (int x = 0; x < 8; x++)
				{
					for (int y = 4; y < 8; y++)
					{
						if (arena.PoleHero[x][y].active) currentUnits++;
					}
				}

				//поз счетчика (который лвл в ап колво юнитов в шопе) 
				float scoreUiX = GetScreenWidth() / 2.0f;

				//опускаю текст +- де сетка противников 
				float scoreUiY = 300.0f;

				//используем TextFormat для создания строки  1 / 3  сколько стоит и лимит
				const char* countText = TextFormat("%d / %d", currentUnits, playerLevel);
				float fontSize = 40.0f;

				//считаем нужную ширину текста чтоб выровнять по центру
				Vector2 textSize = MeasureTextEx(mainFont, countText, fontSize, 2);


				Color textColor = WHITE; // 3/3 то белый прост
				if (currentUnits > playerLevel) textColor = RED; // слишком много  юнитов  ошибки 
				else if (currentUnits < playerLevel) textColor = GREEN; // все норнм свободные места есть

				//рисуем
				DrawTextEx(mainFont, countText, { scoreUiX - textSize.x / 2, scoreUiY }, fontSize, 2, textColor);

			}



			arena.DrawHpHero(camera);

			if (currentPhase == GAMEOVER)
			{
				DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
				
				const char* gameOverText = "FAME OVER";
				Vector2 sz1 = MeasureTextEx(mainFont, gameOverText, 60, 2);
				
				DrawTextEx(mainFont, gameOverText, { GetScreenWidth() / 2.0f - sz1.x / 2, GetScreenHeight() / 2.0f - 50 }, 60, 2, RED);


				const char* RestartText = "Press PROBEL to Restart";
				Vector2 sz2 = MeasureTextEx(mainFont, RestartText, 25, 2);


				DrawTextEx(mainFont, RestartText, { GetScreenWidth() / 2.0f - sz2.x / 2, GetScreenHeight() / 2.0f + 30 }, 25, 2, LIGHTGRAY);

			}
			else
			{
				//магазин поверх 3д 
				gameShop.Draw(playerGold, playerLevel,playerXp,xpTable, gameShop.GetHeroCamera(), banchslot, arena);

				if (controller.isDragging) {
					TraceLog(LOG_INFO, "DRAG CHECK: Price=%d, isDragging=%d",
						controller.GetHandUnitPtr()->buyPrice,
						controller.isDragging);
				}

				banchslot.Draw(
					gameShop.getSlotRing(),
					gameShop.GetHeroCamera(),
					gameShop.getLibrary(),
					playerGold,
					gameShop.getPlusMoneta(),        // Картинка плюса
					gameShop.getSellZone(),           // Текстура фона (в Draw это fonDelete)
					gameShop.getDeletePers(),         // Текстура человечка с мусоркой
					gameShop.getBtnCoin(),             // Текстура монетки
					arena,
					controller.selectedTile, // Передаем выделенную плитку из контроллера
					(controller.isDragging ? (controller.GetHandUnitPtr()->buyPrice - (controller.GetHandUnitPtr()->lvl > 1 ? 1 : 0)) : 0));
					//ТАЩИМ ли мы перса если нет то цена продажи не пишется 
					//если да то берем цену 
					//базовая цена (buyPrice) - штраф ( если лвл больше 1 то минус 1 иначе минус 0)

				//рисуем интерфейс предметов поверх 3д //круги  на персов
				items.DrawDropZones(allUnits, camera);


				//шмотки героев
				items.ItemsDrawInHero(arena, banchslot, controller.GetHandUnitPtr(), controller.isDragging, camera);
				
				//инвентарь
				items.DrawInventory({ (float)GetScreenWidth() - 170, 300.0f });


				// Рисуем героя в руке ПОСЛЕДНИМ в 3D сцене
				if (controller.isDragging) 
				{
					//в 3д заходим то для героя  в руке
					BeginMode3D(camera);

					controller.DrawInHand(ray, gameShop.getLibrary());

					EndMode3D();
				}



				//отрисовка выбора 
				if (currentPhase == CHOICE_ITEMS)
				{
					//передавать текстуру левой панели и правой 
					choiceItemBuffs.Draw(LeftChoice, RightChoice, choicePhateTimer);
				}


				//визуал раунда 
				if (currentPhase == BATTLE && battleStartDelay > 0.0f)
				{
					//не мног затемнить экран 
					DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, 0.4f));


					//строка с номером раунда
					const char* roundText = TextFormat("ROUND %d", currentRound);



					// считаем ширину текста чтоб он точно был по центру
					Vector2 rSz = MeasureTextEx(mainFont, roundText, 80, 2);



					// Крупный номер раунда
					DrawTextEx(mainFont, roundText, { GetScreenWidth() / 2.0f - rSz.x / 2, 
						GetScreenHeight() / 2.0f - 100 }, 80, 2, YELLOW);

					//название волны 
					//проверка шоб не выйти за пределы массива
					if ((currentRound - 1) < waveRoundsPos.size())
					{
						//так как массив начинается с 0 а у меня раунды с 1 то название текущей волны 
						//waveRoundsPos[currentRound - 1].waveName.c_str();
						const char* waveNameText = waveRoundsPos[currentRound - 1].waveName.c_str();
						Vector2 wSz = MeasureTextEx(mainFont, waveNameText, 40, 2);
						// название волны чуть ниже центра
						DrawTextEx(mainFont, waveNameText, { GetScreenWidth() / 2.0f - wSz.x / 2, 
							GetScreenHeight() / 2.0f + 20 }, 40, 2, WHITE);
					}
				}

				//отрисовка таймера и фазы
				if (currentPhase == PREPARATION || currentPhase == BATTLE)
				{
					//формируем время (округляем дол сотых)
					const char* timerText = TextFormat("%02d", (int)ceil(phaseTimer));
					//ceil(timeleft)-  округление в большую сорону

					int timerFontSize = 50;

					Vector2 textSize = MeasureTextEx(mainFont, timerText, timerFontSize, 2);

					Vector2 timepos = { GetScreenWidth() / 2.0f - textSize.x / 2.0f, 50.0f };

					Color timeColor = (phaseTimer <= 8.0f && currentPhase == PREPARATION ) ? RED : WHITE;

					//для красоты подложка 
					DrawTextEx(mainFont, timerText, { timepos.x + 2, timepos.y + 2 }, timerFontSize, 2, BLACK);

					// основной текст 
					DrawTextEx(mainFont, timerText, timepos, timerFontSize, 2, timeColor);
				}

				//инфа вин луз
				if (currentPhase == FINISH)
				{
					int sw = GetScreenWidth(); 
					int sh = GetScreenHeight();

					//рисуем подложку окна  (черно полупрозрапчный прямоугольник в центре экрана
					DrawRectangle(sw / 2 - 250, sh / 2 - 100, 500, 200, ColorAlpha(BLACK, 0.7f));
					//рисуем рамку окна (если вин белое если луз то красное)
					DrawRectangleLines(sw / 2 - 250, sh / 2 - 100, 500, 200, isWin ? GREEN : RED);


					if (isWin)
					{
						const char* winText = "WIN";

						Vector2 sizeWin = MeasureTextEx(mainFont, winText, 50, 2);
						//если победа то вин пишем и скока голды упало
						DrawTextEx(mainFont, winText, { sw / 2.0f - sizeWin.x / 2, sh / 2.0f - 80 }, 50, 2, GOLD);



						//+ gold 
						const char* goldText = TextFormat("+%d GOLD", lastGoldEarned);
						Vector2 goldSize = MeasureTextEx(mainFont, goldText, 25, 2);
						DrawTextEx(mainFont, goldText, { sw / 2.0f - goldSize.x / 2, sh / 2.0f + 10 }, 25, 2, YELLOW);



					}
					else
					{
						//поражение 
						const char* loseT = "LOSE";
						Vector2 loseSize = MeasureTextEx(mainFont, loseT, 50, 2);
						DrawTextEx(mainFont, loseT, { sw / 2.0f - loseSize.x / 2, sh / 2.0f - 80 }, 50, 2, RED);


						//минус хп 
						const char* hpT = TextFormat("-%d HP", lastDamageTaken);
						Vector2 hpSize = MeasureTextEx(mainFont, hpT, 25, 2);
						DrawTextEx(mainFont, hpT, { sw / 2.0f - hpSize.x / 2, sh / 2.0f + 0 }, 25, 2, MAROON);


						//голда
						const char* goldLose = "+2 GOLD";
						Vector2 goldloseSize = MeasureTextEx(mainFont, goldLose, 20, 2);
						DrawTextEx(mainFont, goldLose, { sw / 2.0f - goldloseSize.x / 2.0f, sh / 2.0f + 40 }, 20, 2, GOLD);
					}
					//полоска таймера  (внизу окна)
					//(finishTimer / 4.0f) * 460.0f; дает нам процент от 0.0 до 1.0 который мы умножаем на ширину
					float barWidth = (finishTimer / 4.0f) * 460.0f;
					DrawRectangle(sw / 2 - 230, sh / 2 + 90, barWidth, 10, WHITE);
				}



				//выбираем текстуру в зависимости от хп плеера 
				Texture2D* currentHeart = &FullHp;
				Color HpColor = GREEN;

				if (playerHP < 40)
				{
					currentHeart = &LowHp;
					HpColor = ORANGE;
				}
				else if (playerHP < 70)
				{
					currentHeart = &HalfHp;
					HpColor = YELLOW;
				}

				//инконка уменьшаю  до 60 х50
				DrawTexturePro(*currentHeart, //текстура которую рисуем (рапзыменовываем)
					{ 0,0,(float)currentHeart->width, (float)currentHeart->height }, //исходные данные фала
					{ 20,20,60,55 }, // поз 20 х 20    60 50 широина и высота 
					{ 0,0 }, 0, WHITE); //0 0 верхний левый угол точка отсчета  (0 поворот рисунка  рисую ровно)

				//рисуем цифры (хп сверху)
				DrawTextEx(mainFont, TextFormat("%d", playerHP), { 32, 32 }, 22, 2, WHITE);
				//fps
				//DrawFPS(10, 50);

			}

			
		EndDrawing();




	}
	UnloadModel(altar);
	UnloadTexture(LeftChoice);
	UnloadTexture(RightChoice);
	UnloadTexture(FullHp);
	UnloadTexture(HalfHp);
	UnloadTexture(LowHp);
	UnloadFont(mainFont);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
