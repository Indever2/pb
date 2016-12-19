#include "../include/game.h"

void GameTools::InitField() {
    for (int i = 0; i < FS_y; i++) {
        for (int j = 0; j < FS_x; j++) {
            field[i][j] = 0;
            if (i == 0 || j == 0 || i == FS_y - 1 || j == FS_x - 1)
                field[i][j] = 1;
        }
    }
}
