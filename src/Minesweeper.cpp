//
//  Minesweeper.cpp
//  Minesweeper
//
//  Created by Zoltán Majoros on 07/Apr/15.
//  Copyright (c) 2015 Zoltán Majoros. All rights reserved.
//

#include "Minesweeper.h"

void Minesweeper::render(int pixelWidth, int pixelHeight)
{
    int mapTileWidth = pixelWidth / map->width;
    int mapTileHeight = pixelHeight / map->height;
    
    for(int tileY=0; tileY < map->width; tileY++)
    {
        for(int tileX=0; tileX < map->height; tileX++)
        {
            SDL_Rect dstRect = { .x = tileX * mapTileWidth,
                                 .y = tileY * mapTileHeight,
                                 .w = mapTileWidth,
                                 .h = mapTileHeight };
            SDL_Rect srcRect;
            
            switch(visibilityMap[tileY * map->width + tileX])
            {
                case MapTileVisibility::covered:
                {
                    srcRect = { .x = 0,
                                .y = 0,
                                .w = spriteSheetTileWidth,
                                .h = spriteSheetTileHeight };
                    break;
                }
                    
                case MapTileVisibility::marked:
                {
                    srcRect = { .x = spriteSheetTileWidth,
                                .y = 0,
                                .w = spriteSheetTileWidth,
                                .h = spriteSheetTileHeight };
                    break;
                }
                    
                case MapTileVisibility::uncovered:
                {
                    int tileValue = map->map[tileY * map->width + tileX];
                    
                    if(tileValue == 0xff)
                    {
                        srcRect = { .x = 2 * spriteSheetTileWidth,
                                    .y = 0,
                                    .w = spriteSheetTileWidth,
                                    .h = spriteSheetTileHeight };
                    }
                    else
                    {
                        srcRect = { .x = 3 * spriteSheetTileWidth + tileValue * spriteSheetTileWidth,
                                    .y = 0,
                                    .w = spriteSheetTileWidth,
                                    .h = spriteSheetTileHeight };
                    }
                    break;
                }
            }
            SDL_RenderCopy(renderer, spriteSheet, &srcRect, &dstRect);
        }
    }
}

// ------------------------------------------------------------

int Minesweeper::input(InputType inputType, int x, int y)
{
    int tileX = x / mapTileWidth;
    int tileY = y / mapTileHeight;
    
    MapTileVisibility *visibilityTile = &visibilityMap[tileY*map->width + tileX];
    
    if(isGameOver)
    {
        int oldMapWidth = map->width;
        int oldMapHeight = map->height;
        
        gameOver();
        newGame(oldMapWidth, oldMapHeight);
        return 1;
    }
    
    switch (inputType)
    {
        case InputType::mark:
        {
            switch (*visibilityTile)
            {
                case MapTileVisibility::marked: // unmark tile
                    *visibilityTile = MapTileVisibility::covered;
                    minesLeft++;
                    printf("Mines left = %d\n", minesLeft);
                    break;
                    
                case MapTileVisibility::covered: // mark tile
                    *visibilityTile = MapTileVisibility::marked;
                    
                    if(minesLeft > 0)
                        minesLeft--;
                    
                    printf("Mines left = %d\n", minesLeft);
                    
                    return checkIfGameIsWon();
                    
                    break;
                    
                case MapTileVisibility::uncovered:
                {
                    if(map->map[tileY*map->width+tileX]==0)
                        break;
                    
                    int markedTilesNearby = 0;
                    
                    for(int j = tileY-1; j <= tileY+1; j++)
                        for(int i = tileX-1; i <= tileX+1; i++)
                        {
                            if(j<0 || j >= map->height || i<0 || i >= map->width || (i == tileX && j == tileY))
                                continue;
                            
                            if(visibilityMap[j * map->width + i] == MapTileVisibility::marked)
                                markedTilesNearby++;
                        }
                    
                    if(map->map[tileY*map->width + tileX] == markedTilesNearby)
                    {
                        for(int j = tileY-1; j <= tileY+1; j++)
                            for(int i = tileX-1; i <= tileX+1; i++)
                            {
                                if(j<0 || j >= map->height || i<0 || i >= map->width || (i == tileX && j == tileY))
                                    continue;
                                
                                if(visibilityMap[j * map->width + i] == MapTileVisibility::marked)
                                    continue;
                                
                                if(input(InputType::step, i * mapTileWidth, j * mapTileHeight))
                                    return 1;
                            }
                    }
                    
                    break;
                }
            }
            break;
        }
        case InputType::step:
        {
            switch (*visibilityTile)
            {
                case MapTileVisibility::marked:
                    break;
                    
                case MapTileVisibility::covered: // step on tile ("uncover")
                    *visibilityTile = MapTileVisibility::uncovered;
                    if(map->map[tileY * map->width + tileX] == 0xff) // stepped on mine?
                    {
                        for(int i = 0; i<map->width * map->height; i++)
                            visibilityMap[i] = MapTileVisibility::uncovered; // show complete map
                        
                        isGameOver = true;
                        won = false;
                        puts("Game over");
                        return 1;
                    }
                    if(map->map[tileY * map->width + tileX] == 0) // if empty, uncover nearby area
                    {
                        autoUncover({tileX,tileY});
                        
                        for(int i = 0; i<map->width * map->height; i++) // clean up after autoUncover()
                            if(map->map[i] == VISITED)
                                map->map[i] = 0;
                    }
                    
                    return checkIfGameIsWon();
                    
                    break;
                case MapTileVisibility::uncovered:
                    break;
            }
            break;
        }
    }
    
    return 0;
}

void Minesweeper::loadSpriteSheet()
{
    if(spriteSheet != nullptr) return;
    
    SDL_RWops *file = nullptr;
    
    if(highDPIScale == 1)
        file = SDL_RWFromFile("images/sprite_sheet.png", "r");
    else
        file = SDL_RWFromFile("images/sprite_sheet2x.png", "r");
    
    if(file == NULL)
    {
        puts("Error while opening file: 'images/sprite_sheet.png'");
        SDL_Quit();
        exit(-1);
    }
    SDL_Surface *surface = IMG_LoadPNG_RW(file);
    spriteSheet = SDL_CreateTextureFromSurface(renderer, surface);
    delete file;
    
    spriteSheetTileWidth = surface->h;
    spriteSheetTileHeight = surface->h;
}

// ------------------------------------------------------------

void Minesweeper::newGame( int mapWidth, int mapHeight)
{
    map = new Map(mapWidth, mapHeight);
    minesLeft = map->numberOfMines;
    
    visibilityMap = new MapTileVisibility[mapWidth*mapHeight];
    
    for(int i = 0; i<mapWidth*mapHeight; i++)
        visibilityMap[i] = MapTileVisibility::covered;
    
    mapTileWidth = pixelWidth / map->width;
    mapTileHeight = pixelHeight / map->height;
    
    isGameOver = false;
    
    printMap();
}

// ------------------------------------------------------------

void Minesweeper::gameOver()
{
    if(map != nullptr) delete map;
    if(visibilityMap != nullptr) delete [] visibilityMap;
    
    map = nullptr;
    visibilityMap = nullptr;
    
    isGameOver = true;
}

void Minesweeper::gameWon()
{
    puts("Game won, yay!");
    won = true;
    isGameOver = true;
}

// ------------------------------------------------------------

int Minesweeper::checkIfGameIsWon()
{
    if(minesLeft == 0)
    {
        for(int i=0; i<(map->width * map->height); i++)
            if(visibilityMap[i] == MapTileVisibility::covered)
                return 0;
        
        gameWon();
        return 1;
    }
    return 0;
}
// ----------------------------------------------------

void Minesweeper::autoUncover(const Coord2d &tileCoord)
{
    if(map->map[tileCoord.y * map->width + tileCoord.x] == VISITED) return; // visited before? quit.
    map->map[tileCoord.y * map->width + tileCoord.x] = VISITED;
    
    for(int j = tileCoord.y-1; j < tileCoord.y+2; j++) // iterate through all neighbouring tiles
        for(int i = tileCoord.x-1; i < tileCoord.x+2; i++)
        {
            if(i<0 || i>map->width-1 || j<0 || j>map->height-1) continue; // off map? next tile.
            
            if(map->map[j*map->width+i] >= 0 && map->map[j*map->width+i] < 6) // numbered tile?
                input(InputType::step, i * mapTileWidth, j * mapTileHeight);  // simulate stepping on it
            
            if(map->map[j*map->width+i] == 0) // empty tile? recursive call on it.
                autoUncover({i,j});
        }
}

// ----------------------------------------------------

void Minesweeper::printMap()
{
    puts("");
    for(int y=0; y<map->height; y++)
    {
        for(int x=0; x<map->width; x++)
        {
            if(map->map[y*map->width + x] == 255)
                printf("* ");
            else if (map->map[y*map->width + x] == 254)
                printf("X ");
            else
                printf("%d ", map->map[y*map->width + x]);
        }
        puts("");
    }
}
