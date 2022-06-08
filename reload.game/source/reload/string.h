#pragma once
#include <vector>
#include <string>
#include <sstream>
namespace reload::string {
    std::string after_last_char(const std::string& p_string, char p_last_char)
    {
        const size_t pos = p_string.rfind(p_last_char);
        if (std::string::npos != pos)
        {
            return p_string.substr(pos+1, p_string.length()-(pos+1));
        }
        return "";
    }

    template <typename T> T to_number( const std::string& _str)
    {
        std::stringstream converter;
        T value = 0;
        converter << _str;
        converter >> value;
        return value;
    }
}

