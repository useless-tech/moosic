#pragma once
#include <string>
#include <vector>
#include "miniaudio.h"
namespace moosic
{

	struct MusicInfo
	{
		std::string path;
		std::string name;
		ma_sound sound;
	};

	class MusicQueue
	{
	public:
		MusicQueue() {};


		void push(MusicInfo music_info);
		
		void pop(void);

		void remove_id(uint32_t id);

		std::vector<MusicInfo>& get_queue(void);

		


	private:
		std::vector<MusicInfo> queue;

	};
}