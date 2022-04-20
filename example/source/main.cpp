#include <iostream>
#include <string>
#include <typeindex>
#include <glew/glew.h>
#include <reload/config/cfg.h>
#include <reload/format.h>
#include <reload/application.h>
#include <reload/message.h>
using namespace std;

class myapp : public reload::application<myapp>
{
    public:
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
        void before_update() override
        {
            std::cout << "myapp before_update.\n";
        }
        void after_update() override
        {
            std::cout << "myapp after_update.\n";
        }
};

class mysubtask : public reload::task<myapp>
{
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
    void before_update() override
    {
        std::cout << "- mytask before_update.\n";
    }
    void after_update() override
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

    glewInit();

    auto msg = new reload::msg_stop_application();
    msg->text = "hello world";
    reload::imessage* imsg = msg;

    std::cout  << " "<< reload::msg_stop_application().id() << std::endl;
    std::cout  << " "<< reload::msg_pause_application().id() << std::endl;
    std::cout  << " "<< imsg->cast<reload::msg_pause_application>() << std::endl;
    std::cout  << " "<< imsg->cast<reload::msg_stop_application>() << std::endl;

    int retval = 0;
    {
        std::unique_ptr<myapp> app = std::make_unique<myapp>();
        app->tasklist.add<mytask>();
        retval = app->run();
    }
    return retval;
}
