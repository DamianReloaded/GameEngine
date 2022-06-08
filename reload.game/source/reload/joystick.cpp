#include "joystick.h"
#include <string>

namespace reload
{
    static constexpr int JOYSTICK_MAX_AXES = 10;

    joystick::joystick()
    {
        had_at_least_one_device = false;
    }

    joystick::~joystick()
    {

    }

    void joystick::initialize ()
    {
        devices.clear();
    }

    void joystick::update ()
    {
        bool device_error = false;
    }
}
