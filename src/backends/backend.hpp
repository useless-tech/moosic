#pragma once
#include <string>

namespace moosic
{

class Backend
{
public:
    virtual std::string get_artist() = 0;
    virtual std::string get_album_cover() = 0;
    virtual std::string get_title() = 0;
};

} // namespace moosic
