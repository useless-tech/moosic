#pragma once
#include <backends/backend.hpp>
#include <json/json.h>

namespace moosic
{

class YoutubeBackend : Backend
{
public:
    YoutubeBackend(std::string video_id);

    std::string get_artist() override;
    std::string get_album_cover() override;
    std::string get_title() override;
    std::fstream get_file() override;

private:
    std::string video_id;
    Json::Value json_root;
};

} // namespace moosic
