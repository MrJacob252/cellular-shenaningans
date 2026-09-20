#ifndef CELL_H
#define CELL_H
// ************************************************************
// Libraries

#define SDL_MAIN_USE_CALLBACKS 1 // Use callbacks insted of main
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_stdinc.h>

// ************************************************************
// Defines

#define REFRESH_RATE_IN_HZ 60.0f
#define REFRESH_RATE_IN_MS (1000.0f / REFRESH_RATE_IN_HZ) // 60Hz ((1/60) * 1000)
#define PIXEL_SIZE 24 // Size of the individual pixels of the screen
#define SCREEN_WIDTH_IN_PX  64
#define SCREEN_HEIGHT_IN_PX 32
#define SCREEN_MATRIX_SIZE (SCREEN_HEIGHT_IN_PX * SCREEN_WIDTH_IN_PX)
#define NUM_PX_STATES 2 // On / Off

#define SDL_WINDOW_WIDTH (SCREEN_WIDTH_IN_PX * PIXEL_SIZE)
#define SDL_WINDOW_HEIGHT (SCREEN_HEIGHT_IN_PX * PIXEL_SIZE)

// ************************************************************
// Enums

typedef enum
{
    PX_OFF = 1U,
    PX_ON  = 0U,
} ScreenPxState;

// ************************************************************
// Typedef

typedef struct
{
    bool buttonDown;
    Sint8 buttonPressed;
    Sint32 lastPxX; // Last X position in screen pixels
    Sint32 lastPxY; // Last Y position in screen pixels
} MouseState;


typedef struct 
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    Uint8 screen[SCREEN_MATRIX_SIZE];
    MouseState mouseState;
    bool paused;
    Uint64 lastTick;
    float timeAccumulator;
} AppState;

// ************************************************************
// Global Variables

// ************************************************************
// Prototypes

ScreenPxState ProcessCell(AppState *state, Uint32 x, Uint32 y);
// ************************************************************
#endif