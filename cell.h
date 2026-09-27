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

#define REFRESH_RATE_IN_HZ 20.0f
#define REFRESH_RATE_IN_MS (1000.0f / REFRESH_RATE_IN_HZ) // 60Hz ((1/60) * 1000)
#define PIXEL_SIZE 24 // Size of the individual pixels of the screen
#define SCREEN_WIDTH_IN_PX  96
#define SCREEN_HEIGHT_IN_PX 48
#define SCREEN_MATRIX_SIZE (SCREEN_HEIGHT_IN_PX * SCREEN_WIDTH_IN_PX)
#define NUM_PX_STATES 2 // On / Off

#define SDL_WINDOW_WIDTH (SCREEN_WIDTH_IN_PX * PIXEL_SIZE)
#define SDL_WINDOW_HEIGHT (SCREEN_HEIGHT_IN_PX * PIXEL_SIZE)

#define R(x) ((x & 0xFF0000) >> 16)
#define G(x) ((x & 0x00FF00) >> 8)
#define B(x)  (x & 0x0000FF)

// ************************************************************
// Enums

typedef enum
{
    PX_OFF = 1U,
    PX_ON  = 0U,
} ScreenPxState;

typedef enum
{
    BG = 0x504945U,
    CELL_ON = 0xFABD2FU,
} Colors;

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
    bool takeStep;
    Uint64 lastTick;
    Uint8 selectedAlgorith;
    float timeAccumulator;
} AppState;

typedef void(*Handler)(AppState *, Uint8 *);

// ************************************************************
// Prototypes

void Convay(AppState *, Uint8 *);
ScreenPxState ProcessCellConvay(AppState *, const Uint32, const Uint32, const Sint8 [9][2], const size_t);
void Rule110(AppState *, Uint8 *);
void Rule30(AppState *, Uint8 *);
void Rule184(AppState *, Uint8 *);
void Process1DAlgorithm(AppState *, Uint8 *, const Uint8 *, const Uint8);

// ************************************************************
// Global Variables

Handler AlgorithmHandlers[10] = {
    Convay,
    Rule110,
    Rule30,
    Rule184,
    Convay,
    Convay,
    Convay,
    Convay,
    Convay,
    Convay,
};

// ************************************************************
#endif