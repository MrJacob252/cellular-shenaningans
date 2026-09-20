#ifndef CELL_C
#define CELL_C

#include "cell.h"

// ************************************************************
// Cell rules

static const Uint8 neighborhood[9][2] = {
//    x   y
    {-1, -1}, { 0, -1}, { 1, -1},
    {-1,  0}, { 0,  0}, { 1,  0},
    {-1,  1}, { 0,  1}, { 1,  1},
};

// ************************************************************
// Process Cell function
ScreenPxState ProcessCell(AppState *state, const Uint32 x, const Uint32 y)
{
    Uint64 location = (y * SCREEN_WIDTH_IN_PX) + x;
    Uint64 newLoc;
    ScreenPxState cellState = state->screen[location] & 1;
    Uint32 i, newX, newY;
    Uint8 numLiveNeighbours;

    size_t numNeighbours = sizeof(neighborhood) / sizeof(neighborhood[0]);

    // Calculate number of live neighbours
    numLiveNeighbours = 0;
    for (i = 0; i < numNeighbours; i++)
    {
        newX = (x + neighborhood[i][0]) % SCREEN_WIDTH_IN_PX;
        newY = (y + neighborhood[i][1]) % SCREEN_HEIGHT_IN_PX;

        newLoc = (newY * SCREEN_WIDTH_IN_PX) + newX;

        if (newLoc != location)
        {
            if (state->screen[newLoc] == PX_ON)
            {
                numLiveNeighbours++;
            }
        }
    }

    // Convay rules (can be simplified, I'm lazy)
    if ((numLiveNeighbours < 2) && (cellState == PX_ON))
    {
        cellState = PX_OFF;
    }
    else if ((numLiveNeighbours >= 2) && (numLiveNeighbours <= 3) && (cellState == PX_ON))
    {
        cellState = PX_ON;
    }
    else if ((numLiveNeighbours > 3) && (cellState == PX_ON))
    {
        cellState = PX_OFF;
    }
    else if ((numLiveNeighbours == 3) && (cellState == PX_OFF))
    {
        cellState = PX_ON;
    }
    
    return cellState;
}

// ************************************************************
// Key handler

SDL_AppResult HandleKeyEventDown(AppState *state, SDL_Scancode keyCode, SDL_EventType eventType)
{
    SDL_AppResult result = SDL_APP_CONTINUE;

    switch (keyCode)
    {
        // Quit
        case SDL_SCANCODE_ESCAPE:
            result = SDL_APP_SUCCESS;
            break;
        // Pause the simulation
        case SDL_SCANCODE_SPACE:
            state->paused = !state->paused;
            // SDL_Log("%d\n", state->paused);
            break;
        default:
            break;
    }

    return result;
}


void TogglePixelType(AppState *state, Uint64 location)
{
    // Left:    Turn pixel on
    // Right:   Turn pixel off
    // Middle:  Toggle pixel state
    switch (state->mouseState.buttonPressed)
    {
        case SDL_BUTTON_LEFT:
            state->screen[location] = PX_ON;
            break;
        case SDL_BUTTON_RIGHT:
            state->screen[location] = PX_OFF;
            break;
        case SDL_BUTTON_MIDDLE:
            state->screen[location] = ~state->screen[location];
            break;
        default:
            break;
    }

    return;
}

SDL_AppResult TogglePixel(AppState *state, float windowX, float windowY)
{
    SDL_AppResult result = SDL_APP_CONTINUE;

    float renderX = 0.0f, renderY = 0.0f;
    Sint32 pxX, pxY;
    Uint64 location;

    // Convert the window coordinates to renderer coordinates in case the window is resized
    if (SDL_RenderCoordinatesFromWindow(state->renderer, windowX, windowY, &renderX, &renderY))
    {
        pxX = (Uint32)(renderX / PIXEL_SIZE);
        pxY = (Uint32)(renderY / PIXEL_SIZE);
        
        // Sanity check that the cursor is in the valid window range
        if (pxX >= 0 && pxX < SCREEN_WIDTH_IN_PX && pxY >= 0 && pxY < SCREEN_HEIGHT_IN_PX)
        {
            // Only update if the location is new
            if (pxX != state->mouseState.lastPxX || pxY != state->mouseState.lastPxY)
            {
                // Switch state
                location = (pxY * SCREEN_WIDTH_IN_PX) + pxX;
                TogglePixelType(state, location);
                // Save the location   
                state->mouseState.lastPxX = pxX;
                state->mouseState.lastPxY = pxY;
            }
        }
    }
    else
    {
        SDL_Log("Failed to calculate renderer coordinates: %s\n", SDL_GetError());
        result = SDL_APP_FAILURE;
    }

    return result;
}

// ************************************************************
// Screen refresh functions

// Get value of specific pixel on the screen
ScreenPxState GetPixelValue(const Uint8 *screen, Uint32 x, Uint32 y)
{
    Uint64 location = (y * SCREEN_WIDTH_IN_PX) + x;
    return (ScreenPxState)(screen[location] & 1);
}

// Set the position of the pixel-to-be-drawn
void SetPixelPosition(SDL_FRect *r, Uint32 x, Uint32 y)
{
    r->x = (float)(x * PIXEL_SIZE);
    r->y = (float)(y * PIXEL_SIZE);
}

void RefreshScreen(AppState *appstate)
{
    SDL_FRect r;
    Uint32 x, y;
    ScreenPxState pxState;

    r.w = r.h = PIXEL_SIZE;
    // Set the drawing color to black and clear the screen
    SDL_SetRenderDrawColor(appstate->renderer, (PX_OFF * 255), (PX_OFF * 255), (PX_OFF * 255), SDL_ALPHA_OPAQUE);
    SDL_RenderClear(appstate->renderer);

    for (y = 0; y < SCREEN_HEIGHT_IN_PX; y++)
    {
        for (x = 0; x < SCREEN_WIDTH_IN_PX; x++)
        {
            pxState = GetPixelValue(appstate->screen, x, y);
            if (pxState == PX_ON) // Draw white rectangle where ON pixel should be
            {
                SetPixelPosition(&r, x, y);
                SDL_SetRenderDrawColor(appstate->renderer, (PX_ON * 255), (PX_ON * 255), (PX_ON * 255), SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(appstate->renderer, &r);
            }
        }
    }

    SDL_RenderPresent(appstate->renderer);
}

// ************************************************************
// SDL Callbacks

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    SDL_AppResult result = SDL_APP_CONTINUE;

    // TODO Metadata

    // Inittialize the SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) && (result == SDL_APP_CONTINUE))
    {
        SDL_Log("Couldn't initialize SDL: %s\n", SDL_GetError());
        result = SDL_APP_FAILURE;
    }

    // Allocate the appstate struct
    AppState *state = (AppState *)SDL_calloc(1, sizeof(AppState));
    SDL_memset(state->screen, PX_OFF, sizeof(state->screen)); // Clear screen
    if ((!state) && (result == SDL_APP_CONTINUE))
    {
        result = SDL_APP_FAILURE;
    }
    *appstate = state;

    // Create window
    if (!SDL_CreateWindowAndRenderer("Cell Automata", SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &state->window, &state->renderer)
        && (result == SDL_APP_CONTINUE))
    {
        result = SDL_APP_FAILURE;
    }
    // To enable scaling of the window while keeping the desired resolution
    SDL_SetRenderLogicalPresentation(state->renderer, SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // Tics
    if (result == SDL_APP_CONTINUE)
    {
        state->lastTick = SDL_GetTicks();
    }

    return result;
}

SDL_AppResult SDL_AppIterate(void *appsate)
{
    SDL_AppResult result = SDL_APP_CONTINUE;

    AppState *state = (AppState *)appsate;
    static Uint8 newScreen[SCREEN_MATRIX_SIZE];
    Uint64 location;
    SDL_memset(newScreen, PX_OFF, sizeof(newScreen)); // New screen state

    Uint64 now = SDL_GetTicks();
    float delta = (float)(now - state->lastTick); // Time since last tick
    state->lastTick = now;
    
    state->timeAccumulator += delta;

    // run game logic if we're at or past the time to run it.
    // if we're _really_ behind the time to run it, run it
    // several times.
    while (state->timeAccumulator >= REFRESH_RATE_IN_MS)
    {
        Uint32 x, y;
        
        if (!state->paused)
        {
            for (y = 0; y < SCREEN_HEIGHT_IN_PX; y++)
            {
                for (x = 0; x < SCREEN_WIDTH_IN_PX; x++)
                {
                    location = (y * SCREEN_WIDTH_IN_PX) + x;
                    newScreen[location] = ProcessCell(state, x, y);
                }
            }
            SDL_memcpy(state->screen, newScreen, sizeof(state->screen));
        }

        state->timeAccumulator -= REFRESH_RATE_IN_MS;
        
        // Refresh screen onece per iteration
    }

    RefreshScreen(state);

    return result;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppState *state = (AppState *)appstate;
    SDL_AppResult result = SDL_APP_CONTINUE;

    switch (event->type)
    {
        case SDL_EVENT_QUIT:
            result = SDL_APP_SUCCESS;
            break;
        case SDL_EVENT_KEY_DOWN:
        // case SDL_EVENT_KEY_UP:
            result = HandleKeyEventDown(state, event->key.scancode, event->key.type);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            // Toggle the initial click
            if (event->button.button == SDL_BUTTON_LEFT || 
                event->button.button == SDL_BUTTON_RIGHT ||
                event->button.button == SDL_BUTTON_MIDDLE)
            {
                state->mouseState.buttonDown = true;
                state->mouseState.buttonPressed = (Sint8)event->button.button;
                // Reset the last coordinates
                state->mouseState.lastPxX = -1;
                state->mouseState.lastPxY = -1;

                result = TogglePixel(state, event->button.x, event->button.y);
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            // Reset the mouse button state
            if (event->button.button == SDL_BUTTON_LEFT || 
                event->button.button == SDL_BUTTON_RIGHT ||
                event->button.button == SDL_BUTTON_MIDDLE)
            {
                state->mouseState.buttonDown = false;
                state->mouseState.buttonPressed = -1;
                state->mouseState.lastPxX = -1;
                state->mouseState.lastPxY = -1;
            }
            break;
        case SDL_EVENT_MOUSE_MOTION:
            // Toggle the motion
            if (state->mouseState.buttonDown)
            {
                result = TogglePixel(state, event->motion.x, event->motion.y);
            }
            break;
        default:
            break;
    }

    return result;
}

void SDL_AppQuit(void *appsate, SDL_AppResult result)
{
    // Clear appsate
    if (appsate != NULL)
    {
        AppState *state = (AppState *)appsate;
        SDL_DestroyRenderer(state->renderer);
        SDL_DestroyWindow(state->window);
        SDL_free(state);
    }
}

#endif