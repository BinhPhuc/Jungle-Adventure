#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include <fstream>
#include <sstream>
#include "graphics.h"
#include "camera.h"

struct TileMap {
    std::vector<std::vector<int>> map;
    SDL_Texture* tileset = nullptr;
    int tileWidth = 64;
    int tileHeight = 64;
    int tilesPerRow = 0;

    void load(Graphics& graphics, const char* tilesetPath, const char* mapPath, int tileW, int tileH) {
        tileWidth = tileW;
        tileHeight = tileH;
        tileset = graphics.loadTexture(tilesetPath);

        int texW;
        SDL_QueryTexture(tileset, NULL, NULL, &texW, NULL);
        tilesPerRow = texW / tileWidth;

        std::ifstream in(mapPath);
        std::string line;
        while (std::getline(in, line)) {
            std::istringstream iss(line);
            int val;
            std::vector<int> row;
            while (iss >> val) row.push_back(val);
            map.push_back(row);
        }
    }

    void render(Graphics& graphics, const Camera& cam) {
        for (int row = 0; row < map.size(); ++row) {
            for (int col = 0; col < map[row].size(); ++col) {
                int id = map[row][col];
                if (id == 0) continue;

                SDL_Rect src;
                src.x = ((id - 1) % tilesPerRow) * tileWidth;
                src.y = ((id - 1) / tilesPerRow) * tileHeight;
                src.w = tileWidth;
                src.h = tileHeight;

                graphics.blitRect(tileset, &src, col * tileWidth - cam.x, row * tileHeight - cam.y);
            }
        }
    }

    int getCols() const {
        return map[0].size();
    }

    void quit() {
        SDL_DestroyTexture(tileset);
    }
};

#endif

