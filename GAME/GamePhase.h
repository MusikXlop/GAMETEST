#pragma once
//фаза и логика боя
enum GamePhase {
	PREPARATION,  // подготовка 
	BATTLE, // файт
	FINISH, // конец раунда
	GAMEOVER, // ФУЛЛ ЛУЗ
	CHOICE_ITEMS // ВЫБОР ШМОТОК 
};