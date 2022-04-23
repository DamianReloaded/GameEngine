#pragma once
#include "service.h"
#include "tasklist.h"
#include "messagebus.h"
#include "assert.h"
#include "display.h"
namespace reload {

    template <class TApp>
    class application : public service
    {
        public:
            application()
            {
                tasklist.app((TApp*)this);
            }

            virtual ~application()
            {
            }

            virtual void on_start() override
            {
                display.event.closed = [this](){stop();};
                display.start();
                tasklist.start_all();
            }

            virtual void on_stop() override
            {
                tasklist.stop_all();
                display.stop();
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
                display.update();
                tasklist.update();
                display.swap_buffers();
            }

            virtual void on_destroy() override
            {
                tasklist.terminate();
            }

            reload::tasklist<TApp>  tasklist;

            messagebus      messages;
            reload::display display;
        protected:
    };
}

