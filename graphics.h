#ifndef _GRAPHICS__H
#define _GRAPHICS__H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "defs.h"
#include <vector>

struct Sprite {
    SDL_Texture* texture = nullptr;
    std::vector<SDL_Rect> clips;
    int currentFrame = 0;
    Uint32 lastFrameTime = 0;
    int frameDelay = 100;

    void init(SDL_Texture* _texture, int frames, const int _clips[][4]) {
        texture = _texture;
        clips.clear();
        SDL_Rect clip;
        for (int i = 0; i < frames; i++) {
            clip = { _clips[i][0], _clips[i][1], _clips[i][2], _clips[i][3] };
            clips.push_back(clip);
        }
    }

    void initAuto(SDL_Texture* _texture, int frameWidth, int frameHeight, int frameCount, int startX = 0, int startY = 0) {
        texture = _texture;
        clips.clear();
        for (int i = 0; i < frameCount; i++) {
            SDL_Rect clip = { startX + i * frameWidth, startY, frameWidth, frameHeight };
            clips.push_back(clip);
        }
    }

    void initGrid(SDL_Texture* _texture, int frameWidth, int frameHeight, int frameCount, int row, int startX = 0, int startY = 0) {
        texture = _texture;
        clips.clear();
        for (int i = 0; i < frameCount; i++) {
            int x = startX + i * frameWidth;
            int y = startY + row * frameHeight;
            SDL_Rect clip = { x, y, frameWidth, frameHeight };
            clips.push_back(clip);
        }
    }

    void tick() {
        currentFrame = (currentFrame + 1) % clips.size();
    }

    void tickTimed(Uint32 currentTime) {
        if (currentTime - lastFrameTime > (Uint32)frameDelay) {
            currentFrame = (currentFrame + 1) % clips.size();
            lastFrameTime = currentTime;
        }
    }

    const SDL_Rect* getCurrentClip() const {
        return &clips[currentFrame];
    }
    int getWidth() const {
        return getCurrentClip()->w;
    }
    int getHeight() const {
        return getCurrentClip()->h;
    }
};

struct Graphics {
    SDL_Renderer *renderer;
	SDL_Window *window;

	void logErrorAndExit(const char* msg, const char* error)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
        SDL_Quit();
    }

	void init() {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
            logErrorAndExit("SDL_Init", SDL_GetError());

        window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        //full screen
        //window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
        if (window == nullptr) logErrorAndExit("CreateWindow", SDL_GetError());

        if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
            logErrorAndExit( "SDL_image error:", IMG_GetError());

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
                                              SDL_RENDERER_PRESENTVSYNC);
        if (renderer == nullptr) logErrorAndExit("CreateRenderer", SDL_GetError());
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        if (TTF_Init() == -1) {
            logErrorAndExit("SDL_ttf could not initialize! SDL_ttf Error: ",
                             TTF_GetError());
        }
        if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
        {
            logErrorAndExit( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        }
    }

	void prepareScene(SDL_Texture * background = nullptr)
    {
        SDL_RenderClear(renderer);
        if (background != nullptr) SDL_RenderCopy( renderer, background, NULL, NULL);
    }

    void presentScene()
    {
        SDL_RenderPresent(renderer);
    }

    SDL_Texture *loadTexture(const char *filename)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

        SDL_Texture *texture = IMG_LoadTexture(renderer, filename);
        if (texture == NULL)
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Load texture %s", IMG_GetError());
        return texture;
    }

    void renderTexture(SDL_Texture *texture, int x, int y)
    {
        SDL_Rect dest;

        dest.x = x;
        dest.y = y;
        SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }

    void blitRect(SDL_Texture *texture, SDL_Rect *src, int x, int y)
    {
        SDL_Rect dest;
        dest.x = x;
        dest.y = y;
        dest.w = src->w;
        dest.h = src->h;
        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }
    void quit()
    {
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    TTF_Font* loadFont(const char* path, int size)
    {
        TTF_Font* gFont = TTF_OpenFont( path, size );
        if (gFont == nullptr) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Load font %s", TTF_GetError());
        }
        return gFont;
    }

    SDL_Texture* renderText(const char* text, TTF_Font* font, SDL_Color textColor)
    {
        SDL_Surface* textSurface = TTF_RenderText_Solid( font, text, textColor );
        if( textSurface == nullptr ) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Render text surface %s", TTF_GetError());
            return nullptr;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, textSurface );
        if( texture == nullptr ) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Create texture from text %s", SDL_GetError());
        }

        SDL_FreeSurface( textSurface );
        return texture;
    }
    Mix_Music *loadMusic(const char* path)
    {
        Mix_Music *gMusic = Mix_LoadMUS(path);
        if (gMusic == nullptr) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                           "Could not load music! SDL_mixer Error: %s", Mix_GetError());
        }
        return gMusic;
    }
    void play(Mix_Music *gMusic)
    {
        if (gMusic == nullptr) return;

        if (Mix_PlayingMusic() == 0) {
            Mix_PlayMusic( gMusic, -1 );
        }
        else if( Mix_PausedMusic() == 1 ) {
            Mix_ResumeMusic();
        }
    }
    Mix_Chunk* loadSound(const char* path) {
        Mix_Chunk* gChunk = Mix_LoadWAV(path);
        if (gChunk == nullptr) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                       "Could not load sound! SDL_mixer Error: %s", Mix_GetError());
        }
        return gChunk;
    }
    void play(Mix_Chunk* gChunk) {
        if (gChunk != nullptr) {
            Mix_PlayChannel( -1, gChunk, 0 );
        }
    }
    void renderSprite(int x, int y, Sprite& sprite, bool flip, float scale = 1.0f, int offsetX = 0, int offsetY = 0) {
        SDL_Rect src = sprite.clips[sprite.currentFrame];
        SDL_Rect dst = {
            x + offsetX,
            y + offsetY,
            static_cast<int>(src.w * scale),
            static_cast<int>(src.h * scale)
        };
        SDL_RenderCopyEx(renderer, sprite.texture, &src, &dst, 0, nullptr, flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }

};

#endif
