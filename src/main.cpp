#include "backends/youtube.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"
#include <SDL.h>
#include <SDL_image.h>
#include <cstdio>
#include <httplib.h>
#include <json/json.h>

static constexpr auto window_name = "hello";
static constexpr auto window_width = 1280;
static constexpr auto window_height = 720;

static SDL_Texture *load_texture_from_image(SDL_Renderer *renderer, const char *file)
{
    SDL_Texture *texture = NULL;
    SDL_Surface *surface = IMG_Load(file);
    if (surface)
    {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
    return texture;
}

int main(int, char **)
{
    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        std::printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    moosic::YoutubeBackend youtube_backend("Qr0-7Ds79zo");

    auto song_name = youtube_backend.get_title();
    auto artist = youtube_backend.get_artist();
    auto cover = youtube_backend.get_album_cover();

    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    auto window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    auto *window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, window_flags);
    auto *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return 0;
    }

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    auto &io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    auto texture = load_texture_from_image(renderer, cover.c_str());

    bool done = false;
    ImVec4 clear_color = {0.45f, 0.55f, 0.60f, 1.00f};

    while (!done)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT ||
                (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)))
                done = true;
        }

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin(song_name.c_str());

            ImGui::Image((ImTextureID)texture, ImVec2(480, 360));

            ImGui::End();
        }

        ImGui::Render();

        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x,
                           io.DisplayFramebufferScale.y);

        SDL_SetRenderDrawColor(renderer, clear_color.x * 255, clear_color.y * 255, clear_color.z * 255, clear_color.w * 255);

        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
