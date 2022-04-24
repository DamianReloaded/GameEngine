#include "joystick.h"
#include "string.h"
#include <string>
#include <filesystem>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <cerrno>
#include <sys/ioctl.h>
namespace reload
{
    static constexpr int JOYSTICK_MAX_AXES = 10;

    joystick::joystick(const std::string p_message)
    {
        had_at_least_one_device = false;
    }

    joystick::~joystick()
    {

    }

    struct js_event {
        uint32_t time;	/* event timestamp in milliseconds */
        int16_t value;	/* value */
        uint8_t type;	/* event type */
        uint8_t number;	/* axis/button number */
    };
    #define JSIOCGAXES		_IOR('j', 0x11, uint8_t)				/* get number of axes */
    #define JSIOCGBUTTONS		_IOR('j', 0x12, uint8_t)				/* get number of buttons */
    #define JS_EVENT_BUTTON		0x01	/* button pressed/released */
    #define JS_EVENT_AXIS		0x02	/* joystick moved */
    #define JS_EVENT_INIT		0x80	/* initial state of device */

    int read_event(int fd, struct js_event *event)
    {
        ssize_t bytes;

        bytes = read(fd, event, sizeof(*event));

        if (bytes == sizeof(*event))
            return 0;

        /* Error, could not read full event. */
        return -1;
    }

    size_t get_axis_count(int fd)
    {
        uint8_t axes;

        if (ioctl(fd, JSIOCGAXES, &axes) == -1)
            return 0;

        return axes;
    }

    size_t get_button_count(int fd)
    {
        uint8_t buttons;
        if (ioctl(fd, JSIOCGBUTTONS, &buttons) == -1)
            return 0;

        return buttons;
    }

    struct axis_state {
        short x, y;
    };

    size_t get_axis_state(struct js_event *event, struct axis_state axes[JOYSTICK_MAX_AXES])
    {
        size_t axis = event->number / 2;

        if (axis < JOYSTICK_MAX_AXES)
        {
            if (event->number % 2 == 0)
                axes[axis].x = event->value;
            else
                axes[axis].y = event->value;
        }

        return axis;
    }


    void joystick::initialize ()
    {
        devices.clear();
        std::string devpath = "/dev/input";
        for (const auto& entry : std::filesystem::directory_iterator(devpath))
        {
            std::string item = reload::string::after_last_char(entry.path(),'/');
            if (item.length() > 2)
            {
                if (isdigit(item[2]))
                {
                    std::string prefix = item.substr(0,2);
                    if (prefix=="js")
                    {
                        int js {};
                        js = open(entry.path().c_str(),O_RDONLY| O_NONBLOCK);
                        if (js != -1)
                        {
                            devices.emplace_back();
                            devices.back().id = js;
                            devices.back().name = entry.path();
                            had_at_least_one_device = true;
                        }
                    }
                }
            }
        }
        if (!had_at_least_one_device) return;

        if (event.initialize!=nullptr)
            event.initialize(this);
    }

    void joystick::update ()
    {
        bool device_error = false;

        for (const auto& dev : devices)
        {
            struct js_event event;
            struct axis_state axes[JOYSTICK_MAX_AXES] = {0};
            size_t axis;

            while (read_event(dev.id, &event) == 0)
            {
                switch (event.type)
                {
                    case JS_EVENT_BUTTON:
                        if (dev.event.button!=nullptr) dev.event.button(event.number, event.value);
                        break;
                    case JS_EVENT_AXIS:
                        axis = get_axis_state(&event, axes);
                        if (axis < JOYSTICK_MAX_AXES) if (dev.event.axis!=nullptr) dev.event.axis(axis, axes[axis].x, axes[axis].y);
                        break;
                    default:
                        break;
                }

                fflush(stdout);
            }
            device_error = device_error || (errno==ENODEV);
        }

        // try to reconnect to device. This could be better.
        if ( (device_error || devices.size()==0) && had_at_least_one_device )
        {
            static int count = 0;
            printf("%i\n",count);
            initialize();
            return;
        }
    }
}
