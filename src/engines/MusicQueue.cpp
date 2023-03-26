#include "MusicQueue.hpp"




std::vector<moosic::MusicInfo>& moosic::MusicQueue::get_queue(void)
{
	return queue;
}

void moosic::MusicQueue::push(moosic::MusicInfo info)
{
	queue.push_back(info);
}

void moosic::MusicQueue::pop(void)
{
	queue.pop_back();
}

void moosic::MusicQueue::remove_id(uint32_t id)
{
	queue.erase(queue.begin() + id);
}