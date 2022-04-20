#pragma once
#include <string>
#include <deque>
#include "../error.h"
namespace reload::config
{
    struct property
    {
        property (const std::string p_name="",const std::string p_value="") : name(p_name), value(p_value) {}
        std::string name;
        std::string value;
    };
    struct node
    {
        node(const std::string p_name="") : name(p_name) {}
        std::string             name;
        std::deque<property*>   properties;
        std::deque<node*>       nodes;
    };
    class cfg : public node
    {
        public:
                                    cfg    ();
            virtual                 ~cfg   ();

            bool                    load    (const std::string& p_path);
            bool                    load    (const std::vector<uint8_t>& p_buffer);
            void                    clear   ();
            reload::error           error;

        protected:

    };
}

