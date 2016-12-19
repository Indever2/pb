#include "../include/game.h"

void GameTools::LoadBitmapToSurface(SDL_Texture * tex, const char * path) {

    SDL_Surface * bmp;
    if ((bmp = SDL_LoadBMP(path)) == nullptr) {
        std::cout << "SDL_LoadBMP error: " << SDL_GetError() << std::endl
        << "Target image : " << path << std::endl;
        exit(-1);
    }
    if ((tex = SDL_CreateTextureFromSurface(ren, bmp)) == nullptr) {
        std::cout << "SDL_CreateTextureFromSurface error: " << SDL_GetError() << std::endl;
        exit(-1);
    }
    SDL_FreeSurface(bmp);

}

void GameTools::ApplySurface(int x, int y, int w, int h, SDL_Texture * tex) {

    SDL_Rect pos;
    pos.x = x;
    pos.y = y;
    pos.h = h;
    pos.w = w;
    SDL_RenderCopy(ren, tex, NULL, &pos);

}

void GameTools::InitGraphics() {

    LoadBitmapToSurface(interface, "img/Bckgr.bmp");
    LoadBitmapToSurface(block, "img/block.bmp");
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, NULL, NULL, NULL);
    SDL_RenderPresent(ren);

}

void GameTools::Refresh() {

    ApplySurface(0, 0, FS_x * PCSIZE, FS_y * PCSIZE, interface);

    for (int i = 0; i < FS_y; i++) {
        for (int j = 0; j < FS_x; j++) {
            switch (field[i][j]) {
                case 0:
                    break;
                case 1:
                    std::cout << "block found! pos = " << i << ":" << j << std::endl;
                    ApplySurface(j * PCSIZE, i * PCSIZE, PCSIZE, PCSIZE, block);
            }
        }
    }
    std::cout << "Refreshing done!" << std::endl;
    SDL_RenderPresent(ren);
    SDL_Delay(500);

}

GameTools::GameTools() {

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "SDL_Init error." << std::endl;
        exit(-1);
    }

    if ((win = SDL_CreateWindow("Devide Like Master", 100, 100,
        FS_x * PCSIZE, FS_y * PCSIZE, SDL_WINDOW_BORDERLESS)) == nullptr) {
        std::cout << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        exit(-1);
    }

    if ((ren = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == nullptr) {
        std::cout << "SDL_CreateRenderer error:" << SDL_GetError() << std::endl;
        exit(-1);
    }

    std::cout << "Initializing graphics..." << std::endl;
    InitGraphics();
    SDL_SetRenderDrawColor(this->ren, 255, 0, 0, 255);
    std::cout << "Initializing game field..." << std::endl;
    InitField();
    Refresh();

}

GameTools::~GameTools() {

    std::cout << "Running destructor" << std::endl;

    SDL_DestroyTexture(interface);
    SDL_DestroyTexture(block);

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

}
