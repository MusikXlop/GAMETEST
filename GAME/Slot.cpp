#include "Slot.h"
#include "rlgl.h"
#include "raymath.h" //чтоб врубить Clamo
#pragma comment(lib, "opengl32.lib") // подключаем библиотерку (для glClear)
#define GL_DEPTH_BUFFER_BIT 0x00000100 //очищаем только глубину Z 
extern "C" void glClear(unsigned int mask); //говорим компилятору шо оно написано на С


// star1{0},star2{0},star3{0}  просто берем и инициализируем их нулями(пустыми)
Bench::Bench() : star1{ 0 }, star2{ 0 }, star3{ 0 }
{
	for (int i = 0; i < MAX_BENCH_SLOTS; i++) slots[i].active = false; // false т.к пустые в начале игры
	Clear();
}

void Bench::Clear()
{
	for (int i = 0; i < MAX_BENCH_SLOTS; i++)
	{
		slots[i].active = false;
		slots[i].unitID = -1;
		slots[i].lvl = 1;         // Персонажи возвращаются к 1 звезде
		slots[i].buyPrice = 0;
		slots[i].animTime = 0.0f;

		// Очищаем предметы на лавке 
		slots[i].unitItems[0].type = EMPTY;
	}
	selectedId = -1;
	isDragging = false;
	isSelected = false;
}



Bench::~Bench()
{
	UnloadTexture(star1);
	UnloadTexture(star2);
	UnloadTexture(star3);
}

//загрузка звезд один раз при старте
void Bench::Load()
{
	star1 = LoadTexture("starslvl/star_rank1_psd.png");
	star2 = LoadTexture("starslvl/star_rank2_psd.png");
	star3 = LoadTexture("starslvl/star_rank3_psd.png");
}




Rectangle Bench::GetSlotHitbox(int index) //хитбоксы
{
	// вычисляем поз поля(кольца) на земле для текущего слота 
	// startX  начало скамьи spacing  расстояние между центрами слотов.
	Vector2 pos = { startX + (index * spacing),startY };



	float boxWidth = 150.0f; //ширина 
	float boxHeight = 180.0f; //высота 
	//делаем невидимый прямоугольник (хитбокс) вокруг слота 

	return {
		pos.x + 50, // Центрируем отнимаем ровно половину ширины
		pos.y - boxHeight + 40,    // Сдвиг вверх. +20 
		boxWidth,                  // Ширина
		boxHeight                  // Полная высота
	};
}

//для покупки из магаза
bool Bench::AddHero(int unitID, int price,Setka& arena, Camera3D camera, Texture2D tex)
{
	//перебираем платформы слева на право 
	for (int i = 0; i < MAX_BENCH_SLOTS; i++)
	{
		// if slot active == false(свободно)
		if (!slots[i].active)
		{
			slots[i].unitID = unitID;// записывает айди героя в эту ячейку
			slots[i].active = true; // помечаем как занятую
			slots[i].lvl = 1; // 1 лвл у перса
			slots[i].buyPrice = price; // сохраняем цену покупки

			arena.InitializeHeroStats(slots[i]); //инициализируем с нуля
			return true; // возвращаем тру(магазу)
		}
	}
	return false; // если все занято
}
/* делаем перегрузку функций(два метода с одинаковым именем но с разными аргуметами) компилятор сам поймет какой вызывать
Метод для возврата с поля (сохранение статов и предметов)*/
bool Bench::AddHero(UnitSetka SaveAllHero, Camera3D camera, Texture2D tex)
{
	for (int i = 0; i < MAX_BENCH_SLOTS; i++)
	{
		//если свободно
		if (!slots[i].active)
		{
			//копируем все (лвл бьонус дамаг праймтс и тд) и прочее перейдет в слот
			slots[i] = SaveAllHero;
			slots[i].active = true;
			this->UpdateUnitWorldPos(i, camera, tex); // Обновляем поз шмоток

			return true;
		}
	}
	return false; //места нет 
}

bool Bench::HaveUnit(int UnitID)
{
	for (int i = 0; i < MAX_BENCH_SLOTS; i++)
	{
		//ищем среди занятых слотов тот где айди совпадает
		if (slots[i].active && slots[i].unitID == UnitID)
		{
			return true; // да = подсвечиваем
		}
	}
	return false; //нет
}

void Bench::checkMerge(Setka& arena, AllSound& sounds)
{
	//проход по слотам в скамейке
	for (int i = 0; i < MAX_BENCH_SLOTS; i++)
	{
		//если слот пустой или юнит 3го лвла то скип
		if (!slots[i].active || slots[i].lvl >= 3) continue;

		int searchID = slots[i].unitID;
		int searchlvl = slots[i].lvl;


		// это для того чтоб на поле и на бенче у нас были они оддинаковые
		// * позволяет менять ориг данные (через список)
		
		// *функция ищет одинаковых героев и на скамейке и на поле
		// чтоб не делать два отдельных цикла для изменения (собираем адреса в один список)
		// меняем его через адрес - меняем оригинал
		
		//структура чтоб запомнить где лежит юнит
		struct UnitRef
		{
			int* lvl; // указаталь на лвл(на бенче или на поле)
			bool* active; //указатель на  занята ли клетка 
			int* id; // id // -1 = пусто
			int* price; //цена
			bool isOnBench; //на бенче перс чи не
			UnitSetka* unit; //доступ к самому герою
		};
		//записываем адресс того места в памяти где они лежат


		//вектор для хранения индексов (найденных одинаковых юнитов)
		std::vector<UnitRef> foundPers;


		//тут ищем таких же юнитов того же лвлва по лавке
		for (int j = 0; j < MAX_BENCH_SLOTS; j++)
		{
			if (slots[j].active && slots[j].unitID == searchID && slots[j].lvl == searchlvl)
			{
				/* & тут берем адресс переменной ( теперь оно указывает в память скамейки)*/
				foundPers.push_back({&slots[j].lvl,&slots[j].active, &slots[j].unitID,
					&slots[j].buyPrice, true,&slots[j]});
			}
		}

		//ищем таких же на поле 
		for (int x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; y++)
			{
				if (arena.PoleHero[x][y].active && arena.PoleHero[x][y].unitID == searchID
					&& arena.PoleHero[x][y].lvl == searchlvl)
				{
					foundPers.push_back({ &arena.PoleHero[x][y].lvl,&arena.PoleHero[x][y].active,
						&arena.PoleHero[x][y].unitID, &arena.PoleHero[x][y].buyPrice,false,
						&arena.PoleHero[x][y]}); //передаем адрес юнита на поле 
				}
			}
		}



		//если нашли 2 одинаковых то объеденяем (2 и более типо купленные + те что уже были)
		if (foundPers.size() >= 2)
		{
			/* логик foundPers[0] станем 2м лвлом остальные удалятся 
			приоритет  если один из юнитов на поле лучше апнуть его чтоб он там и остался */

			int targetIdx = 0; //приоритет поля (по умолчанию первый найденный)

			//ищем есть ли среди найденных кто-то на поле 
			for (int u = 0; u < foundPers.size(); u++)
			{
				if (!foundPers[u].isOnBench) //если нагел кого-то на поле
				{
					//если среди найденных есть герой на поле то апаем
					targetIdx = u;
					break;
				}
			}
			//сумируем цену для продажи(обоих)
			int totalPrice = 0;
			for (int u = 0; u < 2; u++)
			{
				totalPrice += *foundPers[u].price;
			}

			//апаем юнита лвл ап 
			(*foundPers[targetIdx].lvl)++;
			//разыменование  ( иди по этому адресу и возьми или измени значение там )
			/*() нужны потому что ++ без скобок применился бы к указателю (сдвинул бы адрес)
			а не к значению 
			foundPers[targetIdx].lvl  это просто записанный адрес (например, 0xABC123).
			Звездочка * говорит: «Иди по адресу 0xABC123 и прибавь +1 к тому, что там лежит». */

			int newLvl = *foundPers[targetIdx].lvl;// считаем нью лвл типа
			if (newLvl == 2)
			{
				sounds.PlayMerge1to2(); // 1 to 2 
			}
			else if (newLvl == 3)
			{
				sounds.PlayMerge2to3();
			}

			
			//сразу обноляем статы 
			if (foundPers[targetIdx].unit != nullptr)
			{
				//разыменовываем указатель * чтоб передать сам объект
				arena.InitializeHeroStats(*foundPers[targetIdx].unit);
			}

			*foundPers[targetIdx].price = totalPrice;
			//записіваем суммарную цену в ориг переменную 

			//удаляем остальных с лавки потому что мы их апнули 
			int deleted = 0;
			for (int u = 0; u < foundPers.size(); u++)
			{
				if (u == targetIdx) continue; //целового не трогаем перса 

				//удаляем второго перса ( через указатели меняем ориг данные где бы они не лежали)
				*foundPers[u].active = false;
				*foundPers[u].id = -1;
				*foundPers[u].price = 0;
				deleted++;


				if (deleted >= 1) break;// удалили перса - стоп
			}


			//рекурсия (вызывает саму себя )
			/* пояснение
			допустим на лавке стоит герой 1го лвлва и я в магазе покупаю еще одного
			функция видит что есть еще один герой на лавке и объеденяет их в 2 лвл
			НО если у меня на лавке будет еще один герой 2го лвлва то рекурсия пройдется еще раз
			по коду и объеденит их в 3й лвл
			Вызов checkMerge() внутри себя заставляет функцию заново пробежаться по лавке
			*/
			checkMerge(arena,sounds);
			return;
		}
	}
}


int Bench::GetSellPrice(int index) //ПРОДАЖА ЮНИТОВ (индекс юнитов от 0 до 7)
{
	//если никто не выбран и слот пуст ( если оба возвращают 0 то ничего не делаем)
	if (index == -1 || !slots[index].active) return 0;

	int totalMoneyTake = slots[index].buyPrice; //сумируем траты на героя
	int level = slots[index].lvl; //лвл перса

	//логика продажи ( минус деньги)
	if (level == 1)
	{
		return totalMoneyTake; // 1 лвл продаю за полную цену
	}
	else {
		return totalMoneyTake - 1;//минус 2 за продажу 2-3 лвла 
	}
}


//анимация (зацикленная)
void Bench::Update(std::map<int, Animation>& library)
{
	for (int i = 0; i < MAX_BENCH_SLOTS; i++)
	{
		//обновляем анимции только для активный слотов которые не перетаскивается 
		if (slots[i].active && library.count(slots[i].unitID))
		{
			//берем ссылку на модель и анимацию из мапы
			Animation& data = library[slots[i].unitID];
			
			
			//проверка на наличие анимации 
			if (data.animations != nullptr)
			{
				//обнова кадра анимации 
				// Используем GetFrameTime() для плавности 30.0f  скорость кадров
				slots[i].animTime += GetFrameTime() * 30.0f;
				//границы конкретной анимации
				int maxFrames = data.animations[data.idleIndex].frameCount;




				//зацикливаем анимации
				if (slots[i].animTime >= maxFrames)
					slots[i].animTime = 0; //сброс в 0 когда до конца дошли
				/*  data.animations массив всех анимаций
				data.animations[data.idleIndex]  конкретная анимация
				frameCount скока кадров
				*/

				// применяем текущий кадр к костям модели
				UpdateModelAnimation(data.model, data.animations[data.idleIndex], slots[i].animTime);
			}
		}
	}
}


void Bench::Draw(Texture2D collisionSlot, Camera3D camera3d, std::map<int, Animation>& library,
	int& playerGold, Texture2D pluskMonete, Texture2D fonDelete, Texture2D deletePers, Texture2D btnCoin,
	Setka& arena, Vector2 currentTile, int dragPrice)
{
	//появляется если кто-то нажал на перса или тянет его 
	if (selectedId != -1 || isDragging)
	{

		/*ОТРИСОВКА зоны продажи
		{ sellZone.x + sellZone.width, sellZone.y } -точка отрисовки смещаем Х на ширину
		(т.к я картинку переворачиваю на 90 градусов и картинка развернется влево от этой точки

		90.0 - угол поворота) */
		DrawTextureEx(fonDelete, { sellZone.x + sellZone.width, sellZone.y }, 90.0f, 1.0f, WHITE);


		//рисуем типа для продажи 
		// (координаты внутри нашего фона) ,sellZone.x + 25, sellZone.y + 80,
		DrawTexture(deletePers, sellZone.x - 43, sellZone.y + 80, WHITE);


		//рисуем монкетку ниже ЧеловекЧка 
		DrawTexturePro(btnCoin,
			{ 0,0,(float)btnCoin.width,(float)btnCoin.height },
			{ sellZone.x + 25,sellZone.y + 246, 35,35 }, // поз и размер монеты 
			{ 0,0 }, 0, WHITE);


		//логика цены
		int finalPrice = 0;


		if (isDragging)
		{
			//если тащим юнита то используем цену которую передали снаружи
			finalPrice = dragPrice;
		}
		//Рисуем цену если индекс правильный от 0 до 7
		else if (selectedId >= 0 && selectedId < MAX_BENCH_SLOTS)
		{
			//если кто-то выбран на лавке то берем его цену и запоминем
			finalPrice = GetSellPrice(selectedId);
		}



		//если цена определилась то рисуем ее (больше нуля)
		if (finalPrice > 0)
		{
			DrawText(TextFormat("+%i", finalPrice), sellZone.x - 15, sellZone.y + 250, 25, WHITE);
		}
	}




	//цикл по слотам и обводка ( для обработки нажатий и обводки)
	for (int i = 0; i < MAX_BENCH_SLOTS; i++)
	{

		//поз каждой платформы (i * spacing)-сдвиг вправо
		Vector2 pos = { startX + (i * spacing), startY };

		//расчеты
		Rectangle slotRect = GetSlotHitbox(i);





		//если текущий индекс совпадает с кем кого мы выбрали кликом
		if (selectedId == i)
		{
			//то рисуем зеленую рамку выбранного героя 
			//{x -5 y-5} делаем рамку чуть шире слота чтоб она не сливалась 
			//4 толщина рамки
			DrawRectangleLinesEx({ slotRect.x - 5, slotRect.y - 5, slotRect.width + 10, slotRect.height + 10 }, 4, GREEN);
		}

		//если слот выбран selectedIdx == i подкрашиваем зеленым если нет то оставляем
		DrawTextureEx(collisionSlot, pos, 0.0f, 1.5f, (selectedId == i) ? GREEN : WHITE);
	}








	//камера интерфеса
	BeginMode3D(camera3d);

	//передний план рисуем персов
	for (int i = 0; i < MAX_BENCH_SLOTS; i++)
	{
		//если перетаскивает ща то не рисуем перса на скамейке
		if (isDragging && selectedId == i) continue;

		//проверка если в слоте кто-то есть active == true
		//проверка загружена ли моделька для этого id в библиотеку
		if (slots[i].active && library.count(slots[i].unitID))
		{
			//берем ссылку на модель и анимацию из мапы
			Animation& data = library[slots[i].unitID];



			//продвигаем время на анимации для этого конкретного слота
			slots[i].animTime += GetFrameTime();// Увеличиваем время анимации слота

			//расчитываем кадр(30 кадров в сек)
			int frame = (int)(slots[i].animTime * 30) % data.animations[data.idleIndex].frameCount; // считаем кадр
			UpdateModelAnimation(data.model, data.animations[data.idleIndex], frame);//двигаем кости модели

			//(collisionSlot.width * 1.5f * 0.5f);  половина ширины текстуры с учетом 1.5 размера
			float centerX = startX + (i * spacing) + (collisionSlot.width * 1.5f * 0.5f);
			float centerY = startY + (collisionSlot.height * 1.5f * 0.5f);

			
			
			
			//луч из камеры через точку на экране
			Ray ray = GetScreenToWorldRay({ centerX,centerY }, camera3d);


			//дистанция полоски пола (t)растояние от камеры
			float distance = (-1.3f - ray.position.y) / ray.direction.y;

			/*
			Y = -1.3
			ищем t(растояние от камеры) в уравнении
			P(t) = ray.position + ray.direction * t чем больше t тем дальше точка по лучу
			*/

			//игровая 3д позиция
			Vector3 pos3D = {
				ray.position.x + ray.direction.x * distance, //x
				-1.3f, //высота Y
				ray.position.z + ray.direction.z * distance // глубина Z подбирается автоматически по лучу
			};

			//сама модель (180 лицом к камере) 
			//1.0f.. маштаб Scale
			DrawModelEx(data.model, pos3D, { 0,1,0 }, 0.0f,
				{ data.scaleHero,data.scaleHero,data.scaleHero }, WHITE);

		}
	}
	EndMode3D();


	for (int i = 0; i < MAX_BENCH_SLOTS; i++)
	{
		//поз каждой платформы (i * spacing)-сдвиг вправо
		Vector2 pos = { startX + (i * spacing),startY };


		//рисуем платформы assets/bench_slot_ring_psd.png"); // обычная текстура (дефолт)
		//0.0  поворот 0.5 маштаб
		DrawTextureEx(collisionSlot, pos, 0.0f, 1.5f, WHITE);



		//ЗВЕЗДЫ 
		if (slots[i].active)
		{

			float centerX = pos.x + (collisionSlot.width * 1.5f * 0.5f) - (star1.width * 1.0f * 0.5f);
			/*pos.x  базовая координата левого верх угоа
			(collisionSlot.width * 1.5f * 0.5f) — находим центр текстуры кольца
			collisionSlot.width * 1.5f полная ширина кольца
			* 0.5f делит на полполам чтоб найти точку посередине

			 - (star1.width * 1.0f * 0.5f); такое же на ширину звезды
			 star1.width * 1.0f фулл ширина
			 * 0.5f) половина
			 Центр = (Позиция платформы + Половина ширины платформы) - Половина ширины звезды.
			*/

			float centerY = pos.y + 30.0f;// смещение ниже кольца

			if (slots[i].lvl == 1)
			{
				// лвл - 1 = бронзовая звезда 
				DrawTextureEx(star1, { centerX , centerY }, 0.0f, 1.0f, WHITE);
			}
			else if (slots[i].lvl == 2)
			{
				//lvl 2 = 2 star
				DrawTextureEx(star2, { centerX - 15,centerY }, 0.0f, 1.0f, WHITE);
				DrawTextureEx(star2, { centerX + 15,centerY }, 0.0f, 1.0f, WHITE);
			}
			else if (slots[i].lvl == 3)
			{
				//3 lvl = 3 star
				DrawTextureEx(star3, { centerX - 25, centerY }, 0.0f, 1.0f, WHITE);
				DrawTextureEx(star3, { centerX + 5, centerY }, 0.0f, 1.0f, WHITE);
				DrawTextureEx(star3, { centerX + 35, centerY }, 0.0f, 1.0f, WHITE);

			}
		}
	}



}

int Bench::GetClickedSlot(Vector2 mousePos)
{
	for (int i = 0; i < MAX_BENCH_SLOTS; i++)
	{
		//получаем прямоугольник(хитбокс) і слота
		Rectangle slotRect = GetSlotHitbox(i);

		//проверяем находится ли мышка внутри этого прямоугольника
		if (CheckCollisionPointRec(mousePos, slotRect))
		{
			return i; //возвращаем индекст слота
		}
	}
	return -1; //возвращаем -1 если кликнули мимо слотов
}

UnitSetka Bench::TakeUnit(int index, std::map<int, Animation>& library)
{
	//копираем весь объект из слота со всем 
	UnitSetka take = slots[index];

	
	//take.buyPrice = slots[index].buyPrice;
	//take.unitID = slots[index].unitID;


	//убеждаемся шо он активный  и имеет модел
	take.active = true;

	int id = slots[index].unitID;

	if (library.count(id))
	{
		take.modelka = library[id].model;

		
		take.scaleHero = library[id].scaleHero * 4.0f;
	}


	////заполняем данные слота 
	////копируем данные из слота скамьи в формат для поля
	//take.unitID = id; // 
	//take.lvl = slots[index].lvl; // 
	
	//take.active = true;

	////дотаем модель из библиотеки по id 
	//// library[id].model; та самая загруженная модель
	//take.modelka = library[id].model;

	//настройки поля 
	take.rotation = 180.0f; // разворот на 180
	take.animTime = 0.0f;

	//очистка скамейки
	slots[index].active = false;
	slots[index].unitID = -1;
	slots[index].buyPrice = 0; // только копирование

	//сбрасываем выбор в интерфейсе
	//isSelected = false;
	//selectedId = -1;

	return take;
}
