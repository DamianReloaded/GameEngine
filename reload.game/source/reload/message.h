#pragma once
#include "identifiable.h"
namespace reload {

    struct imessage : public identifiable
    {

    };

    template<size_t Tid>
    struct message : public imessage
    {
        size_t id() const { return Tid; }
    };
}

