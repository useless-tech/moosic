#include "backends/youtube.hpp"

#include "engines/MusicQueue.hpp"
#include "engines/MusicEngine.hpp"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"
#include <SDL.h>
#include <SDL_image.h>
#include <cstdio>
#include <httplib.h>
#include <json/json.h>
#include <filesystem>

static constexpr auto window_name = "moosic";
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


bool get_list_elements_callback(void* data, int n, const char** out_text)
{
    moosic::MusicQueue* queue = (moosic::MusicQueue*)data;
    *out_text = queue->get_queue()[n].name.c_str();
    return true;
}
int main(int, char **)
{
    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        std::printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    moosic::YoutubeBackend youtube_backend("0Xa3lfd0ukk");

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
    ImVec4 clear_color = {22,22,22,255};

    SDL_Point size;
    SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);



    ImVec4* colors = ImGui::GetStyle().Colors;
      colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
      colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
      colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
      colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
      colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
      colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
      colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
      colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
      colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
      colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
      colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
      colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
      colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
      colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
      colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
      colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
      colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
      colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
      colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
      colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
      colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
      colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
      colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
      colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
      colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
      colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
      colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
      colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
      colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
      colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
      colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
      colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
      colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
      colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
      colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
      colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
      colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
      colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

      colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
      colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
      colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
      colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
      colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
      colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
      colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
      colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
      colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
      colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
      colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
      colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
      colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
      colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
      colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

      ImGuiStyle& style = ImGui::GetStyle();
      style.WindowPadding                     = ImVec2(8.00f, 8.00f);
      style.FramePadding                      = ImVec2(5.00f, 2.00f);
      style.CellPadding                       = ImVec2(6.00f, 6.00f);
      style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
      style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
      style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
      style.IndentSpacing                     = 25;
      style.ScrollbarSize                     = 15;
      style.GrabMinSize                       = 10;
      style.WindowBorderSize                  = 1;
      style.ChildBorderSize                   = 1;
      style.PopupBorderSize                   = 1;
      style.FrameBorderSize                   = 1;
      style.TabBorderSize                     = 1;
      style.WindowRounding                    = 7;
      style.ChildRounding                     = 4;
      style.FrameRounding                     = 3;
      style.PopupRounding                     = 4;
      style.ScrollbarRounding                 = 9;
      style.GrabRounding                      = 3;
      style.LogSliderDeadzone                 = 4;
      style.TabRounding                       = 4;

    moosic::MusicQueue queue;




    for (const auto & entry : std::filesystem::directory_iterator(std::filesystem::current_path()))
    {
         

        if (entry.path().extension() == ".mp3") {

            std::string path = entry.path().generic_string();


            moosic::MusicInfo music_info =
            {
                .name = path.substr( (path.find_last_of("/") == 0 ? path.find_last_of("\\") : path.find_last_of("/")) + 1, path.size()-1),
                .path = path
            };  
            queue.push(music_info);

        };
   
    }

    if (queue.get_queue().empty())
    { // TODO logging!
        std::cerr << "attempted to load .mp3 from directory, \"" << std::filesystem::current_path().generic_string() << "\", however there was no music files.\nREMEDY: Place a .mp3 file in the directory \"" << std::filesystem::current_path().generic_string() << "\", and then load this program again.\n";

        return 1;
    }
   moosic::MusicEngine engine(queue);
    engine.set_song_volume(10);



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
            ImGui::Begin(song_name.c_str(), 0, ImGuiWindowFlags_AlwaysAutoResize);
            {
                ImGui::Image((ImTextureID)texture, ImVec2(size.x, size.y));
            }
            ImGui::End();
             static float song_dur = 0;


            ImGui::Begin("Sound Control", 0, ImGuiWindowFlags_AlwaysAutoResize);
            {
                {

                    if (ImGui::Button("Play"))
                        engine.play();

                    ImGui::SameLine();
                    if (ImGui::Button("Pause"))
                        engine.pause();

                    ImGui::SameLine();
                   
                    if (ImGui::SliderFloat("##song_duration", &song_dur, 0, engine.get_song_duration()))
                      engine.set_song_time(song_dur);
                    else if (engine.get_state() != moosic::MusicState::PAUSED)
                        song_dur = engine.get_song_time();
                    ImGui::SameLine();

                    if (ImGui::Button("Next")) {
                        song_dur = 0;
                        engine.next_song();
                        engine.play();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Prev")) {
                        song_dur = 0;
                        engine.prev_song();
                        engine.play();
                    }



                    ImGui::SameLine();
                    static float vol = 10;
                    ImGui::SliderFloat("Volume", &vol, 0, 100);
                    engine.set_song_volume(vol);
                    static bool autoplay = 0;

                    ImGui::SameLine();
                    ImGui::Checkbox("autoplay", &autoplay);

                    if (autoplay)
                    {
                        if (engine.get_state() == moosic::MusicState::DONE)
                        {
                            song_dur = 0;
                            engine.next_song();
                            engine.play();
                        }
                    }

                }


                ImGui::Text(".mp3 loaded from directory, \"%s\"", std::filesystem::current_path().generic_string().c_str());
                
                    static int id = 0;
                    id = engine.get_index();
                    ImGui::PushItemWidth(-1);
                    if (ImGui::ListBox("##songs", &id, &get_list_elements_callback , &queue, queue.get_queue().size()))
                    {

                        song_dur = 0;

                        engine.start_at(id);
                            engine.play();
                    }  
                        
                    ImGui::SameLine(); 
                

            } ImGui::End();

            


        }
        
        ImGui::Render();




        

        

        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x,
                           io.DisplayFramebufferScale.y);

        SDL_SetRenderDrawColor(renderer, clear_color.x  , clear_color.y  , clear_color.z , clear_color.w );

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
