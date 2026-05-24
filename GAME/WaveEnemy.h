#pragma once

#include <vector>
#include <string>
#include "raylib.h"


struct InfioWaves
{
    int id;
    Vector2 pos;
    int lvl;
};
struct WaveCreeps
{
    std::string waveName;

    //id unit and pos in the grid x y
    std::vector<InfioWaves> enemy;
    bool IsbossWave = false;// true = сразу на 3 лвл перекидываем перса
    float enemyScale = 1.0f; // размер енеми
};

// глобал таблица волн
inline std::vector<WaveCreeps> waveRoundsPos =
{

    //{   // Раунд  (айди 100) поз 2 1  lvl2   pos  lvl   boss or not and scale
    //    "Creep Wave 1",
    //    { {111, {2, 1}, 1},{110, {4, 1}, 1},{109, {5, 1}, 1} }, // (айди 100) поз 2 1  lvl2   pos  lvl
    //    false, // boss or not
    //    3.0f
    //},

    
  {
        "Weak Zombie",
        { {100, {3, 1}, 1} },
        false, 3.0f
    },
    {
        "Zombie Pair",
        { {100, {2, 1}, 1}, {100, {5, 1}, 1} },
        false, 3.0f
    },
    {
        "BOSS: The First Undying",
        { {100, {3, 1}, 1} },
        true, 4.8f
    },

    {
        "Lane Creeps", 
        { {101, {3, 1}, 1}, {103, {2, 1}, 1}, {106, {4, 1}, 1}, {103, {5, 1}, 1} },
        false, 3.0f
    },
    {
        "Forest Ambush", 
        { {104, {2, 1}, 2}, {104, {4, 1}, 2} },
        false, 3.2f
    },
    {
        "Wild Pack", 
        { {104, {3, 1}, 1}, {101, {1, 1}, 1}, {103, {6, 1}, 1} },
        false, 3.0f
    },
    {
        "BOSS: Wild Hellbear", 
        { {104, {3, 1}, 1}, {100, {2, 0}, 2}, {100, {4, 0}, 1} },
        true, 5.2f
    },


    {
        "Ghost & Skeleton Cavalry", 
        { {108, {1, 1}, 2}, {109, {3, 1}, 2}, {108, {5, 1}, 2} },
        false, 3.0f
    },
    {
        "Ogre's Elite Guard", 
        { {102, {3, 1}, 2}, {101, {2, 0}, 2}, {103, {4, 0}, 2}, {102, {5, 1}, 2} },
        false, 3.2f
    },
    {
        "Swamp Threat",
        { {102, {2, 1}, 2}, {108, {3, 1}, 2}, {102, {5, 1}, 2} },
        false, 3.2f
    },
    {
        "BOSS: Ghost Captain", 
        { {108, {3, 1}, 3}, {109, {2, 1}, 2}, {109, {4, 1}, 2}, {101, {3, 0}, 2} },
        true, 5.2f
    },

    {
        "Ancient Vanguard", 
        { {110, {2, 1}, 2}, {105, {3, 1}, 2}, {111, {4, 1}, 2}, {105, {5, 1}, 2} },
        false, 3.5f
    },
    {
        "Granite Golems Charge", 
        { {105, {2, 1}, 3}, {105, {4, 1}, 3}, {107, {3, 0}, 2} },
        false, 3.8f
    },
    {
        "Rock-Solid Line", 
        { {105, {1, 1}, 3}, {105, {3, 1}, 3}, {105, {5, 1}, 3} },
        false, 3.6f
    },
    {
        "BOSS: Ancient Brothers", 
        { {110, {2, 1}, 3}, {111, {4, 1}, 3}, {103, {3, 0}, 3} },
        true, 5.8f
    },


    {
        "Unstoppable Ghost Horde", // Раунд 16
        { {108, {0, 1}, 3}, {109, {2, 1}, 2}, {109, {4, 1}, 2}, {108, {7, 1}, 3} },
        false, 3.4f
    },
    {
        "Granite Fortress", 
        { {105, {1, 1}, 3}, {105, {3, 1}, 3}, {105, {5, 1}, 3}, {102, {3, 0}, 3} },
        false, 3.8f
    },
    {
        "The Eternal Night", 
        { {100, {1, 1}, 3}, {109, {2, 1}, 3}, {105, {3, 1}, 3}, {109, {4, 1}, 3}, {100, {6, 1}, 3} },
        false, 3.6f
    },
    {
        "FINAL SHOWDOWN: The World Ender", 
        { {110, {3, 1}, 3}, {105, {1, 0}, 3}, {111, {2, 0}, 3}, {111, {4, 0}, 3}, {105, {6, 0}, 3} },
        true, 6.5f
    }
};


