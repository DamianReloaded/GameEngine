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

                                    joystick        ();
            virtual                 ~joystick       ();

            void                    initialize      ();
            void                    update          ();

            std::list<device>       devices;
            int                     reinit_retry_attempts_count=0;
            int                     reinit_retry_max_attempts=50;
            int                     reinit_retry_interval = 100;
            int                     reinit_retry_interval_count = 0;

            struct
            {
                typedef std::function<void(joystick*)> ev_initialize;
                ev_initialize initialize;
            } event;

        protected:
            bool                    had_at_least_one_device;

    };
}

