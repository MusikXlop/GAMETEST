#include "Setka.h"
#include <cmath> // для fmodf
#include "raymath.h" // для Vector2Distance


Setka::Setka()
{
	model = LoadModel("setka/dire_board3.glb");
	tSize = SIZE / (float)COUNT; // 26/8 = 3.25

	star1 = LoadTexture("starslvl/star_rank1_psd.png");
	star2 = LoadTexture("starslvl/star_rank2_psd.png");
	star3 = LoadTexture("starslvl/star_rank3_psd.png");

	//заполняем массив нулям чтоб не было случайных данных
	for (int x = 0; x < COUNT; x++)
	{
		for (int y = 0; y < COUNT; y++)
		{
			PoleHero[x][y].active = false; //клетка пуста
			PoleHero[x][y].unitID = -1; //пусто
			PoleHero[x][y].modelka = { 0 };
		}
	}
}

Setka::~Setka()
{
	//выгрузка доски
	UnloadModel(model);
	UnloadTexture(star1);
	UnloadTexture(star2);
	UnloadTexture(star3);

	//выгружаем портреры 
	for (auto const& [id, tex] : heroPortret)
	{
		UnloadTexture(tex);
	}

	//выгружаем иконки спелов
	for (auto const& [id, tex] : spellsIcons)
	{
		UnloadTexture(tex);
	}
}

void Setka::Draw()
{
	DrawModel(model, { 0,0,0 }, 1.0f, WHITE);

}


void Setka::LoadUnitsProfile()
{
	//мои персы 
	heroPortret[0] = LoadTexture("myteamIcon/npc_dota_hero_dragon_knight_persona1_png.png");
	heroPortret[1] = LoadTexture("myteamIcon/npc_dota_hero_skeleton_king_png.png");
	heroPortret[2] = LoadTexture("myteamIcon/npc_dota_hero_juggernaut_png.png");
	heroPortret[3] = LoadTexture("myteamIcon/npc_dota_hero_drow_ranger_png.png");
	heroPortret[4] = LoadTexture("myteamIcon/npc_dota_hero_axe_png.png");


	//скелеты вк
	heroPortret[50] = LoadTexture("myteamIcon/npc_dota_dark_troll_warlord_skeleton_warrior_png.png");
	heroPortret[51] = LoadTexture("myteamIcon/npc_dota_dark_troll_warlord_skeleton_warrior_png.png");
	heroPortret[52] = LoadTexture("myteamIcon/npc_dota_dark_troll_warlord_skeleton_warrior_png.png");


	//с 100 начинаем енеми 
	heroPortret[100] = LoadTexture("EnemyIcon/npc_dota_unit_undying_zombie_png.png");
	heroPortret[101] = LoadTexture("EnemyIcon/npc_dota_hero_creep_dire_png.png");
	heroPortret[102] = LoadTexture("EnemyIcon/npc_dota_neutral_ogre_magi_png.png");
	heroPortret[103] = LoadTexture("EnemyIcon/npc_dota_hero_creep_radiant_png.png");
	heroPortret[104] = LoadTexture("EnemyIcon/npc_dota_neutral_polar_furbolg_champion_png.png");
	heroPortret[105] = LoadTexture("EnemyIcon/npc_dota_neutral_granite_golem_png.png");
	heroPortret[106] = LoadTexture("EnemyIcon/npc_dota_hero_creep_radiant_png.png");
	heroPortret[107] = LoadTexture("EnemyIcon/npc_dota_neutral_polar_furbolg_ursa_warrior_png.png");
	heroPortret[108] = LoadTexture("EnemyIcon/npc_dota_neutral_ghost_png.png");
	heroPortret[109] = LoadTexture("EnemyIcon/npc_dota_neutral_ghost_png.png");

	heroPortret[110] = LoadTexture("EnemyIcon/npc_dota_hero_tower_dire_png.png");
	heroPortret[111] = LoadTexture("EnemyIcon/npc_dota_hero_tower_radiant_png.png");




	//иконки моих персов 0 1 2 3 
	spellsIcons[0] = LoadTexture("spells/dragon_knight_dragon_blood_helm_persona1_png.png");
	spellsIcons[1] = LoadTexture("spells/skeleton_king_bone_guard_png.png");
	spellsIcons[2] = LoadTexture("spells/juggernaut_blade_dance_png.png");
	spellsIcons[3] = LoadTexture("spells/drow_ranger_marksmanship_png.png");
	spellsIcons[4] = LoadTexture("spells/axe_counter_helix_png.png");



	//скелеты вк
	spellsIcons[50] = LoadTexture("spells/action_stop_png.png");
	spellsIcons[51] = LoadTexture("spells/action_stop_png.png");
	spellsIcons[52] = LoadTexture("spells/action_stop_png.png");

	//с 100 иконки енеми 
	spellsIcons[100] = LoadTexture("spells/action_stop_png.png");
	spellsIcons[101] = LoadTexture("spells/action_stop_png.png");
	spellsIcons[102] = LoadTexture("spells/action_stop_png.png");
	spellsIcons[103] = LoadTexture("spells/action_stop_png.png");
	spellsIcons[104] = LoadTexture("spells/action_stop_png.png");
	spellsIcons[105] = LoadTexture("spells/action_stop_png.png");
	spellsIcons[106] = LoadTexture("spells/action_stop_png.png");
	spellsIcons[107] = LoadTexture("spells/action_stop_png.png");
	spellsIcons[108] = LoadTexture("spells/action_stop_png.png");
	spellsIcons[109] = LoadTexture("spells/action_stop_png.png");
	spellsIcons[110] = LoadTexture("spells/action_stop_png.png");
	spellsIcons[111] = LoadTexture("spells/action_stop_png.png");

}

Vector3 Setka::GetTileWorldPos(int x, int y)
{
	// 3.25
	float tileSize = SIZE / (float)COUNT;

	// Сдвигаемся от центра в самый угол сетки (левый верхний)
	float startOffset = -SIZE / 2.0f;

 
	// Угол + (номер клетки * размер) + половина размера (чтобы попасть в центр клетки)
	float worldX = startOffset + (x * tileSize) + (tileSize / 2.0f);
	float worldZ = startOffset + (y * tileSize) + (tileSize / 2.0f);

	// Возвращаем точку на уровне пола (Y = 0)
	return { worldX, 0.0f, worldZ };
}






void Setka::DrawHightlight(Vector2 tile)
{
	if (tile.x == -1) return;
	// если меш на клетке то рисуем подсветку


	//центр клетки для отрисовки подсветки (для клетки)
	//px -левый край + (индекс * размер) + половина размера
	//tile.x * tSize отступаем на нужное количество клеток
	// по х колонки 
	float px = -13.0f + (tile.x * tSize) + (tSize / 2.0f);
	// по Y колонки 
	float pz = -13.0f + (tile.y * tSize) + (tSize / 2.0f);

	float drawSize = tSize - PADDING;//размер подсветки меньше клетки

	DrawPlane({ px,HIGHLIGHT_Y,pz }, { drawSize,drawSize }, ColorAlpha(BLUE, 0.3f));//полупрозрачный квадрат
	DrawCubeWires({ px, HIGHLIGHT_Y,pz }, drawSize, 0.01f, drawSize, BLUE);

}

Vector2 Setka::GetPlitkaTile(Ray ray, GamePhase phase)
{
	//используем BoundingBox   модели для точности
	BoundingBox box = GetModelBoundingBox(model);
	RayCollision coll = GetRayCollisionBox(ray, box);


	if (coll.hit)
	{
		//если доска растянута от -13 до 13 то ширина(width будет) 26   
		//13 - (-13) = 26
		float width = box.max.x - box.min.x;
		float depth = box.max.z - box.min.z; //глубина

		/*
		Нам нужно понять в какой "процент" доски попал луч.
		относительные координаты (0.0 - 1.0)
		(coll.point.x - box.min.x) находит растояние от самого левого края доски то точки удара луча
		 / width ( растояние)
		*/
		float relativeX = (coll.point.x - box.min.x) / width;
		float relativeZ = (coll.point.z - box.min.z) / depth;


		//перевод в индексы ячеек
		//берем наш процент и умножаем его на колво клеток(8)
		int c = (int)(relativeX * COUNT); // колонки 0 7 (int дотбрасывает дробную часть)
		int r = (int)(relativeZ * COUNT);// ряд 0 7 

		//ограничения если кликнули на ряды 0 1 2 3 (не наше поле)
		// так же в подготовке нельзя кликать на енеми
		//возвращаем -1 как будто клика не было 
		if (phase == PREPARATION && r < 4) return { -1.0f, -1.0f };

		//оставляем тока 4 5 6 7 поля
		if (c >= 0 && c < COUNT && r >= 0 && r < COUNT) return { (float)c, (float)r };
	}
	return { -1.0f,-1.0f };
}



bool Setka::HaveUnitOnPole(int id)
{
	for (int x = 0; x < COUNT; x++)
	{
		for (int y = 0; y < COUNT; y++)
		{
			//если клетка занята и id совпадает 
			if (PoleHero[x][y].active && PoleHero[x][y].unitID == id)
			{
				return true;
			}
		}
	}
	return false;
}





//отрисовка всех на доске 
void Setka::DrawUnits(std::map<int, Animation>& library, Camera3D camera, Shop& shop)
{
	//скока кадров анимации
	static bool logsPrinted = false; // Статическая переменная, чтобы сработало 1 раз
	if (!logsPrinted) {
		TraceLog(LOG_INFO, "=== HERO ANIMATION DATA ===");
		for (auto const& [id, animData] : library) {
			// Берем индекс анимации атаки для этого героя
			int attackIdx = animData.attackIndex;
			// Достаем саму анимацию из массива
			ModelAnimation attackAnim = animData.animations[attackIdx];

			// Выводим ID героя и количество кадров его атаки
			TraceLog(LOG_INFO, "Hero ID: %d | Attack Frames: %d", id, attackAnim.frameCount);
		}
		TraceLog(LOG_INFO, "============================");
		logsPrinted = true;
	}



	BoundingBox box = GetModelBoundingBox(model); //берем границы поля
	for (int x = 0; x < COUNT; x++) // 0 7 проходимся по колонке 
	{
		for (int y = 0; y < COUNT; y++)// 0 7 проходимся по ряду
		{

			/*проверяем active в массиве сетки PoleHero
			если false то пусто
			(стоит ли кто-то в этой ячейке массива PoleHero)
			*/
			if (PoleHero[x][y].active)
			{


				//получаем айди юнита
				int id = PoleHero[x][y].unitID;


				/*Проверка если id юнита нет в библиотеке магазина
				то програма пропустит отрисовку*/
				//id не пустой и не такой как в библиотеке
				if (id != -1 && library.find(id) != library.end())
				{

				

					//выбираем индекс анимации на основе состояния юнита
					int animIdx = library[id].idleIndex; //по умолчанию стоим

					if (PoleHero[x][y].isDead)
					{
						animIdx = library[id].deathIndex; // анимация смерти
					}
					else
					{
						/*switch - прога спрашиваем каждый кадр юнита чо он делает если у него в
						currentState = move то прога лезет в библиотеку и берем walkIndex и т д*/
						switch (PoleHero[x][y].currentState)
						{
						case UnitSetka::ATTACK:
							animIdx = library[id].attackIndex;
							break;
						case UnitSetka::MOVE:
							animIdx = library[id].walkIndex;
							break;
						case UnitSetka::SPELL:
							animIdx = library[id].spellIndex;
							break;
						case UnitSetka::WIN:
							animIdx = library[id].winIndex;
							break;
						default:
							animIdx = library[id].idleIndex;
							break;
						}
					}



					//проверяем что индекст не кривой и не выходит за границу массива анимаций модели 
					if (animIdx < 0 || animIdx >= library[id].animsCount)
					{
						animIdx = library[id].idleIndex; // если спел не найдет -1 или индекст битый  ставим idle 
					}

					//берем  модель из библиотеки animation в shop.h (общая)
					ModelAnimation anim = library[id].animations[animIdx];// берем idle 

					//обновляем время(кадры)
					//смерть отдельно проверяем потому что если юнит помер то все равно какая была анимация до этот
					if (PoleHero[x][y].isDead)
					{
						//убеждаемся что проигрывается именно анимация смерти 
						animIdx = library[id].deathIndex;
						anim = library[id].animations[animIdx];


						//для смерти проигрываем один раз и до конца 
						//увеличиваем время анимации именно этого юнита(*30.0f чтоб кадры быстрее были)
						PoleHero[x][y].animTime += GetFrameTime() * 30.0f;
						if (PoleHero[x][y].animTime >= anim.frameCount - 1)
							PoleHero[x][y].animTime = (float)anim.frameCount - 1;
						/*fmodf делаем анимацию бесконечной
						но смерть не может быть бесконечной иначе герой будет бесконечно валяться у меня
						так шо делаем  anim.frameCount - 1 чтоб он замер в ласт кадре
						*/

						//обновляем модель передаем текущий кадр как int
						//связывает скелет модельки с кадром анимации
						UpdateModelAnimation(PoleHero[x][y].modelka, anim, (int)PoleHero[x][y].animTime);
					}
					else // рассчет для живых
					{
						//скорость атаки(МОДЕЛЬКИ)

						float animSpeed = 30.0f; //

						//если юнит атакует разгоняем его анимацию 
						if (PoleHero[x][y].currentState == UnitSetka::ATTACK)
						{
							switch (id)
							{
								//hero
							case 0: animSpeed = 62.5f; break; // dk
							case 1: animSpeed = 120.7f; break; // wk 
							case 2: animSpeed = 108.9f; break; // jugg
							case 3: animSpeed = 57.1f; break; // drow
							case 4: animSpeed = 67.0f; break; // axe


								//Skeleti
							case 50: animSpeed = 50.0f; break; //  скелет(обычный
							case 51: animSpeed = 140.0f; break; // красный скелет
							case 52: animSpeed = 120.0f; break; // скелет зеленый


								//енеми 
							case 100: animSpeed = 120.0f; break; //зомбак
							case 101: animSpeed = 70.0f; break; // dire creep
							case 102: animSpeed = 110.0f; break;// blue ogr
							case 103: animSpeed = 78.0f; break; //radiantCreep
							case 104: animSpeed = 62.0f; break; // Yellow Bear
							case 105: animSpeed = 125.0f; break;// BigGolem
							case 106: animSpeed = 110.0f; break; //creep radiant
							case 107: animSpeed = 145.0f; break; // redBear
							case 108: animSpeed = 105.0f; break;// tusk ghost
							case 109: animSpeed = 120.0f; break; //tust skelet
							case 110: animSpeed = 190.0f; break; // ancient dire
							case 111: animSpeed = 120.0f; break;// ancient radiant

							default: animSpeed = 50.0f; break;
							}
						}

						//если это спел мы его не зацикливаем 
						if (PoleHero[x][y].currentState == UnitSetka::SPELL)
						{
							
							//допустим мы хотим чтоб анимация проигрывалась со скоростью 30 кадров в сек
							float spellAnimFrame = PoleHero[x][y].animTime * 30.0f;
							//ограничитель чтоб не уйти за пределы кадров моеди 
							if (spellAnimFrame >= anim.frameCount - 1)
								spellAnimFrame = (float)anim.frameCount - 1;

							UpdateModelAnimation(PoleHero[x][y].modelka, anim, spellAnimFrame);

						}
						else
						{
							float currentAs = 1.0f;
							//если юнит атакует разгоняем его анимацию 
							if (PoleHero[x][y].currentState == UnitSetka::ATTACK)
							{
								currentAs = (1.0f + PoleHero[x][y].bonusAS); // ускоряем только если бьем 
								//(1.0f + PoleHero[x][y].bonusAS) множитель от предметов
							}

							//увеличиваем время анимации именно этого юнита(*animSpeed чтоб кадры быстрее были)
							PoleHero[x][y].animTime += GetFrameTime() * animSpeed * currentAs;

							//зацикливаем по количеству кадров
							PoleHero[x][y].animTime = fmodf(PoleHero[x][y].animTime, (float)anim.frameCount);
							//делает анимацию бесконечной

							//обновляем модель передаем текущий кадр как int
							//связывает скелет модельки с кадром анимации
							UpdateModelAnimation(PoleHero[x][y].modelka, anim, (int)PoleHero[x][y].animTime);
						}
					}

					


					//x координата центра клетки
					/* */
					float px = PoleHero[x][y].worldPos.x;
					// z координата центра клетки (глубина)
					float pz = PoleHero[x][y].worldPos.y;

					//берем маштаб из клетки 
					float PoleScale = PoleHero[x][y].scaleHero;


					//отрисовка конкретной 3д модели 
					DrawModelEx(
						PoleHero[x][y].modelka, //используем индивидуальную копию модели 
						//которая хранится в ячейке сетки PoleHero[x][y].modelka
						{ px, 0.0f,pz }, //поз в центре клетки  Y = 0 потому шо 0 уровень пола 
						{ 0,1,0 }, // ось вращения (вертикально вокруг Y) 
						PoleHero[x][y].rotation, // угол поворота (180 чтоб спиной зырили)
						{ PoleScale,PoleScale,PoleScale }, // размер перса
						WHITE
					);
				}

			}
		}
	}
	EndMode3D();

	for (int x = 0; x < COUNT; x++)
	{
		for (int y = 0; y < COUNT; y++)
		{
			//рисуем звезды клетка активна  и юнгит не мертв
			if (PoleHero[x][y].active && !PoleHero[x][y].isDead && PoleHero[x][y].currentState != UnitSetka::DEAD)
			{
				//берем ссылку на нешего юнита
				UnitSetka& unit = PoleHero[x][y];

				//x координата центра клетки
				/*box.min.x левый край + смешение на колво клеток + половина клеток для центра */
				float px = PoleHero[x][y].worldPos.x;
				// z координата центра клетки 
				float pz = PoleHero[x][y].worldPos.y;

				//получаем экранную позицию ног юнита
				Vector2 screenPos = GetWorldToScreen({ px,0.0f,pz }, camera);

				//настраиваем положение лвлов(звезд чуть ниже ног)
				float centerX = screenPos.x;
				float centerY = screenPos.y + 10.0f;//чуть ниже ног 

				//отрисовка звезд в зависимости от лвлва
				if (PoleHero[x][y].lvl == 1)
				{
					// лвл - 1 = бронзовая звезда 
					DrawTextureEx(star1, { centerX - (star1.width * 0.5f) , centerY }, 0.0f, 1.0f, WHITE);
				}
				else if (PoleHero[x][y].lvl == 2)
				{
					//lvl 2 = 2 star
					DrawTextureEx(star2, { centerX - 25,centerY }, 0.0f, 1.0f, WHITE);
					DrawTextureEx(star2, { centerX + 5,centerY }, 0.0f, 1.0f, WHITE);
				}
				else if (PoleHero[x][y].lvl == 3)
				{
					//3 lvl = 3 star
					DrawTextureEx(star3, { centerX - 45, centerY }, 0.0f, 1.0f, WHITE);
					DrawTextureEx(star3, { centerX - 15, centerY }, 0.0f, 1.0f, WHITE);
					DrawTextureEx(star3, { centerX + 15, centerY }, 0.0f, 1.0f, WHITE);

				}
			}
		}
	}
	//возвращаем в 3д
	BeginMode3D(camera);
}

//spawn summons 
void Setka::SpawnSummons(int id, Vector2 tile, int team, int level, std::map<int, Animation>& library)
{
	//превращаем координаты X,Y из дробных в целое число int для индеса массива 
	int x = (int)tile.x; 
	int y = (int)tile.y;

	if (x < 0 || x >= COUNT || y < 0 || y >= COUNT) return;

	//получаем ссылку на объект в сетке тепеорь summon это клетка (где будут спавнится скелеты)
	UnitSetka& summon = PoleHero[x][y];

	summon.active = true; //активируем юнита (UpdateBattle будет его обрабатывать)
	summon.unitID = id; //записываем айди (например 50 для скелета) чтоб подгрузить модель
	summon.team = team; //ставим за нашу тиму
	summon.lvl = level; //ставим лвл 

	summon.gridPos = tile; // запоминаем поз на доске 



	//присваиваем модель 
	if (library.find(id) != library.end()) {
		summon.modelka = library[id].model;

		summon.scaleHero = library[id].scaleHero * 4.0f;
	}

	//получаем центр клетки 
	Vector3 pos = GetTileWorldPos(x, y);
	summon.worldPos = { pos.x, pos.z }; // записываем X и Z 
	summon.spawnWorldPos = summon.worldPos;

	//заполняем хп урон броню на основе айди юнита (switch в InitializeHeroStats)
	InitializeHeroStats(summon);

	//сразу отправляем его искать ближайшую цель 
	summon.currentState = UnitSetka::SEARCH;
	summon.animTime = 0.0f;
	TraceLog(LOG_INFO, ">>> SUMMON SPAWNED: %s at [%d, %d]", summon.name.c_str(), x, y);
}

Vector2 Setka::FindEmptyTileNear(Vector2 pos) //пустые клетки ищем 
{
	//проходим по всем соседят слева -8   центр 0   справа 8 
	for (int dx = -2; dx <= 2; dx++)
	{
		//проходим по всем соседям сверху -8    центр 0  снизу 8
		for (int dy = -2; dy <= 2; dy++)
		{
			//вычисляем координаты соседей клетки относительно нашего перса 
			int nx = (int)pos.x + dx;
			int ny = (int)pos.y + dy;

			//проверка границ чтоб келет не заспавнился за пределами доски
			if (nx >= 0 && nx < COUNT && ny >= 0 && ny < COUNT)
			{
				//проверяем флаг .active есл он false то клетка пустая 
				if (!PoleHero[nx][ny].active)
				{
					//как только нашли первую свободную клетку  возвращаем ее координаты
					return { (float)nx, (float)ny };
				}
			}
		}
	}
	//если все занято и нет пустой клетки то возвращаем -1 -1 чтоб функция поняла что спавнить нельзя
	return{ -1,-1 };
}


//извлечь (забираем данные и совобождаем клетку)
UnitSetka Setka::ExtractUnit(int x, int y)
{
	UnitSetka temp = PoleHero[x][y]; //копируем данные в руку

	//очищаем клетку на поле чтоб не было дубликатов
	PoleHero[x][y].active = false; //вынимаем героя (клетка пустая)
	PoleHero[x][y].unitID = -1;

	return temp;
}

//вставка ( кладем данные из руки в нью клетку)
void Setka::InsertUnit(int x, int y, UnitSetka unit)
{
	PoleHero[x][y] = unit;
	PoleHero[x][y].active = true; //ставим героя в клетку
	PoleHero[x][y].gridPos = { (float)x, (float)y }; //обеовляем координаты внутри юнита


	//считаем где этот юнит должен стоять в 3д мире изначально
	//получаем границы модели доскии
	BoundingBox box = GetModelBoundingBox(model);

	//считаем координаты центра клетки 
	//Если box.min.x равен 0 (ошибка загрузки) используем -13.0f
	float startX = (box.min.x == 0) ? -13.0f : box.min.x;
	float startZ = (box.min.z == 0) ? -13.0f : box.min.z;

	//-13.0f начало доски (левый край)
	float px = -13.0f + (x * tSize) + (tSize / 2.0f);
	float pz = -13.0f + (y * tSize) + (tSize / 2.0f);

	//Записываем в worldPos
	PoleHero[x][y].worldPos = { px,pz };

	// запоминаем для респавна
	PoleHero[x][y].spawnWorldPos = { px,pz };
}

UnitSetka* Setka::FindNearestEnemy(int attackerX, int attackerY, int attackerTeam)
{
	UnitSetka* closestEnemy = nullptr; // сначала запишем найденную цель
	float minDistance = 9999.0f; // начальная огромная дистанцию для сравнения 

	for (int x = 0; x < COUNT; x++)
	{
		for (int y = 0; y < COUNT; y++)
		{
			UnitSetka& potentialEnemy = PoleHero[x][y]; // берем юнита из клетки

			//если в клетке кто-то есть он не мертв и он из другой команды
			if (potentialEnemy.active && !potentialEnemy.isDead && potentialEnemy.team != attackerTeam)
			{
				/*Vector2Distance вычисляет расстояние между двумя точками по формуле пифагора
				считаем расстояние от атакующешо до потенциальной цели
				используем координаты клеток x y как векторы*/
				float dist = Vector2Distance({ (float)attackerX, (float)attackerY },
					{ (float)x, (float)y });

				//если враг ближе чем любой другой найденный ранее в этом цикле
				if (dist < minDistance)
				{
					minDistance = dist; // обновляем рекорд минимальной дистанции 
					closestEnemy = &potentialEnemy; // запоминаем адрес(указатель) этого врага
				}
			}
		}
	}
	return closestEnemy; // возвращаем самого близкого врага или никого(nullptr)
}

//полоска хп и расположение ее
void Setka::DrawHpHero(Camera3D camera)
{
	float dt = GetFrameTime(); // получаем время кадра для плавной анимации 
	//оно над для выравнивания скорости за 1 сек

	for (int x = 0; x < COUNT; x++) {
		for (int y = 0; y < COUNT; y++) {
			//создаем ссылку на юнита в текущей клетке 
			UnitSetka& unit = PoleHero[x][y];
			//если в клетке никого нет (.active == false) или герой уже мертв 
			//continue скипгем и уйдем в некст
			if (!unit.active || unit.isDead) continue;


			//берем размер одной клетки
			float cellsize = GetTileSIze();

			//привязка хп над бошкой юнита
			Vector3 headpos;

			//считаем позицию
			// координата * размер - (общ размер /2) + (размер 	клетки /2)
			headpos.x = unit.worldPos.x;
			headpos.z = unit.worldPos.y;
			headpos.y = 3.0f; //высота над бошкой


			//переведим 3д в 2д координаты экрана
			Vector2 screenPos = GetWorldToScreen(headpos, camera);

			//логика отнимаения хп  
			//если визуальное хп больше реального то уменьшаем 
			if (unit.stats.lerpHP > unit.stats.hp)
			{
				// 250.0f скорость уменьшения хп 
				unit.stats.lerpHP -= 250.0f * dt;

				// чтоб не проскочить реальное значение
				if (unit.stats.lerpHP < unit.stats.hp) unit.stats.lerpHP = unit.stats.hp;
			}

			//если апнули лвл и хп стало больше то подтягиваем визуал мгновенно 
			else if (unit.stats.lerpHP < unit.stats.hp)
			{
				unit.stats.lerpHP = unit.stats.hp;
			}


			// отрисовка
			float width = 60.0f;
			float height = 8.0f;
			float hpPercent = unit.stats.hp / unit.stats.maxhp; //%hp
			float lerpPercent = unit.stats.lerpHP / unit.stats.maxhp;

			//слой 0 тень/обводка (чуть больше основной полоски)
			DrawRectangle(screenPos.x - width / 2 - 2, screenPos.y - 2, width + 4, height + 4, ColorAlpha(BLACK, 0.8f));

			//слой 1 фон (пустое хп темно серое
			DrawRectangle(screenPos.x - width / 2, screenPos.y, width, height, DARKGRAY);

			//слой 2 красная потеря хп (рисуем lerpPercent)
			if (lerpPercent > hpPercent)
			{
				DrawRectangle(screenPos.x - width / 2, screenPos.y, width * lerpPercent, height, RED);
			}

			//слой 3 основное хп (зеленый)
			DrawRectangle(screenPos.x - width / 2, screenPos.y, width * hpPercent, height, LIME);


			//слой 4 разделы хп(каждые 100  мб потом на 200 -300 поменяю)
			if (unit.stats.maxhp > 0)
			{
				for (int i = 1; i < (int)(unit.stats.maxhp / 100); i++) {
					float xOffset = (width / (unit.stats.maxhp / 100)) * i;
					DrawRectangle(screenPos.x - width / 2 + xOffset, screenPos.y, 1, height, ColorAlpha(BLACK, 0.4f));
				}
			}

		}
	}
}



void Setka::UpdateBattle(int currentRound, ItemManager& items,
	std::map<int, Animation>& library, AllSound& sounds)
{
	float dt = GetFrameTime();

	//проверка конца боя чтоб скелеты мои исчезли 
	bool enemyAlive = false;
	bool playerAlive = false;

	//делаем цикл чтоб пройтись по всей доске
	for (int x = 0; x < COUNT; x++)
	{
		for (int y = 0; y < COUNT; y++)
		{
			UnitSetka& u = PoleHero[x][y];
							//если клетка не занята то скип
				if (!u.active) continue;
			

			//группа 1 призванные скелеты(вк)
			//они всегда исчезают после боя 
			if (u.active && u.currentState != UnitSetka::DEAD)
			{
				//1 енеми  0 игрок
				if (u.team == 1) enemyAlive = true;
				else playerAlive = true;
			}	
		}
	}

	// если хотя бы один из флагов остался false значит бой закончет 
	if (!enemyAlive || !playerAlive)
	{
		// еще раз пеоебегаем по всей доске шоб найти кого делитать
		for (int x = 0; x < COUNT; x++)
		{
			for (int y = 0; y < COUNT; y++)
			{
				UnitSetka& u = PoleHero[x][y];
				//если клетка не занята то скип
				if (!u.active) continue;

				//удаляем скелетов сразу
				if (u.unitID >= 50 && u.unitID < 100)
				{
					//как только active false то перестает движок рисовать 
					//и перестает счиать для него логику в апдейт и он прост исчезает 
					u.active = false;
					u.isDead = false; 
					u.currentState = UnitSetka::IDLE; // сбрасываем стейкт
				}
				//героев и своих и чужим выключаем если они трупы 
				else if (u.currentState == UnitSetka::DEAD || u.isDead)
				{
					u.active = false;
					u.isDead = false;
				}
					
			}
		}
	}


	for (int x = 0; x < COUNT; x++) {
		for (int y = 0; y < COUNT; y++) {
			
			//создаем ссылку на юнита в текущей клетке 
			UnitSetka& unit = PoleHero[x][y];

			//если в клетке никого нет (.active == false) или герой уже мертв 
			//continue скипгем и уйдем в некст
			if (!unit.active || unit.currentState == UnitSetka::DEAD) continue;

			unit.skillTimer += dt;

			//поиск цели
			//если юнит ща в режими поиска противника 
			if (unit.currentState == UnitSetka::IDLE ||
				unit.currentState == UnitSetka::SEARCH)
			{
				//вызываем функцию поиска самого близкого врага
				unit.target = FindNearestEnemy(x, y, unit.team);

				//если функция нашла хоть кого-то (не nullptr)
				if (unit.target != nullptr)
				{
					//переключаем юнита в режим ходьбы(нашли = идем)
					unit.currentState = UnitSetka::MOVE;
				}
			}

			//если юнит ща в режиме мува/атаки значит (у него есть цель)
			if (unit.currentState == UnitSetka::MOVE ||
				unit.currentState == UnitSetka::ATTACK)
			{
				//если цель погиблка пока мы шли или били ее
				if (!unit.target || unit.target->isDead || !unit.target->active)
				{
					//сбрасываем все и заставляем юнита снова искать цель в нест кадре
					unit.currentState = UnitSetka::SEARCH;
					unit.target = nullptr;
					continue; // переходим к некст герою на доске
				}

				if (unit.target != nullptr)
				{
					//растрояние между клетками (0.0 -8.0)
					float dist = Vector2Distance(unit.gridPos, unit.target->gridPos);

					//проверка  если мы уже кастуем то не надо ходить или бить
					if (unit.currentState != UnitSetka::SPELL)
					{
						//если враг дальше чем атак ренжа 
						if (dist > unit.stats.range)
						{
							unit.currentState = UnitSetka::MOVE;

							// Поворот (вычисляем угол чтоб герой смотрел прямо на енеми врага)
							Vector2 dir = { unit.target->gridPos.x - unit.gridPos.x,
											unit.target->gridPos.y - unit.gridPos.y };
							unit.rotation = atan2f(dir.x, dir.y) * RAD2DEG; // поворот в градусах



							//вычисляем вектор направления к цели в мировых координатах 
							//Используем x и y из  Vector2 worldPos
							Vector2 moveDir =
							{
								unit.target->worldPos.x - unit.worldPos.x,
								unit.target->worldPos.y - unit.worldPos.y
							};
							/* это стрелка которая указывает к врагу если враг стоит на 10, 10
							а мы на 2 2 то вектор будет 8 8 */


							//считаем длинну вектора 
							float length = sqrtf(moveDir.x * moveDir.x + moveDir.y * moveDir.y);
							//корень х^2 + y^2 
							/*делаем эту стрелку длинной ровно в 1 единицу чтоб потом умножить ее на нужную скорость*/

							if (length > 0.01f)
							{
								float moveSpeed = 5.0f; //мувспид
								//нормализуем и двигаем
								unit.worldPos.x += (moveDir.x / length) * moveSpeed * dt;
								unit.worldPos.y += (moveDir.y / length) * moveSpeed * dt;
								/*делим компоненты на общую длину moveDir.x / length (теперь у нас чистое направление */

								//синхронизация обновляем логик поз на сетке 
								unit.gridPos.x = (unit.worldPos.x - (-13.0f)) / tSize;
								unit.gridPos.y = (unit.worldPos.y - (-13.0f)) / tSize;
							}
						}

						//если енеми в переделах радиуса тычки 
						else {
							// Поворот (вычисляем угол чтоб герой смотрел прямо на енеми врага)
							Vector2 dir = { unit.target->gridPos.x - unit.gridPos.x,
											unit.target->gridPos.y - unit.gridPos.y };
							unit.rotation = atan2f(dir.x, dir.y) * RAD2DEG; // поворот в градусах

							//проверка спелов 
							//если спел есть и он не пасивка (>0) и она откатилась
							if (unit.hasSpell && unit.skillCoolDown > 0 && unit.skillTimer >= unit.skillCoolDown)
							{
								//заходим в селы если еще не там и не мертвы
								if (unit.currentState != UnitSetka::SPELL && unit.currentState != UnitSetka::DEAD)
								{
									unit.currentState = UnitSetka::SPELL;
									unit.skillTimer = 0.0f; // сброс кд 
									unit.hasDealtDamage = false; // 
									unit.animTime = 0.0f; //сброс анимации для начала каста 
									TraceLog(LOG_INFO, ">>> [%s] CASTING START", unit.name.c_str());
								}
							}
							//логика обычной атаки (заходим сюда если мы не кастуем спелл щас 
							else if (unit.currentState != UnitSetka::SPELL)
							{
								//Обычная атака
								if (unit.currentState != UnitSetka::ATTACK)
								{
									unit.currentState = UnitSetka::ATTACK;
									unit.attackTimer = 0.0f;
								}
							}
						}
					}
				

			
					/*копилка (если юнит смотрит по сторонам и видит тиммейта оч близко он записывает в эту переменную
					надо отодвинутся на 0.2 надо отодвинутся вверх на 0.1*/
					Vector2 pushDir = { 0.0f, 0.0f }; // суммарный вектор отталкивания юнитов дру от друга
					for (int ox = 0; ox < COUNT; ox++)
					{
						for (int oy = 0; oy < COUNT; oy++)
						{
							UnitSetka& otherPpl = PoleHero[ox][oy];

							//скипаем себя врагов и мертвых типов
							if (&otherPpl == &unit) continue;
							if (!otherPpl.active || otherPpl.currentState == UnitSetka::DEAD) continue;
							if (otherPpl.team != unit.team) continue; // ток тиммейты
							//если сам юнит атакует то не двигает его толчком 




							//дистанция в мировых координатах
							/*находим  вектор между персов и его соседом
							если dx положиьтельное = сосед слева значит над вправо идти
							если dx отрицательный то сосед справа надо отойти  влево*/
							float dx = unit.worldPos.x - otherPpl.worldPos.x;
							float dy = unit.worldPos.y - otherPpl.worldPos.y;
							float dist = sqrtf(dx * dx + dy * dy); // пифагор с = корень а^2 + б^2

							//если слишком близки
							float minDist = 1.8f; // минимальная дистанция между юнитами
							if (dist < minDist && dist > 0.001f)
							{
								//добавляем отталкивания
								float strength = (minDist - dist) / minDist;
								/* если сосед далеко (на границе minDist) то strength = 0 я его почти не увижу
								если сосед залет в текстуру юнита то dist почти 0 и strength = 1(мсаксимум и пытаемся
								вытолкнуть)*/
								strength = strength * strength; //сила выталкивавния (мягкое расталкивание)



								//это все для обхода юнитов 
								//если сосед дальше от цели чем сам перс то уменьшаем его влияние
								// это знеачт что тот кто сзади не может пихать того кто впереди
								float myDistToTarget = Vector2Distance(unit.gridPos, unit.target->gridPos);
								float otherDistToTarget = Vector2Distance(otherPpl.gridPos, unit.target->gridPos);

								float power = 1.5f;
								if (otherDistToTarget > myDistToTarget)
								{
									power = 0.2f; // почти не реагируем на толчки в спину
								}

								//(dx / dist) нормализация  а это * power чтоб сильнее расталкивание было сильнее
								pushDir.x += (dx / dist) * strength * power;
								pushDir.y += (dy / dist) * strength * power;
							}
						}
					}

					//применем отталкивание 
					float pushSpeed = 4.0f;
					//если сила выталкивания мелкапя то не двигаем(убираю дрожжание это)
					if (sqrtf(pushDir.x * pushDir.x + pushDir.y * pushDir.y) > 0.01f)
					{
						unit.worldPos.x += pushDir.x * pushSpeed * dt;
						unit.worldPos.y += pushDir.y * pushSpeed * dt;
					}


					//синхронизируем gridPos ( переводим из 3д координаты  шахматную клетку)
					unit.gridPos.x = (unit.worldPos.x - (-13.0f)) / tSize;
					unit.gridPos.y = (unit.worldPos.y - (-13.0f)) / tSize;

					//нанесение урона (тычка)
					if (unit.currentState == UnitSetka::ATTACK)
					{
						unit.attackTimer += dt;


						/* скорость атаки (КАК ЧАСТО УРОН НАНОСИТ) */
						//считаем полную длительность анимации (кадры /60fps)
						float baseAnimTime = 1.25f; //по умолчанию 

						switch (unit.unitID)
						{
						case 0: baseAnimTime = 1.20f; break; // dk  75/1.2 и д
						case 1: baseAnimTime = 1.30f; break; //  wk  157
						case 2: baseAnimTime = 1.23f; break; // jugg  134
						case 3: baseAnimTime = 1.26f; break; // drow  72
						case 4: baseAnimTime = 2.00f; break; // axe 134



							// skelet
						case 50: baseAnimTime = 1.20f; break; // 60 
						case 51: baseAnimTime = 1.30f; break; // 182 
						case 52: baseAnimTime = 1.28f; break; // 154 

							// Enemy
						case 100: baseAnimTime = 1.28f; break; // 154 
						case 101: baseAnimTime = 1.27f; break; // 89 
						case 102: baseAnimTime = 1.29f; break; // 142 
						case 103: baseAnimTime = 1.27f; break; // 99 
						case 104: baseAnimTime = 1.27f; break; // 79 
						case 105: baseAnimTime = 1.26f; break; // 158 
						case 106: baseAnimTime = 1.29f; break; // 142 
						case 107: baseAnimTime = 1.29f; break; // 187 
						case 108: baseAnimTime = 1.30f; break; // 136 
						case 109: baseAnimTime = 1.29f; break; // 155 
						case 110: baseAnimTime = 1.30f; break; // 246 
						case 111: baseAnimTime = 1.28f; break; // 154 


						default:
							if (unit.unitID >= 100) baseAnimTime = 1.28f; //больше 100 1.28
							else baseAnimTime = 1.20f; //меньше 100 1.20
							break;
						}


						// формула чем больше bonusAS тем меньше fullAnimTime (быстрее анимация)
						float fullAnimTime = baseAnimTime / (1.0f + unit.bonusAS);

						//момент  удара
						float hitFactor = 0.5f; // удар на середине


						switch (unit.unitID)
						{

								// hero
							case 0: hitFactor = 0.58f; break; // DK 
							case 1: hitFactor = 0.55f; break; // WK    +
							case 2: hitFactor = 0.40f; break; // Jugg  +
							case 3: hitFactor = 0.35f; break; // Drow  +
							case 4: hitFactor = 0.40f; break; // Axe  +

								// skeleti
							case 50: hitFactor = 0.40f; break; // Зеленый скелет 
							case 51: hitFactor = 0.60f; break; // Красный 
							case 52: hitFactor = 0.50f; break; // 

								// Enemy
							case 100: hitFactor = 0.50f; break; // Зомби  +
							case 101: hitFactor = 0.70f; break; // тьма крип + 
							case 102: hitFactor = 0.60f; break; // синий огр  +
							case 103: hitFactor = 0.45f; break; // свет крип  +
							case 104: hitFactor = 0.55f; break; //  медведь желтый +
							case 105: hitFactor = 0.45f; break; // Голем +
							case 106: hitFactor = 0.10f; break; // свет крип  +
							case 107: hitFactor = 0.25f; break; // медведь КРАСНЫЙ +
							case 108: hitFactor = 0.50f; break; // Туск призрак + 
							case 109: hitFactor = 0.50f; break; // Туск скЕЛЕТ +
							case 110: hitFactor = 0.18f; break; // ТьмаТрон  +
							case 111: hitFactor = 0.17f; break; // СветТрон  +

							default: hitFactor = 0.5f; break;
						}

						float hitMoment = fullAnimTime * hitFactor;
						//наносим урон в момент контакта 
						//!hasDealtDamage гарант что урон нанесется один раз а не каждый кадр
						if (unit.attackTimer >= hitMoment && !unit.hasDealtDamage)
						{
							sounds.PlayHeroAttackSound(unit.unitID);
							

							float finalDamage = unit.stats.damage;
							//пассика для дровки 
							//на всякий проверка если есть цель 
							if (unit.target != nullptr)
							{
								if (unit.unitID == 3 && GetRandomValue(1, 100) <= 20) //20% шанс
								{
									//прокает = игнорим армор 
									unit.target->stats.hp -= finalDamage; 
									// Для отладки:
									TraceLog(LOG_INFO, ">>> DROW: PIERCING SHOT! (Pure Damage: %.1f)", finalDamage);
								}
								else
								{
									//базовая тыка 

									unit.target->stats.Armore(finalDamage);
								}
							}
							unit.hasDealtDamage = true;


							//проверка на  смертельную тычку если после удара у цели хп 0 или меньше
							if (unit.target->stats.hp <= 0)
							{
								unit.target->isDead = true; // помер
								unit.target->currentState = UnitSetka::DEAD;
								unit.target->animTime = 0.0f; // начинаем анимацию смерти с 0 кадра

							}
						}

						//когда таймер дошел до конца  сбрасываем цикл для некст тычки
						if (unit.attackTimer >= fullAnimTime)
						{
							unit.attackTimer = 0.0f;
							unit.hasDealtDamage = false; //Сбрасываем флаг при начале новой атаки
						}
					}
				}
			}
			//проверяем находится ли юнит в состоянии применения спела
			if (unit.currentState == UnitSetka::SPELL)
			{

				//увеличиваем внутрений таймер спела (dt время кадра)
				unit.animTime += dt;

				//задаем длительность каста (подрбрать 1.5) 
				// Настраиваем длительность под каждого героя
				float spellDuration = 2.6f;
				if (unit.unitID == 2) spellDuration = 2.9f; 
				if(unit.unitID == 4) spellDuration = 2.8f;

				//наносим урон когда анимация проигралась на 60% (hit moment)
				//!hasDealtDamage гарант что урон нанесется один раз а не каждый кадр
				if (unit.animTime >= spellDuration * 0.6f && !unit.hasDealtDamage)
				{

					TraceLog(LOG_INFO, ">>> [DEBUG] MOMENT OF SUMMON!"); // Должно появиться в логах

					sounds.PlayHeroSpellSound(unit.unitID);


					switch (unit.unitID)
					{
						case 1: // wk
						{
							int count = unit.lvl; // 1лвл ВК  = 1 скелет на 2 лвлве ВК = 2 скелета 

							for (int i = 0; i < count; i++)
							{
								// ищем свободное место рядом с вк 
								Vector2 spawnTile = FindEmptyTileNear(unit.gridPos);
								if (spawnTile.x != -1)
								{
									//логика айди 
									/*
									i = 0 - id 50
									id 1 = id 51
									id 2 = id 52
									*/
									int summonID = 50 + i;
									TraceLog(LOG_INFO, ">>> SPAWNED SKELETON ID: %d at [%.f, %.f]", summonID, spawnTile.x, spawnTile.y);
									//передаем summonID(какой скелет и unit.lvl какой лвл будет 
									SpawnSummons(summonID, spawnTile, unit.team, unit.lvl,library);
								}
							}
							unit.hasDealtDamage = true;
							break;
						}
						case 2:  // jugg spell krit
						{
							if (unit.target)
							{
								// спелл крит урон множитель урона 2.0 3.0 4.0
								float bonusMult = (unit.lvl == 1) ? 2.0f : (unit.lvl == 2 ? 3.0f : 4.0f);
								unit.target->stats.Armore(unit.stats.damage* bonusMult);

								TraceLog(LOG_INFO, "Juggernaut used Skill! Damage: %f", unit.stats.damage* bonusMult);
							

								//проверка на смерть
								if (unit.target->stats.hp <= 0)
								{
									unit.target->stats.hp = 0;
									unit.target->isDead = true;
									unit.target->currentState = UnitSetka::DEAD;
									unit.target->animTime = 0.0f;
								}
							}
							//урон будет только один раз за всю анимацию
							unit.hasDealtDamage = true; // за текущий взмах нанесли урон 
							break;
						}
						case 4: // axe spell 
						{
							//циклы для осмотра каждой клетки на поле 
							for (int ex = 0; ex < COUNT; ex++)
							{
								for (int ey = 0; ey < COUNT; ey++)
								{
									UnitSetka& enemy = PoleHero[ex][ey];
									//дъют или въаг
									/*
									1 в этой клетке кто=то есть 
									это друг или враг?
									он жив или нет
									*/
									if (enemy.active && enemy.team != unit.team && !enemy.isDead)
									{
										//если враг в радиусе 2 клетки
										/*Vector2Distance вычисляет расстояние между координатами акса и текущего врага*/
										if (Vector2Distance(unit.gridPos, enemy.gridPos) <= 1.8f)
										{
											//крутилка наносит урон больше в 1.2 раза от обычной тычки
											enemy.stats.Armore(unit.stats.damage * 1.2f);


											//проверка на смерть
											if (enemy.stats.hp <= 0)
											{
												enemy.stats.hp = 0;
												enemy.isDead = true;
												enemy.currentState = UnitSetka::DEAD;
												enemy.animTime = 0.0f; // запуска анимации смерти
											}
										}
									}
								}
							}
							//урон будет только один раз за всю анимацию
							unit.hasDealtDamage = true;
							TraceLog(LOG_INFO, ">>> AXE USED COUNTER HELIX!");
							break;
						}
					}
				}
				//завершение спела 
				if (unit.animTime >= spellDuration)
				{
					unit.currentState = UnitSetka::SEARCH; // возвращаем к поиску 
					unit.animTime = 0.0f;
					unit.hasDealtDamage = false;
					TraceLog(LOG_INFO, ">>> UNIT [%s] FINISHED SPELL", unit.name.c_str());
				}
			}
		
		}
	}
}

void Setka::SaveStartPos()
{
	for (int x = 0; x < COUNT; x++)
	{
		for (int y = 0; y < COUNT; y++)
		{
			if (PoleHero[x][y].active)
			{
				// Запоминаем текущие координаты как стартовые
				PoleHero[x][y].spawnWorldPos = PoleHero[x][y].worldPos;
			}
		}
	}
}

void Setka::RespawnUnitsAfterBattle()//respawn
{
	for (int x = 0; x < COUNT; x++)
	{
		for (int y = 0; y < COUNT; y++)
		{
			UnitSetka& unit = PoleHero[x][y];

			if (!unit.active) continue;

			

			//призыв скелетов удаляем всегда 
			if (unit.unitID >= 50 && unit.unitID < 100)
			{
				unit.active = false;
				unit.unitID = -1;
				continue;
			}

			if (unit.team == 0)
			{
				unit.isDead = false; // спавним
				unit.stats.hp = unit.stats.maxhp;// возвращаем фулл хп
				unit.worldPos = unit.spawnWorldPos;// спавним на той же клетке

				// синхронизируем gridPos с worldPos
				// без этого юниты думают что стоят там где файтились а не там где спавнились
				unit.gridPos.x = (unit.worldPos.x - (-13.0f)) / tSize;
				unit.gridPos.y = (unit.worldPos.y - (-13.0f)) / tSize;

				unit.currentState = UnitSetka::IDLE; // возвращаем в идл поз
				unit.target = nullptr;// обнуляем таргет
				unit.attackTimer = 0.0f; //  перезарядка атаки на 0
				unit.hasDealtDamage = false; // сбрасываем флаг урона 
				unit.animTime = 0.0f;//  сброс анимации
				//сброс поворота      (  смотрят на варага изначально)
				unit.rotation = 180.0f;
			}
			//енеми убираем полностью
			else if (unit.active && unit.team == 1)
			{
				unit.active = false;
				unit.unitID = -1;
			}
		}
	}
}

bool Setka::checkEndBattle()
{

	int team0 = 0; // Наши юниты
	int team1 = 0; // противники потом добавить

	for (int x = 0; x < COUNT; x++)
	{
		for (int y = 0; y < COUNT; y++)
		{
			// Юнит считается "в игре", если он активен и НЕ мертв
			if (PoleHero[x][y].active && !PoleHero[x][y].isDead)
			{
				if (PoleHero[x][y].team == 0) team0++;
				else if (PoleHero[x][y].team == 1) team1++;
			}
		}
	}


	// Бой заканчивается, если ХОТЯ БЫ ОДНА команда проиграла а вторая жива
	// или если на поле никогго нема
	if ((team0 > 0 && team1 == 0) || (team1 > 0 && team0 == 0) || (team0 == 0 && team1 == 0))
	{
		return true;
	}
	return false; // файт идет
}

void Setka::SpawmEnemyWave(int round, Shop& shop)
{
	TraceLog(LOG_INFO, "SPAWNING ROUND: %d", round);
	//удаляем всех врагов которые могли остаться
	for (int x = 0; x < COUNT; x++)
	{
		for (int y = 0; y < COUNT; y++)
		{
			if (PoleHero[x][y].team == 1) // если был енеми 
			{

				PoleHero[x][y].active = false; //клетка пуста
				PoleHero[x][y].unitID = -1; //пусто 
				PoleHero[x][y].isDead = false; //спавним
				PoleHero[x][y].target = nullptr; //очищаем таргет
				PoleHero[x][y].currentState = UnitSetka::IDLE;
				//PoleHero[x][y].team = 1; // зона енеми 
			}

		}
	}

	// Защита если раунды кончились ничего не делаем
	if (round > waveRoundsPos.size()) return;

	// Берем настройки для текущего раунда (-1  т к отчет с 0)
	WaveCreeps currentWave = waveRoundsPos[round - 1];

	// Проходимся циклом по всем врагам записанным в конфиге волны
	for (auto& enemyData : currentWave.enemy)
	{
		int id = enemyData.id; //  достаем id 0-дк
		Vector2 pos = enemyData.pos; // достаем координаты например 2 1
		int level = enemyData.lvl; // берем лвла 

		UnitSetka enemyUnit; // Создаем временного юнита
		enemyUnit.unitID = id;
		enemyUnit.lvl = level;
		enemyUnit.team = 1; // team 1 - enemy
		enemyUnit.active = true;
		enemyUnit.isDead = false;
		enemyUnit.currentState = UnitSetka::IDLE;
		enemyUnit.animTime = 0.0f;


		//копируем базовый размер героя  из библиотеки магаза 
		float baseScale = shop.getLibrary()[id].scaleHero;

		//умножаем на маштаб  волны(к примеру 0.5F)
		enemyUnit.scaleHero = baseScale * currentWave.enemyScale;



		//если стоит IsbossWave = true, то делаем его 3 лвла в остальные берем данные лвл из enemydata.lvl 
		if (currentWave.IsbossWave)
		{
			enemyUnit.lvl = 3;
		}
		else
		{
			enemyUnit.lvl = level;  // вот тут будут 1-2 лвла 
		}

		// Копируем 3D-модель из библиотеки магазина по ID
		enemyUnit.modelka = shop.getLibrary()[id].model;

		// Заполняем HP, Урон и Броню на основе уровня
		InitializeHeroStats(enemyUnit);


		// Вызываем твой рабочий метод он посчитает worldPos и воткнет юнита в массив
		//Ставим на поле
		InsertUnit((int)pos.x, (int)pos.y, enemyUnit);

	}
}

