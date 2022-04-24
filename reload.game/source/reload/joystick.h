#pragma once
#include <list>
#include <string>
#include <functional>
namespace reload {

    class joystick
    {
        public:
            typedef std::function<void()>               ev_default;

            struct device
            {
                int id;
                std::string name;

                struct
                {
                    typedef std::function<void(int, bool)> ev_button;
                    typedef std::function<void(int, int, int)> ev_axis;
                    ev_button button;
                    ev_axis   axis;
                } event;
            };

                                    joystick        (const std::string p_message="");
            virtual                 ~joystick       ();

            void                    initialize      ();
            void                    update          ();

            std::list<device>       devices;

            struct
            {
                typedef std::function<void(joystick*)> ev_initialize;
                ev_initialize initialize;
            } event;

        protected:
            bool                    had_at_least_one_device;
    };
}

