#pragma once
#include <string>
#include "format.h"
namespace reload {

    struct imessage
    {
        virtual const size_t id() const =0;

        template <class T>
        T* cast()
        {
            static const T ref {};
            return (ref.id()==id())?static_cast<T*>(this):nullptr;
        }
    };

    template<size_t Tid>
    struct message : public imessage
    {
        const size_t id() const override { return Tid; }
    };

    #define reload_declare_message(name, id) struct name : public message<id>

    reload_declare_message(msg_stop_application, 1)
    {
        std::string text;
    };

    reload_declare_message(msg_pause_application, 2)
    {
        int value;

    };
}

