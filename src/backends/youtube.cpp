#include <backends/youtube.hpp>
#include <fstream>
#include <httplib.h>

using namespace moosic;

YoutubeBackend::YoutubeBackend(std::string video_id) : video_id(video_id)
{

    httplib::Client cli("https://noembed.com");

    auto resp = cli.Get("/embed?url=http%3A//www.youtube.com/watch%3Fv%3D" + video_id);

    Json::Reader reader;

    if (resp)
    {
        reader.parse(resp->body, json_root);
    }
}

std::string YoutubeBackend::get_artist()
{
    return json_root["author_name"].asString();
}

std::string YoutubeBackend::get_title()
{
    return json_root["title"].asString();
}

std::string YoutubeBackend::get_album_cover()
{
    httplib::Client cli("https://i.ytimg.com");
    auto path = video_id + "_thumbnail.jpg";

    std::fstream new_file(path, std::fstream::out);

    auto resp = cli.Get("/vi/" + video_id + "/hqdefault.jpg");

    if (resp)
    {
        new_file.write(resp->body.c_str(), resp->body.length());
    }

    return path;
}
