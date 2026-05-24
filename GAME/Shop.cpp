#include "Shop.h"
#include "Slot.h"
#include "Heroes.h"
#include "Setka.h"




Texture2D Shop::LoadAndTrack(const char* path)
{
	Texture2D tex = LoadTexture(path);
	allTextures.push_back(tex); // сохраняем список для автоматической выгрузки 
	return tex;
}


Shop::Shop(int& gold)
{
	btnOpen = LoadAndTrack("assets/icon_shop_open_psd.png");
	btnClose = LoadAndTrack("assets/icon_shop_close_psd.png");
	btnReroll = LoadAndTrack("assets/reroll_button_psd.png");
	btnLockOpen = LoadAndTrack("assets/lock_button_psd.png");
	btnLockClosed = LoadAndTrack("assets/locked_icon_psd.png");
	btnCoin = LoadAndTrack("assets/dac_shop_button_psd.png");
	slotring = LoadAndTrack("assets/bench_slot_ring_psd.png"); // обычная текстура (дефолт)
	collisionSlot = LoadAndTrack("assets/bench_slot_selection_psd.png"); //когда навелся  
	fonMagaza = LoadAndTrack("assets/puzzle_stats_button_psd.png"); // фон магаза

	pluskMonete = LoadAndTrack("assets/sell_unit_plus_psd.png"); // + для монеты
	sellZone = LoadAndTrack("assets/sell_unit_button_psd.png"); //фон для удаление
	deletePers = LoadAndTrack("assets/sell_unit_icon_psd.png");

	shopFont = LoadFontEx("fonts/UbuntuMono-BoldItalic.ttf", 32, 0, 250);

	btnXp = LoadAndTrack("assets/cirlce-button-grey-bg_png.png");

	//позиция кнопок (справа) по центру
	toggleBtn = { (float)GetScreenWidth() - 100, (float)GetScreenHeight() / 2,80,80 };//поз кнопки

	//поз кнопки опыта слевап от интерфейся 
	xpBtnRect = { (float)GetScreenWidth() / 2 - 320, (float)GetScreenHeight() - 90,70,70 };

	//настройки камеры
	heroCamera.position = { 0.0f, 2.5f,12.0f }; // чут выше земли Y , отодвинута вперед по Z
	heroCamera.target = { 0.0f, 2.0f,0.0f };
	heroCamera.up = { 0.0f, 1.0f, 0.0f };
	heroCamera.fovy = 35.0f; //
	heroCamera.projection = CAMERA_PERSPECTIVE;

	//инициализируем первыми героями
	Reroll(gold,true);
}

Shop::~Shop()
{
	for (auto& [id, data] : library)
	{
		UnloadModel(data.model);
		if (data.animations != nullptr)
			UnloadModelAnimations(data.animations, data.animsCount);
	}

	for (Texture2D& tex : allTextures)
	{
		UnloadTexture(tex);
	}

	allTextures.clear();

	UnloadFont(shopFont);
}



void Shop::Update(Vector2 mousePos, int& playerlvl, int& playerXP, int* xpTable,
	int& playerGold, Bench& gameSlot, Setka& arena, 
	Camera3D camera, Texture2D benchTexture, AllSound& sounds)
{
	//логика кнопки open/close 
	if (CheckCollisionPointRec(mousePos, toggleBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{
		isOpen = !isOpen;
		if (isOpen) sounds.PlayShopOpen();
		else  sounds.PlayShopClose();
	}

	if (!isOpen) return; // если закрыт то выходим из метода

	if (CheckCollisionPointRec(mousePos, rerollBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{
		if (!isLocked)// Рероллим только если замок открыт
		{
			Reroll(playerGold,false);
			sounds.PlayReroll();
		}
		
	}
	//кнопка замка(лока)
	if (CheckCollisionPointRec(mousePos, lockBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{
		isLocked = !isLocked;

		if (isLocked)
		{
			sounds.PlayLockOn(); //закрыл замок = звук закрытия 
		}
		else
		{
			sounds.PlayLockOff();  //открыл замок = открыл закрытия 
		}

	}
	//кнопка лвл апа  слотов поля 
	if (isOpen)
	{
		xpBtnRect = { 130, 65, 75, 75 }; //такие же координаты  что и в DrawXpBar (шоб если шо не уехало при разрешении экрана) 

		if (CheckCollisionPointRec(mousePos, xpBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			if (playerGold >= 4) // если денег не хватаеи
			{
				sounds.PlayUppEXP();
				playerGold -= 4; // тратим
				playerXP += 4; // добавляем опыт

				//если опыта стало больше чем надо для лвлва 
				if (playerXP >= xpTable[playerlvl])
				{
					playerXP -= xpTable[playerlvl]; // вычитаем потраченное 
					playerlvl++;
				}
			}
		}
	}
	

	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{
		float screenW = (float)GetScreenWidth(); // ширина фулл экрана

		for (int i = 0; i < (int)slots.size(); i++)
		{
			float xPos = i * (screenW / MAX_SHOP_SLOTS) + (screenW / (MAX_SHOP_SLOTS * 2)) - 100;
			slots[i].bounds = { xPos,20,200,150 };


			/* проверка 1 слот не может быть продан (!isSolid)
			проверка 2 мышка должна быть внутри прямоугольника */
			if (!slots[i].isSolid && CheckCollisionPointRec(mousePos, slots[i].bounds))
			{
				//если купили героя то проверяем деньги именно на этого героя
				if (playerGold >= slots[i].price)
				{


					//подставляем героя на слот для него(когда купили)
					//передаем его Id и цену 
					if (gameSlot.AddHero(slots[i].UnitID,slots[i].price,arena,camera, benchTexture))
					{
						// если есть место то минус деньги
						playerGold -= slots[i].price;

						sounds.PlayBuyHero();

						//помечаем как купленный 
						slots[i].isSolid = true; //убираем из магаза
						
						gameSlot.checkMerge(arena,sounds);
						

						TraceLog(LOG_INFO, "Hero ID %i purchased for %i gold!", slots[i].UnitID, slots[i].price);

						// Проверяем: остались ли еще герои в магазине?
						bool anyleft = false; 
						for (auto& s : slots) if (!s.isSolid) anyleft = true;

						// Если купили последнего снимаем замок
						if (!anyleft)
						{
							isLocked = false;
							sounds.PlayLockOff();
						}
					}
					else
					{
						// 4. Если места нет (AddHero вернула false)
						TraceLog(LOG_WARNING, "Bench is FULL");
					}

				}
			}
		}
	}
}





void Shop::Draw(int playerGold, int playerlvl, int playerXP, int* xpTable,
	Camera3D& camera3d, Bench& banchslot, Setka& arena)
{
	float screenW = (float)GetScreenWidth();
	


	float panelHeight = 200.0f; //высота магаза
	float itemY = panelHeight / 2 - 30; // центрирование кнопок в магазе 

	toggleBtn = { screenW - 120, itemY,100,100 };// кнопка магаза



	//кнопка открытия магаза  (закрыто)
	if (!isOpen)
	{
		// с 210 200 размера в 100 100
		DrawTexturePro(btnOpen, { 0,0,210,200 }, toggleBtn, { 0,0 }, 0, WHITE);

	}
	else
	{
	//(ОТКРЫТО)
	//фон магаза
		DrawTexturePro(fonMagaza,
			{ 0,0,348,64 }, // исходные размеры
			{ 0,0,screenW,panelHeight }, // куда тянем(весь экран по ширине)
			{ 0,0 }, 0, ColorAlpha(WHITE, 0.9f));

		DrawXpBar(playerXP, playerlvl, xpTable);

		// в 3д иду
		BeginMode3D(heroCamera);
		for (int i = 0; i < (int)slots.size(); i++)
			{
				if (!slots[i].isSolid)
				{
					//проверка есть ли такой тип героя в библиотеке моделей 
					if (library.count(slots[i].UnitID))
					{
						//берем ссылку на модель и анимацию из мапы
						// & — берём ссылку на данные в словаре
						Animation& data = library[slots[i].UnitID];

						// базовый маштаб из библиотеки 
						float finalScale = data.scaleHero;


						//ПУЛЬСАЦИЯ

						int id = slots[i].UnitID;
						int countlvl1 = 0; //считаем всех персов  1го лвла
						int countlvl2 = 0; // считаем всех персов  2го лвла
						bool has3lvl = false; // макс 3 лвв




						//считаем юнитов на бенче 
						for (int b = 0; b < 8; b++)
						{
							//проверка занят ли стол и если айди юнита совпадает с айди с тем кто в магазе 
							if (banchslot.GetUnit(b).active && banchslot.GetUnit(b).unitID == id)
							{
								if (banchslot.GetUnit(b).lvl == 1) countlvl1++;// нашли однозвездочного
								else if (banchslot.GetUnit(b).lvl == 2) countlvl2++;// нашли 2 звездочного
								else if (banchslot.GetUnit(b).lvl == 3) has3lvl = true;// нашли 3 зхвездочного (макс)
							}
						}


						//считаем всех юнитов на поле
						for (int x = 0; x < 8; x++)
						{
							for (int y = 0; y < 8; y++)
							{
								//проверяка каждую клетку поля 8х8 на наличие такого же  героя 
								if (arena.PoleHero[x][y].active && arena.PoleHero[x][y].unitID == id)
								{
									if (arena.PoleHero[x][y].lvl == 1) countlvl1++;
									else if (arena.PoleHero[x][y].lvl == 2) countlvl2++;
									else if (arena.PoleHero[x][y].lvl == 3) has3lvl = true;
								}
							}
						}

						//определяю надо ли пульсировать 
						bool needPulse = false; // оп умолчаниюнет

						//если 3й лвл уже собран то игнорим этот блок
						if (!has3lvl)
						{
							// собираю 2 лвл (если у меня есть 1й лвл и в магазе еще 1й лвл)
							if (countlvl1 == 1)
							{
								needPulse = true;
							}

							//есть 2й лвл и 1й лвл на поле( и 1й лвл в магазе)
							else if (countlvl2 == 1 && countlvl1 == 1)
							{
								needPulse = true;
							}
						}


						//маштабирования эффект
						//если юнит на скамейке и на аренето плавно меняет размер
						if (needPulse)
						{
							float pulse = (sinf(GetTime() * 4.0f) + 1.0f) * 0.5f; //от 1 до 0
							
							//прибавляем 15% к размеру юнита(с пульсацией)
							finalScale += (data.scaleHero * 0.15f) * pulse;
						}
						/* GetTime() возвращает количество в секундах (время растет постоянно)
						*  4.0(скорость) чем больше она тем быстрее моделька уменьшается/увеличивается 
						sinf() функция синуса (превращает растущее время 
						в типо волну вверх вниз (уменьшение/увеличение) 
						по стандарту у синуса значения от -1.0 до 1.0
						+ 1.0f) * 0.5f; переводит значения синуса из -1;1 в 0 1 

						 sinf()  + 1.0f = 0.0 до 2.0
						 *0.5  делит по полам получается 0.1 мин и 1.0 макс
						 из-за этого она плавно плывет между 0 и 1 


						 finalScale += (data.scaleHero * 0.15f) * pulse; финал расчет размера 

						 (data.scaleHero * 0.15f) амплитуда (увеличивает героя максиму на 15%)
						 * pulse умножаем 15% на значения от 0 до 1 
						 (когда 0 = базовый размер когда 1 = 15% прибавка) 
						*/



						//общая ширина 18.0f деним на 3+1(4) части
						//отступ
						float spacing3D = 18.0f / (MAX_SHOP_SLOTS + 1);
						
						//    -5.0f   -6 влево   -4 вправо
						//(i + 1) первый перс не у самого края
						Vector3 pos = { -9.0f + ((i + 1) * spacing3D),4.6f,0.0f };

						// увеличием счетчик кадров для слота этого 
						slots[i].animFrameCounter += GetFrameTime() * 30.0f;
						if (slots[i].animFrameCounter >= data.animations[data.idleIndex].frameCount)
							slots[i].animFrameCounter = 0;
						/*  data.animations массив всех анимаций 
						data.animations[data.idleIndex]  конкретная анимация 
						frameCount скока кадров
						*/


						if (data.animations != nullptr) {
							//обновляем позу модели согласно текущему кадру
							UpdateModelAnimation(data.model, data.animations[data.idleIndex], (int)slots[i].animFrameCounter);
						}
						

						//модель разворот на 0 градусов 
						// { 0,1,0 } ось Y (вертикально)
						DrawModelEx(data.model, pos, { 0,1,0 }, 0.0f, 
							{ finalScale, finalScale, finalScale }, WHITE);
						
					}
				}
			}
		EndMode3D();
		lockBtn = { 50,itemY,60,60 }; //кнопка замка (лево) горизонтальные

		rerollBtn = { screenW - 200, itemY,60,60 }; //рерол справа


		//закрытие магазина
		if (isLocked) {
			// Если ЗАБЛОКИРОВАНО (true), рисуем закрытый замок
			DrawTexturePro(btnLockClosed, { 0,0,57,72 }, 
				{ lockBtn.x + 10,lockBtn.y, 40,50 }, { 0,0 }, 0, WHITE);
			// 57 72  btnLockOpen btnLockClosed
			
		}
		else {
			// Если ОТКРЫТО (false), рисуем открытый замок
			// 138 138 do 60x60
			DrawTexturePro(btnLockOpen, { 0,0,138,138 }, lockBtn, { 0,0 }, 0, WHITE);
		}


		// 138 138 do 60x60 reroll 
		DrawTexturePro(btnReroll, { 0,0,138,138 }, rerollBtn, { 0,0 }, 0, WHITE);

		Rectangle rerollCoin = { rerollBtn.x + 40, rerollBtn.y - 5,30,30 };
		DrawTexturePro(btnCoin, { 0,0,100,100 }, rerollCoin, { 0,0 }, 0, WHITE);

		const char* rerollPriceText = TextFormat("%i", Reroll_cost);
		Vector2 rerollSizePrice = MeasureTextEx(shopFont, rerollPriceText, font_size_coin, 1);
		//шрифт, строка, размер, межсимвольный  интервал


		//центрируем цифру 2 внутри монеты 
		float OffsetX = -1.0f;
		float OffSetY = -1.0f;

		//центрирование цифры внутри монеты
		Vector2 rerollPricePos =
		{
			//Х монеты + половина ширины - половина  текста 
			rerollCoin.x + (rerollCoin.width / 2) - (rerollSizePrice.x / 2) + OffsetX,
			rerollCoin.y + (rerollCoin.height / 2) - (rerollSizePrice.y / 2) + OffSetY,

		};
		DrawTextEx(shopFont, rerollPriceText, rerollPricePos, font_size_coin - 2, 1, BROWN);


		// 210 200 do 60x60  close 
		DrawTexturePro(btnClose, { 0,0,210,200 }, toggleBtn, { 0,0 }, 0, WHITE);

		//слоты героев(магаз)
		Vector2 mousePos = GetMousePosition();

		//слоты персов в магазе
		for (int i = 0; i < (int)slots.size(); i++)
		{
			if (!slots[i].isSolid)
			{
				// рассчитываем х(между замком и реролом)
				float xPos =  i * (screenW / MAX_SHOP_SLOTS) + (screenW/(MAX_SHOP_SLOTS * 2)) -100;
				Rectangle slotRect = { xPos,20,200,150 }; // област слота для мышки 

				// выделяю = collisionSlot выделение else обычное 
				if (CheckCollisionPointRec(mousePos, slotRect)) {
					DrawTexturePro(collisionSlot, { 0,0,166,47 }, { xPos,140,200,50 }, { 0,0 }, 0, WHITE);
				}
				else {
					//кольцо при наводке снизу перса(обычное)
					DrawTexturePro(slotring, { 0,0,300,52 }, { xPos - 20, 140,250,50 }, { 0,0 }, 0, WHITE);
				}


				//рамка слота и цена 
				Rectangle heroCoin = { xPos + 140,150,35,35 };
				DrawTexturePro(btnCoin, { 0,0,100,100 }, heroCoin, { 0,0 }, 0, WHITE);

				const char* ramkaText = TextFormat("%i", slots[i].price);
				Vector2 ramkaSize = MeasureTextEx(shopFont, ramkaText, font_size_coin + 1, 1);

				float offSetX = -1.5f;  
				float offSetY = -2.5f; 

				Vector2 pPos =
				{
					heroCoin.x + (heroCoin.width / 2) - (ramkaSize.x / 2) + offSetX,
					heroCoin.y + (heroCoin.height / 2) - (ramkaSize.y / 2)+ offSetY
				};
				
				DrawTextEx(shopFont, ramkaText, pPos, font_size_coin, 1, BROWN);

				if (!slots[i].isSolid)
				{
					// получаем имя из массива UnitId 
					const char* currentName = heroNames[slots[i].UnitID].c_str();

					//ширина текста и шрифт
					Vector2 nameSize = MeasureTextEx(shopFont, currentName, 20, 1);

					//xpos начало слота (200 ширина)
					float nameX = xPos + (200 / 2) - (nameSize.x / 2);


					DrawTextEx(shopFont, currentName, { nameX, 125 }, 18, 1, WHITE);
				}
			}
		}
	}
	//общ количество денег 
	const char* goldText = TextFormat("%i", playerGold);
	Vector2 magasSize = MeasureTextEx(shopFont,goldText, font_size_coin + 2,1);


	Vector2 goldPos =
	{
		toggleBtn.x + 75.0f - (magasSize.x / 2.0f),
		toggleBtn.y + 28.0f - (magasSize.y / 2.0f)
	};


	DrawTextEx(shopFont, goldText, goldPos, font_size_coin, 1, YELLOW);

}

void Shop::DrawXpBar(int xp, int lvl, int* table)
{
	//поз относилтельно панели магаза 
	//130 чуть правее замка и y 65  ценьо панеди по высоте
	xpBtnRect = { 130,65,75,75 };

	//сжимаем картинку с 128  128 до 70 70
	DrawTexturePro(btnXp, { 0,0,128,128 }, xpBtnRect, { 0,0 }, 0, WHITE);

	//лвл внутри кнопки
	const char* lvlTxt = TextFormat("%i", lvl);
	Vector2 lvlSize = MeasureTextEx(shopFont, lvlTxt, 35, 1);
	Vector2 lvlPos =
	{
		xpBtnRect.x + (xpBtnRect.width / 2 ) - (lvlSize.x / 2),
		xpBtnRect.y + (xpBtnRect.height / 2) - (lvlSize.y / 2)
	};
	DrawTextEx(shopFont, lvlTxt, lvlPos, 35, 1, WHITE);


	//монетка с ценой 4 справа от кнопки
	Rectangle xpCoinRect = { xpBtnRect.x + xpBtnRect.width - 20, xpBtnRect.y + 40,40,40 };
	DrawTexturePro(btnCoin, { 0,0,100,100 }, xpCoinRect, { 0,0 }, 0, WHITE);

	//текст 4 внутри монеты
	DrawTextEx(shopFont, "4", { xpCoinRect.x + 12, xpCoinRect.y + 8 }, 22, 1, BROWN);

	//процент заполнения (скока ща опыта и скок надо до некст лвлва)
	float xpPercent = (float)xp / (float)table[lvl];
	if (xpPercent > 1.0f) xpPercent = 1.0f; //защита от вылета за края

	//полоска прогресса под кнопкой 7/42 
	float barY = xpBtnRect.y + xpBtnRect.height + 5;
	// пустая полоска(фон)
	DrawRectangle(xpBtnRect.x, barY, xpBtnRect.width, 8, RED);



	//рисувем часть заполнения (оранжевая полоска опыта
	DrawRectangle(xpBtnRect.x, barY, xpBtnRect.width * xpPercent, 8, ORANGE);

	//text XP 4 / 42 
	std::string info = std::to_string(xp) + " / " + std::to_string(table[lvl]);
	DrawText(info.c_str(), xpBtnRect.x + 5, barY + 12, 16, WHITE);
}

void Shop::LoadHeroModel(int type, const char* path, const char* idleName,float scale)
{
	//проверка есть ли файл ваще по указанному пути
	if (!FileExists(path))
	{
		TraceLog(LOG_WARNING, "FILE NOT FOUND %s", path);
		return;
	}


	//загрузка основных данных
	Animation data;
	data.model = LoadModel(path); //загружаем 3д текстуру
	//загружаем массив анимаций из файла
	data.animations = LoadModelAnimations(path, &data.animsCount); 
	//&data.animsCount передаем адресс переменной чтоб LoadModelAnimations записал количество анимации в нее
	data.scaleHero = scale; //запоминаем маштаб перса



	//инициализация всех индексов в 0 (защита от мусора в памяти)
	data.idleIndex = 0; //устанавливаем индекс анимации  по умолчанию 0 
	data.walkIndex = 0;
	data.attackIndex = 0;
	data.deathIndex = 0;
	data.winIndex = 0;
	data.spellIndex = -1; // не найдено 




	//если анимация не загрузилась не идем дальше
	if (data.animations == nullptr)
	{
		TraceLog(LOG_WARNING, "[SHOP] NO ANIMATIONS FOUND IN: %s", path);

	}
	else
	{
		//поиск анимации по имени 
		for (int i = 0; i < data.animsCount; i++)
		{
			std::string aName = data.animations[i].name;
			
			//поиск walk 
			if (aName.find("walk") != std::string::npos || aName.find("run") != std::string::npos)
				data.walkIndex = i;

			//поск атаки 
			if (aName.find("attack") != std::string::npos)
				data.attackIndex = i;

			//поик dead
			if (aName.find("dead") != std::string::npos || aName.find("die") != std::string::npos
				|| aName.find("death") != std::string::npos)
				data.deathIndex = i;

			//поиск вин  
			if (aName.find("win") != std::string::npos)
				data.winIndex = i;


			//spawn скелетов  (поиск wake)
			if (aName.find("wake") != std::string::npos ||
				aName.find("Spawn") != std::string::npos|| 
				aName.find("wakeup") != std::string::npos) 
			{
				data.idleIndex = i;
			}

			//поиск спелов (акс джага вк)
			if (aName.find("spell") != std::string::npos ||
				aName.find("cast") != std::string::npos ||
				aName.find("passivka") != std::string::npos ||
				aName.find("360") != std::string::npos)
			{
				data.spellIndex = i;
			}
				
		}
	}

	TraceLog(LOG_INFO, "========================================");
	TraceLog(LOG_INFO, "[SHOP] Loading Model: %s (Type: %d)", path, type);


	// 1. Сначала выводим ВООБЩЕ ВСЕ анимации, которые есть в файле
	for (int i = 0; i < data.animsCount; i++)
	{
		TraceLog(LOG_INFO, "  Found in file -> Anim[%d]: '%s'", i, data.animations[i].name);
	}

	//idleIndex ищем 
	for (int i = 0; i < data.animsCount; i++)
	{
		//сравнение текстового имя анимации в файле с тем что мы ищем 
		if (std::string(data.animations[i].name) == idleName)
		{
			data.idleIndex = i; // нашли номер анимации
			break;
		}
	}

	TraceLog(LOG_INFO, "  >> Result Indices: Idle: %d | Walk: %d | Attack: %d | Spell: %d",
		data.idleIndex, data.walkIndex, data.attackIndex, data.spellIndex);
	TraceLog(LOG_INFO, "========================================");



	library[type] = data; // кладем в словарик



}


void Shop::Reroll(int& playerGold,bool free)
{
	//Если стоит ЗАМОК  вообще ничего не делаем
	if (isLocked) 
	{
		return; 
	}
	if (!free)// Если игрок жмет кнопку реролла САМ
	{
		if (playerGold >= Reroll_cost)
		{
			playerGold -= Reroll_cost;
		}
		else
		{
			return; // dwneg net
		}
	}
	
	slots.clear(); //делитаем старых чтоб новые были
	float screenW = (float)GetScreenWidth();

	// ПРОВЕРКА: Если библиотека пуста, выходим, чтобы не было random(0, -1)
	//if (library.empty()) return;

	for (int i = 0; i < MAX_SHOP_SLOTS; i++)
	{
		//вычисляем х для каждого героя( i = 0 лево i= 1 центр i = 2 вправо)    

		float xPos = 150 + i * (screenW / (MAX_SHOP_SLOTS + 1));
		//+1 чтоб не упирался в край моника

		//невидимый прямоугольник для клика
		Rectangle clickArea = { xPos,20,200,150 };



		// 3 цена потом менять 
		// (int)library.size() колво загруженых персов
		int randomID = GetRandomValue(0, (int)library.size() - 1); // вычитаем 1 потому что 0 1 2 3
		//false - флаг isSolid не куплен (он материален)
		slots.push_back({ GetRandomValue(0,4),3,false,clickArea });
		//slots.push_back({ randomID, 3, false, clickArea });
	}
}
