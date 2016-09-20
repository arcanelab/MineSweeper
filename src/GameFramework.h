//
//  game.h
//  Minesweeper
//
//  Created by Zoltán Majoros on 23/Mar/15.
//  Copyright (c) 2015 Zoltán Majoros. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <SDL2/SDL.h>
#include "Minesweeper.h"

struct GameFramework
{
    int tileMapWidth = 16;
    int tileMapHeight = 16;
    
    int tileWidth = 50, tileHeight = 50;
    
    int windowWidth = tileMapWidth * tileWidth / 2;
    int windowHeight = tileMapHeight * tileHeight / 2;
    
    int pixelWidth;
    int pixelHeight;
    
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    double HighDPIScale = 1;
    
    int setupSDL()
    {
        if(SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            puts("Error initializing SDL2.");
            return -1;
        }
        
        if(SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1") == false)
        {
            puts("Could not enable vsync.");
        }
        
        SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
        
        window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  windowWidth, windowHeight, SDL_WINDOW_ALLOW_HIGHDPI);
        if(window == NULL)
        {
            puts("Error while creatign SDL window.");
            SDL_Quit();
            return -1;
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if(window == NULL)
        {
            puts("Error while creatign SDL renderer.");
            SDL_Quit();
            return -1;
        }
        
        SDL_GL_GetDrawableSize(window, &pixelWidth, &pixelHeight);
        HighDPIScale = (double)pixelWidth / (double)windowWidth;
        
        printf("HighDPIScale = %f\n", HighDPIScale);
        
        if(IMG_Init( IMG_INIT_PNG)!= IMG_INIT_PNG)
        {
            puts("Error while initializing SDL2_image.");
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return -1;
        }
        
        return 0;
    }
    
    void cleanupSDL()
    {
        IMG_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    
    // -----------------------------------------------------------
    
    int main()
    {
        if(setupSDL())
            return -1;
        
        Minesweeper minesweeper(renderer, tileMapWidth, tileMapHeight, pixelWidth, pixelHeight, HighDPIScale);
        
        SDL_Event event;
        bool running = true;
        
        while(running)
        {
            while(SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_QUIT:
                        running = false;
                        break;
                    case SDL_WINDOWEVENT:
                        if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                        {
                            SDL_GL_GetDrawableSize(window, &pixelWidth, &pixelHeight);
                        }
                        break;
                    case SDL_MOUSEBUTTONUP:
                        switch(event.button.button)
                        {
                            case SDL_BUTTON_RIGHT:
                                minesweeper.input(Minesweeper::InputType::step, event.button.x * HighDPIScale, event.button.y * HighDPIScale);
                                break;
                            case SDL_BUTTON_LEFT:
                                minesweeper.input(Minesweeper::InputType::mark, event.button.x * HighDPIScale, event.button.y * HighDPIScale);
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }
            SDL_RenderClear(renderer);
            minesweeper.render(pixelWidth, pixelHeight);
            SDL_RenderPresent(renderer);
        }
        
        cleanupSDL();
        
        return 0;
    }
};
