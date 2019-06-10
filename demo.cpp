#include <functional>
#include <iostream>
#include <string>
#include <vector>

// #include <chrono>

#include "SDL.h"
#include "SDL_ttf.h"

using namespace std;

class InputBox {
    // do i need both?
    TTF_Font *font;
    SDL_Color fg_color = {0, 0, 0, SDL_ALPHA_OPAQUE}; // is this a default or is it shared among the the instances?
    string text;

public:
    InputBox();
    ~InputBox();
    void Event(SDL_Event);
    SDL_Surface *Render();
    string GetText();

    function<void(InputBox *)> OnEnter = nullptr;
};

InputBox::InputBox() {
    if (!TTF_WasInit() && TTF_Init() == -1) {
        cout << TTF_GetError() << endl;
    }
    font = TTF_OpenFont("NotoSans-Bold.ttf", 24);
    // OnEnter = ;
}

// vs ::Free()?
InputBox::~InputBox() {
    TTF_CloseFont(font);
    font = NULL;

    // free SDL_Color?
    // OnEnter?
}

string InputBox::GetText() {
    return text;
}

void InputBox::Event(SDL_Event e) {
    switch(e.type) {
    case SDL_TEXTINPUT:
        text.append(e.text.text);
        break;
    case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
        case SDLK_BACKSPACE:
            if (text.length() > 0) {
                text.pop_back();
            }
            break;
        case SDLK_RETURN:
        case SDLK_RETURN2:
        case SDLK_KP_ENTER:
            if (OnEnter != nullptr) {
                OnEnter(this);
            }
            break;
        default:
            break;
        }
    }
}

SDL_Surface *InputBox::Render() {
    SDL_Surface *surface = TTF_RenderText_Blended(font, text.length() > 0 ? text.c_str() : " ", fg_color);
    return surface;
    // TTF_SizeText(font, "foo".c_str(), &(rect.w), &(rect.h));
}


/********************************
 *  An example using InputBox:  *
 ********************************/


class Game {
    // is this private?
    SDL_Window *window;
    SDL_Renderer *renderer;

    InputBox input_box; // i just want to create an empty variable, but it seems to fill it ...

public:
    int ScreenWidth;
    int ScreenHeight;

    Game(int, int);
    void Tick();
    void Draw();
    void Event();
};


Game::Game(int ScreenWidth, int ScreenHeight) :
    ScreenWidth(ScreenWidth),
    ScreenHeight(ScreenHeight)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cerr << "Error: SDL_Init(SDL_INIT_EVERYTHING) failed: " << SDL_GetError() << endl;
        exit(1);
    }

    if (TTF_Init() == -1) {
        std::cerr << "Error: TTF_Init() failed: " << TTF_GetError() << std::endl;
        exit(1);
    }

    window = SDL_CreateWindow("InputBox Demo", 100, 100, ScreenWidth, ScreenHeight,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == nullptr) {
        std::cerr << "Error: SDL_CreateWindow(...) failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, ScreenWidth, ScreenHeight);

    if (renderer == nullptr) {
        std::cerr << "Error: SDL_CreateRenderer(...) failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, NULL); // vs clear
    SDL_RenderPresent(renderer);

    input_box.OnEnter = [](InputBox *ib) -> void { cout << ib->GetText() << endl; };
};


void Game::Draw() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer); // vs fillrect

    SDL_Surface *surface = input_box.Render();
    if (!surface) {
        cout << SDL_GetError() << endl;
        exit(2);
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        cout << SDL_GetError() << endl;
        exit(2);
    }
    free(surface);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    free(texture);

    SDL_RenderPresent(renderer); // ???
}


void Game::Event() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_QUIT:
            exit(0);
            break;
        }

        input_box.Event(event);
    }
}

int main() {
    Game game(1000, 100);

    while (1) {
        game.Event();
        game.Draw();
        // sleep;
    }

    return 0;
}
