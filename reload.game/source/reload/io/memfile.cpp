#include "memfile.h"
#include <sstream>
#include <fstream>
#include <iterator>
namespace reload::io
{
    memfile::memfile()
    {
        //ctor
    }

    memfile::~memfile()
    {
        //dtor
    }

    bool memfile::load (const std::string& p_path)
    {
        buffer.clear();
        std::ifstream file(p_path.c_str(), std::ios_base::binary);
        if (!file.is_open())
        {
            return false;
        }
        std::streampos sfsize = file.tellg();
        file.seekg( 0, std::ios::end );
        sfsize= file.tellg() - sfsize;
        file.seekg( 0, std::ios::beg );
        file >> std::noskipws; // makes the stream read withespaces
        std::copy(std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>(), std::back_inserter(buffer));
        file.close();
        size_t fsize = (size_t)sfsize;
        if (buffer.size() != fsize)
        {
            return false;
        }
        return true;
    }

    bool memfile::save(const std::string& p_path)
    {
        std::ofstream file(p_path.c_str(), std::ios_base::binary);
        if (!file.is_open())
        {
            return false;
        }
        std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<uint8_t>(file, ""));
        file.close();
        return true;
    }
}
