#include "MusicEngine.hpp"
#include "MusicQueue.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <iostream>

moosic::MusicEngine::MusicEngine(moosic::MusicQueue &q) : queue(q)
{
    if (ma_engine_init(0, &engine) != MA_SUCCESS)
    {
        // TODO: Logging
        std::cerr << "error: music engine failed to initalized." << std::endl;
        return;
    }
    this->insert(q);
    index = 0;
}

void moosic::MusicEngine::insert(moosic::MusicQueue &queue)
{

    // if the previous queue is not empty, might want to delete it...
    if (this->queue.get_queue().empty())
    {
        for (int i = 0; i < (int)this->queue.get_queue().size(); i++)
            ma_sound_uninit(&this->queue.get_queue()[i].sound);
    }

    this->queue = queue;

    /*
        This streams the file, and is probably what you want without a huge memory overhead, if
        for whatever reason you'd like to decode it, please switch MA_SOUND_FLAG_STEAM to
        MA_SOUND_FLAG_DECODE
    */
    for (int i = 0; i < (int)this->queue.get_queue().size(); i++)
    {
        std::cout << "(" << i << "/" << this->queue.get_queue().size() << ") attempt: load: " << queue.get_queue()[i].path.c_str() << std::endl;
        if (ma_sound_init_from_file(&engine, queue.get_queue()[i].path.c_str(), MA_SOUND_FLAG_ASYNC | MA_SOUND_FLAG_DECODE, 0, 0, &this->queue.get_queue()[i].sound) != MA_SUCCESS)
        {
            // TODO: Logging

            std::cerr << "error: failed to load sound, \"" << queue.get_queue()[i].name << "\", from path \"" << queue.get_queue()[i].path << "\". removed id: " << i << std::endl;
            queue.remove_id(i);
        }
        else
            std::cout << "OK" << std::endl;
    }
}

void moosic::MusicEngine::play(void)
{
    ma_sound_start(&queue.get_queue()[index].sound);
}

void moosic::MusicEngine::pause(void)
{
    ma_sound_stop(&queue.get_queue()[index].sound);
}

void moosic::MusicEngine::restart(void)
{
    set_song_time(0);
    ma_sound_stop(&queue.get_queue()[index].sound);
}

float moosic::MusicEngine::get_song_duration(void)
{
    float length = 0;
    ma_sound_get_length_in_seconds(&queue.get_queue()[index].sound, &length);
    return length;
}

float moosic::MusicEngine::get_song_time(void)
{
    ma_uint64 pcm_cursor;
    if (ma_sound_get_cursor_in_pcm_frames(&queue.get_queue()[index].sound, &pcm_cursor) != MA_SUCCESS)
        std::cerr << "failed to get cursor pcm frame" << std::endl; // TODO: Logging

    uint32_t sample_rate;
    ma_sound_get_data_format(&queue.get_queue()[index].sound, 0, 0, &sample_rate, 0, 0);
    return ((float)pcm_cursor / sample_rate);
}

void moosic::MusicEngine::set_song_time(float time)
{

    uint32_t sample_rate;

    ma_sound_get_data_format(&queue.get_queue()[index].sound, 0, 0, &sample_rate, 0, 0);

    uint32_t pos = time * sample_rate;

    ma_sound_seek_to_pcm_frame(&queue.get_queue()[index].sound, pos);
}

void moosic::MusicEngine::set_song_volume(float volume)
{
    ma_sound_set_volume(&queue.get_queue()[index].sound, volume / 100);
}

float moosic::MusicEngine::get_song_volume(void)
{
    return ma_sound_get_volume(&queue.get_queue()[index].sound);
}

moosic::MusicState moosic::MusicEngine::get_state()
{
    if (ma_sound_at_end(&queue.get_queue()[index].sound))
        return moosic::MusicState::DONE;
    if (ma_sound_is_playing(&queue.get_queue()[index].sound))
        return moosic::MusicState::PLAYING;
    // We can assume the song is paused if it's not at the end, and is not playing.
    return moosic::MusicState::PAUSED;
}
void moosic::MusicEngine::prev_song(void)
{
    restart();
    if (index >= 0)
        index -= 1;
    else // TODO: Proper logging
        std::cerr << "error: there's no song at the index: " << index << std::endl;
}

void moosic::MusicEngine::start_at(int id)
{
    restart();
    if ((size_t)id <= queue.get_queue().size() && id >= 0)
        index = id;
    else // TODO: Proper logging
        std::cerr << "error: there's no song at the id: " << id << std::endl;
}
void moosic::MusicEngine::next_song()
{
    restart();
    if ((size_t)index < queue.get_queue().size())
        index += 1;
    else // TODO: Proper logging
        std::cerr << "error: there's no song at the index: " << index << std::endl;
}

std::string moosic::MusicEngine::get_current_song_name()
{
    return queue.get_queue()[index].name;
}
std::string moosic::MusicEngine::get_current_song_path()
{
    return queue.get_queue()[index].path;
}
