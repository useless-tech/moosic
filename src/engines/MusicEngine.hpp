#pragma once
#include "miniaudio.h"
#include "MusicQueue.hpp"
namespace moosic
{

	enum class MusicState : int
	{
		PLAYING,
		PAUSED,
		DONE
	};



	class MusicEngine
	{
		public: 
			MusicEngine(moosic::MusicQueue& q);

			void insert(moosic::MusicQueue& queue);




			void play(void);

			void pause(void);

			void restart(void);

			void prev_song(void);

			void start_at(uint32_t id);

			void next_song(void);

			float get_song_duration(void);

			float get_song_time(void);

			void set_song_time(float time);

			void set_song_volume(float volume);

			float get_song_volume(void);

 			uint32_t get_index(void) { return index; }

			moosic::MusicState get_state();

			std::string get_current_song_name(void);

			std::string get_current_song_path(void);
		private:

			moosic::MusicQueue& queue;

			uint32_t index;

			ma_engine engine;

	};
}