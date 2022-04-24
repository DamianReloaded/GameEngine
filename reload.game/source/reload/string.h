#pragma once
#include <vector>
#include <string>
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
}

