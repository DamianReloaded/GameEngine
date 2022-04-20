#pragma once
#include <vector>
#include <string>
namespace reload::io {
    class memfile
    {
        public:
                                    memfile     ();
            virtual                 ~memfile    ();

            bool                    load        (const std::string& p_path);
            bool                    save        (const std::string& p_path);
            std::vector<uint8_t>    buffer;
    };
}

