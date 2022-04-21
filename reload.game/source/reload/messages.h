#pragma once
#include "message.h"
namespace reload::msg {
    struct application_start : public message<1>
    {
        std::string text;
    };

    struct application_pause : public message<2>
    {
        int value;
    };

    struct application_resume : public message<2>
    {
        int value;
    };
}

