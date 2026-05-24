#pragma once
#include "raylib.h"

class AllSound
{
public:
	AllSound();
	~AllSound();

	void Load();
    void UnLoad();


    void PlayShopOpen();     // звук открытия магазина
    void PlayShopClose();    // звук закрытия магазина
    void PlayReroll();       // звук рерола героев
    void PlayLockOn();       // звук закрытия замка
    void PlayLockOff();      // звук открытия замка
    void PlayUppEXP();       // звук прокачки поля
    void PlayBuyHero();      //  звук покупки персонажа 

    void PlaySellHero();     //  продажа персонажа 
    void PlayClickHero();    // Клик по юниту 6 звуков
    void PlayPlaceOnField(); // куинуть перса на поле(один из 3)
    void PlaySwapUnit();     // Свап юнитов или шмоток 2 звука
    void PlayMerge1to2();    // Лвл ап с 1 до 2 лвла(2 звука)
    void PlayMerge2to3();    // лвл ап с 2 до 3 (2 звука)

    void PlayEquipItem();    // звук когда выбираешь шмот

    void PlayRoundStart();   // звук когда выводится волна (чередование из 2 звуков)
    void PlaySiren();        // горн (8 сек до BATTLE)
    void PlayMatchLose();    // звук(когда вылезает панелька поражения)
    void PlayWinStreak();    // звук(когда вылезает панелька вина)




    //музыка 
    void PlayPrepareMusic();
    void PlayBattleMusic();
    void UpdateMusic(); // в мейне вызвать чтоб музыка обновлялась 
    void StopAllMusic();
    void setGAmeVolume();


    //для звуков с атакой героя 
    void PlayHeroAttackSound(int unitID);
    //для спелов 
    void PlayHeroSpellSound(int unitID);
private:
    //звуки
    Sound sShopOpen;
    Sound sShopClose;
    Sound sReroll;
    Sound sLockOn;
    Sound sLockOff;
    Sound sUPpEXP;
    Sound sBuyHero;

    Sound sSellHero;
    Sound sClickHero[6];     // массив  6 для клика на юнита 
    Sound sPlaceOnField[3];  //   3 с бенча на поле 
    Sound sSwapUnit[2];      //   2 СВАП
    Sound sMerge1to2[2];     //   2  (с 1го на 2 лвл)
    Sound sMerge2to3[2];     //   2  ( с 2го на 3 лвл)

    Sound sEquipItem;

    Sound sRoundStart[2];    // 2 ( старт раунда)
    Sound sSiren;
    Sound sMatchLose;
    Sound sWinStreak;        

    //это для чередования  индексы
    // Хранит номер звука клика который срабатывает некст  (от 0 до 5)
    int idxClick = 0;    // Хранит номер звука клика который срабатывает некст  (от 0 до 5)
    int idxPlace = 0;   // от 0 до 2
    int idxSwap = 0;   // от 0 до 1 
    int idxMerge12 = 0; // от 0 до 1 
    int idxMerge23 = 0; // от 0 до 1 
    int idxRound = 0; // от 0 до 1



    // music
    Music PrepareBAckMus;
    Music BattleBackMus;


    //рандомные звуки атаки персов 
    Sound AxeAttackSound[3];
    Sound WkAttackSound[3];
    Sound DkAttackSound[3];
    Sound DrowAttackSound[4];
    Sound JuggAttackSound[3];

    // Звуки спеллов
    Sound AxeSpellSound;
    Sound WkSpellSound;
    Sound JuggSpellSound;
    Sound DrowSpellSound;
};