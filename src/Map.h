//
//  Map.h
//  Minesweeper
//
//  Created by Zoltán Majoros on 07/Apr/15.
//  Copyright (c) 2015 Zoltán Majoros. All rights reserved.
//

#include <sys/time.h>
#include <random>

struct Map
{
    Map(int widthInTiles, int heightInTiles) : width(widthInTiles), height(heightInTiles) {}
    
    int width, height;
    int *map = nullptr;
    int numberOfMines = createMap();
    
private:
    enum TileContent
    {
        TILE_EMPTY, TILE_MINE
    };
     
    int createMap()
    {
        // create random number generator, don't mind the initializers
        std::mersenne_twister_engine<std::uint_fast32_t, 32, 624, 397, 31,
        0x9908b0df, 11,
        0xffffffff, 7,
        0x9d2c5680, 15,
        0xefc60000, 18, 1812433253> randomEngine;
        
        timeval time;
        gettimeofday(&time, NULL);
        randomEngine.seed(time.tv_usec); // seed the RNG
        
        TileContent *tmpMap = new TileContent[height*width]; // create temporary array to hold the mine map
        int minesToPlant = width*height / 7;
        const int minesPlanted = minesToPlant;
        
        for(int i=0; i<width*height; i++) // initialize map to empty
            tmpMap[i] = TILE_EMPTY;
        
        while(minesToPlant) // place mines to random locations
        {
            int x = randomEngine() % width;
            int y = randomEngine() % height;
            if(tmpMap[y*width + x] == TILE_MINE)
                continue;
            
            tmpMap[y*width + x] = TILE_MINE;
            minesToPlant--;
        }
        
        assert(map == nullptr);
        map = new int[width * height]; // create game map
        
        for(int y = 0; y<height; y++) // calculate mine distances for every tile
        {
            for(int x = 0; x<width; x++)
            {
                if(tmpMap[y*width + x] == TILE_MINE)
                {
                    map[y*width + x] = 0xff;
                    continue;
                }
                
                map[y*width + x] = 0;
                
                for(int j=y-1; j<=y+1; j++)
                    for(int i=x-1; i<=x+1; i++)
                    {
                        if(i<0 || i>width-1 || j<0 || j>height-1 || (j==y && i==x))
                            continue;
                        
                        map[y*width + x] += tmpMap[j*width + i];
                    }
            }
        }
        
        delete [] tmpMap; // free temporary mine map
        tmpMap = nullptr;
        
        return minesPlanted;
    } // createMap()
}; // class Map
