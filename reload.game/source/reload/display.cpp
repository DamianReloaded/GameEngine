#include <reload/implementation/display.h>

namespace reload
{
    display::display()
    {
        imp = std::make_unique<display::implementation>(this);
    }

    display::~display()
    {

    }

    void display::on_start()
    {
        imp->start();
    }

    void display::on_stop()
    {
        imp->stop();
    }

    void display::on_update()
    {
        imp->process_events();
    }

    void display::swap_buffers()
    {
        imp->swap_buffers();
    }

    void display::on_pause()
    {

    }

    void display::on_resume()
    {

    }

    void display::on_create()
    {

    }

    void display::on_destroy()
    {

    }
}
