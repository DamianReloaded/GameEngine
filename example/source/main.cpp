#include <iostream>
#include <string>
#include <typeindex>
#include <glew/glew.h>
#include <reload/config/cfg.h>
#include <reload/format.h>
#include <reload/application.h>
#include <reload/messages.h>
using namespace std;


class myapp : public reload::application<myapp>
{
    public:
        RELOAD_GAME_ID(1000);

        myapp()
        {
            std::cout << "myapp created.\n";
        }
        ~myapp()
        {
            std::cout << "myapp deleted.\n";
        }
        void started() override
        {
            std::cout << "myapp started.\n";
        }
        void stopped() override
        {
            std::cout << "myapp stopped.\n";
        }
        void update_begin() override
        {
            std::cout << "myapp before_update.\n";
        }
        void update_end() override
        {
            std::cout << "myapp after_update.\n";
        }
};

class mysubtask : public reload::task<myapp>
{
public:
    RELOAD_GAME_ID(1001);

    void update() override
    {
        std::cout << "  - mysubtask update" << std::endl;
    }
    void destroyed() override
    {
        std::cout << "  - mysubtask destroyed.\n";
    }
};

class mytask : public reload::task<myapp>
{
public:

    RELOAD_GAME_ID(1002);

    reload::tasklist<myapp>  tasklist;

    void created() override
    {
        std::cout << "- mytask created.\n";
    }
    void destroyed() override
    {
        std::cout << "- mytask destroyed.\n";
    }
    void started() override
    {
        tasklist.app(app());
        tasklist.add<mysubtask>();
        std::cout << "- mytask started.\n";
    }
    void stopped() override
    {
        std::cout << "- mytask stopped.\n";
    }
    void paused() override
    {
        std::cout << "- mytask paused.\n";
    }
    void resumed() override
    {
        std::cout << "- mytask resumed.\n";
    }
    void update_begin() override
    {
        std::cout << "- mytask before_update.\n";
    }
    void update_end() override
    {
        std::cout << "- mytask after_update.\n";
    }
    void update() override
    {
        std::cout << "- mytask update" << std::endl;
        tasklist.update();
        parent()->pause(this);
        parent()->resume(this);
        parent()->remove(this);
        app()->stop();
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
