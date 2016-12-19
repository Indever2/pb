#ifndef _GGRAPH_H_
#define _GGRAPH_H_

#include <iostream>
#include <SDL2/SDL.h>

#define PCSIZE 5
#define FS_x 160
#define FS_y 80

class GameTools {

    public:
        GameTools();
        ~GameTools();

        void LoadBitmapToSurface(SDL_Texture * tex, const char * path );
        void ApplySurface(int x, int y, int h, int w, SDL_Texture * tex);
        void InitField();
        void InitGraphics();
        void Refresh();


    private:
        SDL_Window * win;       // Window for graphics
        SDL_Renderer * ren;     // Render
        SDL_Surface * bmp;      // Bitmap image

        // Game textures
        SDL_Texture * interface;
        SDL_Texture * block;
        // functional parts
        int field[FS_y][FS_x];

};

#endif
