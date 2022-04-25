#include <iostream>
#include <string>
#include <typeindex>
#include <glew/glew.h>
#include <reload/config/cfg.h>
#include <reload/format.h>
#include <reload/application.h>
#include <reload/messages.h>
#include <reload/display.h>
#include <reload/joystick.h>
using namespace std;


class myapp : public reload::application
{
    public:
        RELOAD_GAME_DECLARE_ID(1000);

        reload::display         display;
        reload::joystick        joystick;
        reload::tasklist<myapp> tasklist;

        myapp()
        {
            std::cout << "myapp created.\n";
            tasklist.app((myapp*)this);
        }
        ~myapp()
        {
            std::cout << "myapp deleted.\n";
        }

        virtual void on_destroy() override
        {
            tasklist.terminate();
        }

        virtual void open_display()
        {
            display.event.window.close = [this](){stop();};
            display.event.keyboard.key = [this](uint32_t key, bool pressed)
            {
                if (pressed) std::cout << "key press:" <<key <<"\n"; else std::cout << "key release:" <<key <<"\n";
                if (key==9) stop();
            };
            display.event.keyboard.text = [this](const std::string& _text){ std::cout << "key text:" <<_text<<"\n"; };


            //display.event.pointer.move = [this](int32_t x, int32_t y){ std::cout << "x:" <<x <<",y:"<<y<<"\n"; };
            display.event.pointer.button = [this](int32_t button, bool pressed){ if (pressed) std::cout <<button << " pressed" <<"\n"; else std::cout << button << " released" <<"\n"; };

            display.start();

            joystick.event.initialize = [this](reload::joystick* j)
            {
                for (auto& dev : joystick.devices)
                {
                    dev.event.button = [this](int button, bool pressed){ if (pressed) std::cout <<button << " pressed" <<"\n"; };
                    dev.event.axis = [this](int axis, int x, int y){ std::cout <<axis << " x:" <<x<< ", y:" <<y <<"\n"; };
                }
            };
        }

        virtual void close_display()
        {
            display.stop();
        }

        void on_start() override
        {
            tasklist.start_all();
            std::cout << "myapp started.\n";

        }
        void on_stop() override
        {
            tasklist.stop_all();
            close_display();
            std::cout << "myapp stopped.\n";
        }

        virtual void on_pause() override
        {
            tasklist.pause_all();
        }

        virtual void on_resume() override
        {
            tasklist.resume_all();
        }

        virtual void on_update () override
        {
            tasklist.update();
        }

        void on_update_start() override
        {
            //std::cout << "myapp before_update.\n";
            display.update();
        }
        void on_update_end() override
        {
            //std::cout << "myapp after_update.\n";
            display.swap_buffers();
        }
};

class mysubtask : public reload::task<myapp>
{
public:
    RELOAD_GAME_DECLARE_ID(1001);

    void on_update() override
    {
        //std::cout << "  - mysubtask update" << std::endl;
    }
    void on_destroy() override
    {
        std::cout << "  - mysubtask destroyed.\n";
    }
};

class mytask : public reload::task<myapp>
{
public:

    RELOAD_GAME_DECLARE_ID(1002);

    reload::tasklist<myapp>  tasklist;

    void on_create() override
    {
        std::cout << "- mytask created.\n";
    }
    void on_destroy() override
    {
        std::cout << "- mytask destroyed.\n";
    }
    void on_start() override
    {
        tasklist.app(app());
        tasklist.add<mysubtask>();
        app()->open_display();
        app()->joystick.initialize();
        std::cout << "- mytask started.\n";
    }
    void on_stop() override
    {
        std::cout << "- mytask stopped.\n";
    }
    void on_pause() override
    {
        //std::cout << "- mytask paused.\n";
    }
    void on_resume() override
    {
        //std::cout << "- mytask resumed.\n";
    }
    void on_update_start() override
    {
        //std::cout << "- mytask before_update.\n";
    }
    void on_update_end() override
    {
        //std::cout << "- mytask after_update.\n";
    }
    void on_update() override
    {
        //std::cout << "- mytask update" << std::endl;
        app()->joystick.update();
        tasklist.update();
        //parent()->remove(this);
        //app()->stop();
    }
};

int main()
{
    reload::config::cfg file;
    if (!file.load("../../../data/app.cfg"))
        cout << reload::format("Can't open cfg file. {i}",12) << endl;

    int retval = 0;
    {
        std::unique_ptr<myapp> app = std::make_unique<myapp>();
        reload::idregistry registry;
        registry(myapp::sid);
        registry(mytask::sid);
        app->tasklist.add<mytask>();
        retval = app->run();
    }
    return retval;
}
