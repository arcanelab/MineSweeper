//
//  Minesweeper.h
//  Minesweeper
//
//  Created by Zoltán Majoros on 23/Mar/15.
//  Copyright (c) 2015 Zoltán Majoros. All rights reserved.
//

#pragma once

#include <SDL2/SDL_image.h>
#include <assert.h>
#include <sys/time.h>
#include <random>

/*
 TODO:
 - make room for displaying
   - 'new game' button
   - remaining # of mines
 - make difficulty selectable
 - resize window according to difficulty
 - let the user scale the window
 - add "falsely marked" tile for lost game screen
 - on lost game: show which mine the player stepped on
 */

struct Minesweeper
{
    Minesweeper(SDL_Renderer *renderer, int mapWidth, int mapHeight, int pixelWidth, int pixelHeight, int highDPIScale)
    : renderer(renderer), pixelWidth(pixelWidth), pixelHeight(pixelHeight), highDPIScale(highDPIScale)
    {
        loadSpriteSheet();
        newGame(mapWidth, mapHeight);
    }
    
    ~Minesweeper()
    {
        gameOver();
        SDL_DestroyTexture(spriteSheet);
    }

    enum class InputType
    {
        step, mark
    };
    
    int input(InputType inputType, int x, int y);
    void render(int pixelWidth, int pixelHeight);
    
private:
    struct Coord2d
    {
        int x, y;
    };

    enum class MapTileVisibility
    {
        covered, uncovered, marked
    };

    #include "Map.h"
    
    int pixelWidth, pixelHeight; // size of the window, in pixels
    int spriteSheetTileWidth, spriteSheetTileHeight;
    Map *map = nullptr;
    MapTileVisibility *visibilityMap = nullptr;
    SDL_Texture *spriteSheet = nullptr;
    SDL_Renderer *renderer = nullptr;
    bool isGameOver = false;
    bool won = false;
    int highDPIScale; // init in c'tor
    std::vector<Coord2d> tilesToBeUncovered;
    const int VISITED = 0xfe;
    int mapTileWidth;
    int mapTileHeight;
    int minesLeft = -1;

    void loadSpriteSheet();
    void newGame( int mapWidth, int mapHeight);
    void gameOver();
    void gameWon();
    int checkIfGameIsWon();
    void autoUncover(const Coord2d &tileCoord);
    void printMap();
};
