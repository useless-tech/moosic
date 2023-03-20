#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"
#include <SDL.h>
#include <cstdio>
#include <httplib.h>
#include <json/json.h>

static constexpr auto window_name = "hello";
static constexpr auto window_width = 1280;
static constexpr auto window_height = 720;

static std::string encode_url(std::string &url)
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = url.begin(), n = url.end(); i != n; ++i)
    {
        std::string::value_type c = (*i);

        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            escaped << c;
            continue;
        }

        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char)c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

static void render_loop(ImGuiIO &io, SDL_Renderer *renderer, SDL_Window *window)
{

    bool show_demo_window = true, done = false;
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

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // Rendering
        ImGui::Render();

        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x,
                           io.DisplayFramebufferScale.y);

        SDL_SetRenderDrawColor(renderer, clear_color.x * 255, clear_color.y * 255, clear_color.z * 255, clear_color.w * 255);

        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }
}

int main(int, char **)
{
    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        std::printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    httplib::Client cli("https://noembed.com");

    std::string video_path = "http://www.youtube.com/watch?v=Qr0-7Ds79zo";
    std::string url = encode_url(video_path);
    auto resp = cli.Get("/embed?url=" + url);

    if (resp)
    {
        auto body = resp->body;

        Json::Value val;
        Json::Reader reader;

        reader.parse(body, val);

        std::printf("%s\n", val["author_name"].asCString());
    }

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

    render_loop(io, renderer, window);

    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
