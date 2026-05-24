#include "AllSound.h"

AllSound::AllSound(){}

AllSound::~AllSound()
{
    UnLoad();
}

void AllSound::Load()
{
    sShopOpen = LoadSound("MUSIC/OpenShop/generic_slide_in_01.mp3");
    sShopClose = LoadSound("MUSIC/closeShop/generic_slide_out_01.mp3");
    sReroll = LoadSound("MUSIC/Reroll/reroll.mp3");

    sLockOn = LoadSound("MUSIC/lockbtnShop/shop_lock.mp3");
    sLockOff = LoadSound("MUSIC/OpenBtnShop/shop_unavailable.mp3");

    sUPpEXP = LoadSound("MUSIC/upEXP/purchase_xp.mp3");
    
    sBuyHero = LoadSound("MUSIC/buyUnit/coins.wav"); 

    sSellHero = LoadSound("MUSIC/sellUnit/coins_big.wav"); 

    // клики
    sClickHero[0] = LoadSound("MUSIC/clickONHERO/deploy_unit_01.mp3");
    sClickHero[1] = LoadSound("MUSIC/clickONHERO/deploy_unit_02.mp3");
    sClickHero[2] = LoadSound("MUSIC/clickONHERO/deploy_unit_03.mp3");
    sClickHero[3] = LoadSound("MUSIC/clickONHERO/deploy_unit_04.mp3");
    sClickHero[4] = LoadSound("MUSIC/clickONHERO/deploy_unit_05.mp3");
    sClickHero[5] = LoadSound("MUSIC/clickONHERO/deploy_unit_06.mp3");

    // на поле кидаю
    sPlaceOnField[0] = LoadSound("MUSIC/benchToWorld/bench_unit_01.mp3");
    sPlaceOnField[1] = LoadSound("MUSIC/benchToWorld/bench_unit_02.mp3");
    sPlaceOnField[2] = LoadSound("MUSIC/benchToWorld/bench_unit_03.mp3");

    // свап  лвл апы
    sSwapUnit[0] = LoadSound("MUSIC/SwapUnits/swap_unit_01.mp3");
    sSwapUnit[1] = LoadSound("MUSIC/SwapUnits/swap_unit_02.mp3");

    //  лвл апы
    sMerge1to2[0] = LoadSound("MUSIC/lvlup1to2/unit_combine_01.mp3");
    sMerge1to2[1] = LoadSound("MUSIC/lvlup1to2/unit_combine_opp_01.mp3");
    
    // лвл апы 2
    sMerge2to3[0] = LoadSound("MUSIC/lvlup2to3/unit_combine_02.mp3");
    sMerge2to3[1] = LoadSound("MUSIC/lvlup2to3/unit_combine_opp_02.mp3");

    // выбор шмотки 
    sEquipItem = LoadSound("MUSIC/TakeItems/equip_item.mp3");

    // старт раунда
    sRoundStart[0] = LoadSound("MUSIC/roundsStartBattle/match_ready_01.mp3");
    sRoundStart[1] = LoadSound("MUSIC/roundsStartBattle/match_ready_02.mp3");

    sSiren = LoadSound("MUSIC/8secStartBattle/siren.mp3");
    sMatchLose = LoadSound("MUSIC/loseBattle/match_lose.mp3");
    sWinStreak = LoadSound("MUSIC/winBattle/win_streak.mp3"); 


    //музыка 
    PrepareBAckMus = LoadMusicStream("MUSIC/ThemePREPARE/arena_theme.mp3");
    BattleBackMus = LoadMusicStream("MUSIC/ThemeBattle/heroes_for_hire.mp3");

    //флаг зацикливания 
    PrepareBAckMus.looping = true;  
    BattleBackMus.looping = true;


    //загрузка звуков атаки 
    AxeAttackSound[0] = LoadSound("AttackUnitsSound/axe/attack01.wav");
    AxeAttackSound[1] = LoadSound("AttackUnitsSound/axe/attack02.wav");
    AxeAttackSound[2] = LoadSound("AttackUnitsSound/axe/attack03.wav");

    WkAttackSound[0] = LoadSound("AttackUnitsSound/wk/attack01.wav");
    WkAttackSound[1] = LoadSound("AttackUnitsSound/wk/attack02.wav");
    WkAttackSound[2] = LoadSound("AttackUnitsSound/wk/attack03.wav");

    DkAttackSound[0] = LoadSound("AttackUnitsSound/dk/sword_swing1.wav");
    DkAttackSound[1] = LoadSound("AttackUnitsSound/dk/sword_swing2.wav");
    DkAttackSound[2] = LoadSound("AttackUnitsSound/dk/sword_swing3.wav");

    DrowAttackSound[0] = LoadSound("AttackUnitsSound/drow/arrow_1.wav");
    DrowAttackSound[1] = LoadSound("AttackUnitsSound/drow/arrow_2.wav");
    DrowAttackSound[2] = LoadSound("AttackUnitsSound/drow/arrow_3.wav");
    DrowAttackSound[3] = LoadSound("AttackUnitsSound/drow/arrow_4.wav");

    JuggAttackSound[0] = LoadSound("AttackUnitsSound/jugg/attack01.wav");
    JuggAttackSound[1] = LoadSound("AttackUnitsSound/jugg/attack02.wav");
    JuggAttackSound[2] = LoadSound("AttackUnitsSound/jugg/attack03.wav");

    // ЗАГРУЗКА СПЕЛЛОВ ЗВУКИ
    AxeSpellSound = LoadSound("SpellUnitsSound/axe/counterhelix.wav");
    WkSpellSound = LoadSound("SpellUnitsSound/wk/reincarnate.wav");
    JuggSpellSound = LoadSound("SpellUnitsSound/jugg/sword_swing_special.wav");
    DrowSpellSound = LoadSound("SpellUnitsSound/Drow/marksmanship_target.wav");

    setGAmeVolume();
}

void AllSound::setGAmeVolume()
{

    //фон музыка 

    SetMusicVolume(PrepareBAckMus, 0.25f);
    SetMusicVolume(BattleBackMus, 0.25f);


    // звук атак героев 
    

    // dk
    SetSoundVolume(DkAttackSound[0], 0.70f);
    SetSoundVolume(DkAttackSound[1], 0.70f);
    SetSoundVolume(DkAttackSound[2], 0.70f);


    // wk
    SetSoundVolume(WkAttackSound[0], 0.50f);
    SetSoundVolume(WkAttackSound[1], 0.50f);
    SetSoundVolume(WkAttackSound[2], 0.50f);

    // jugg
    SetSoundVolume(JuggAttackSound[0], 0.70f);
    SetSoundVolume(JuggAttackSound[1], 0.70f);
    SetSoundVolume(JuggAttackSound[2], 0.70f);



    // DROW 
    SetSoundVolume(DrowAttackSound[0], 0.70f);
    SetSoundVolume(DrowAttackSound[1], 0.70f);
    SetSoundVolume(DrowAttackSound[2], 0.70f);
    SetSoundVolume(DrowAttackSound[3], 0.70f);


    // axe
    SetSoundVolume(AxeAttackSound[0], 0.50f);
    SetSoundVolume(AxeAttackSound[1], 0.50f);
    SetSoundVolume(AxeAttackSound[2], 0.50f);






    //звук спелов героев 
    SetSoundVolume(AxeSpellSound, 0.80f); 
    SetSoundVolume(WkSpellSound, 0.80f); 
    SetSoundVolume(JuggSpellSound, 0.80f); 
    SetSoundVolume(DrowSpellSound, 0.80f); 


    // интерфейс и магаз 
    SetSoundVolume(sShopOpen, 0.35f);
    SetSoundVolume(sShopClose, 0.35f);
    SetSoundVolume(sReroll, 0.30f);
    SetSoundVolume(sLockOn, 0.32f);
    SetSoundVolume(sLockOff, 0.32f);
    SetSoundVolume(sUPpEXP, 0.30f);
    SetSoundVolume(sBuyHero, 0.33f);
    SetSoundVolume(sSellHero, 0.30f);
    SetSoundVolume(sEquipItem, 0.30f);


    // клики по персам (6 вариков)

    SetSoundVolume(sClickHero[0], 0.30f);
    SetSoundVolume(sClickHero[1], 0.30f);
    SetSoundVolume(sClickHero[2], 0.30f);
    SetSoundVolume(sClickHero[3], 0.30f);
    SetSoundVolume(sClickHero[4], 0.30f);
    SetSoundVolume(sClickHero[5], 0.30f);


    // перемещение на поле и свапы 

    // кидать на поле с бенча
    SetSoundVolume(sPlaceOnField[0], 0.25f);
    SetSoundVolume(sPlaceOnField[1], 0.25f);
    SetSoundVolume(sPlaceOnField[2], 0.25f);

    // Свапы юнитов шмоток
    SetSoundVolume(sSwapUnit[0], 0.25f);
    SetSoundVolume(sSwapUnit[1], 0.25f);


    // лвл ап 

    // лвл  1 на 2 
    SetSoundVolume(sMerge1to2[0], 0.25f);
    SetSoundVolume(sMerge1to2[1], 0.25f);

    // лвл  2 на 3 
    SetSoundVolume(sMerge2to3[0], 0.25f);
    SetSoundVolume(sMerge2to3[1], 0.25f);


    // систем звуки 

    SetSoundVolume(sRoundStart[0], 0.25f); // начало раунда 
    SetSoundVolume(sRoundStart[1], 0.25f);
    SetSoundVolume(sSiren, 0.30f); // горн за 8 сек
    SetSoundVolume(sMatchLose, 0.35f); // луз звук 
    SetSoundVolume(sWinStreak, 0.35f); // вин звук
}

void AllSound::PlayHeroAttackSound(int unitID)
{
    switch (unitID)
    {
    case 0: PlaySound(DkAttackSound[GetRandomValue(0, 2)]);   break; // dk
    case 1: PlaySound(WkAttackSound[GetRandomValue(0, 2)]);   break; // wk
    case 2: PlaySound(JuggAttackSound[GetRandomValue(0, 2)]); break; // jugg
    case 3: PlaySound(DrowAttackSound[GetRandomValue(0, 3)]); break; // drow
    case 4: PlaySound(AxeAttackSound[GetRandomValue(0, 2)]);  break; // axe
    }
}

void AllSound::PlayHeroSpellSound(int unitID)
{
    switch (unitID)
    {
      //нет спела
    case 0:  break;
    case 1: PlaySound(WkSpellSound);   break; 
    case 2: PlaySound(JuggSpellSound); break; 
    case 3: PlaySound(DrowSpellSound); break; 
    case 4: PlaySound(AxeSpellSound);  break; 
    }
}




void AllSound::UnLoad()
{
    UnloadSound(sShopOpen);  UnloadSound(sShopClose);
    UnloadSound(sReroll);    UnloadSound(sLockOn);
    UnloadSound(sLockOff);   UnloadSound(sUPpEXP);
    UnloadSound(sBuyHero);   UnloadSound(sSellHero);
    UnloadSound(sEquipItem); UnloadSound(sSiren);
    UnloadSound(sMatchLose);
    UnloadSound(sWinStreak);

    // циклами очищаю массив
    for (int i = 0; i < 6; i++) UnloadSound(sClickHero[i]);
    for (int i = 0; i < 3; i++) UnloadSound(sPlaceOnField[i]);
    for (int i = 0; i < 2; i++) UnloadSound(sSwapUnit[i]);
    for (int i = 0; i < 2; i++) UnloadSound(sMerge1to2[i]);
    for (int i = 0; i < 2; i++) UnloadSound(sMerge2to3[i]);
    for (int i = 0; i < 2; i++) UnloadSound(sRoundStart[i]);

    UnloadMusicStream(BattleBackMus);
    UnloadMusicStream(PrepareBAckMus);
}

// вызов саундов
void AllSound::PlayShopOpen() { PlaySound(sShopOpen); }
void AllSound::PlayShopClose() { PlaySound(sShopClose); }
void AllSound::PlayReroll() { PlaySound(sReroll); }
void AllSound::PlayLockOn() { PlaySound(sLockOn); }
void AllSound::PlayLockOff() { PlaySound(sLockOff); }
void AllSound::PlayUppEXP() { PlaySound(sUPpEXP); }
void AllSound::PlayBuyHero() { PlaySound(sBuyHero); }
void AllSound::PlaySellHero() { PlaySound(sSellHero); }
void AllSound::PlayEquipItem() { PlaySound(sEquipItem); }
void AllSound::PlayMatchLose() { PlaySound(sMatchLose); }
void AllSound::PlaySiren() { PlaySound(sSiren); }
void AllSound::PlayWinStreak() { PlaySound(sWinStreak); }

//задний фон музыка 
void AllSound::PlayPrepareMusic() { StopMusicStream(BattleBackMus);  PlayMusicStream(PrepareBAckMus); }

void AllSound::PlayBattleMusic()
{
    StopMusicStream(PrepareBAckMus);
    PlayMusicStream(BattleBackMus);
}

void AllSound::StopAllMusic() { StopMusicStream(BattleBackMus);  StopMusicStream(PrepareBAckMus); }


// для обновы каждый кадр музыки
void AllSound::UpdateMusic()
{
    if (IsMusicStreamPlaying(PrepareBAckMus)) UpdateMusicStream(PrepareBAckMus);
    if (IsMusicStreamPlaying(BattleBackMus)) UpdateMusicStream(BattleBackMus);

}






// чередование звуков 
void AllSound::PlayClickHero()
{
    PlaySound(sClickHero[idxClick]);   // Играем текущий звук из 6 доступных
    idxClick = (idxClick + 1) % 6; // +1 (чтоб нацело делил на 6 и получалось 0 и цикл заново начинался) 
}
void AllSound::PlayPlaceOnField()
{
    PlaySound(sPlaceOnField[idxPlace]); 
    idxPlace = (idxPlace + 1) % 3;     
}
void AllSound::PlaySwapUnit()
{
    PlaySound(sSwapUnit[idxSwap]);      
    idxSwap = (idxSwap + 1) % 2;      
}
void AllSound::PlayMerge1to2()
{
    PlaySound(sMerge1to2[idxMerge12]);  
    idxMerge12 = (idxMerge12 + 1) % 2;  
}
void AllSound::PlayMerge2to3()
{
    PlaySound(sMerge2to3[idxMerge23]);  
    idxMerge23 = (idxMerge23 + 1) % 2; 
}
void AllSound::PlayRoundStart()
{
    PlaySound(sRoundStart[idxRound]);   
    idxRound = (idxRound + 1) % 2;      
}







