#pragma once
#include "service.h"
#include "tasklist.h"
#include "messagebus.h"
#include "assert.h"
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

            virtual void started()
            {
                tasklist.start_all();
            }

            virtual void stopped()
            {
                tasklist.stop_all();
            }

            virtual void paused()
            {
            }

            virtual void resumed()
            {
            }

            virtual void created()
            {
            }

            virtual void destroyed()
            {
            }

            virtual void update_begin()
            {
            }
            virtual void update()
            {
            }
            virtual void update_end()
            {
            }

            int run ()
            {
                started();
                m_running = true;
                while (m_running)
                {
                    if (!m_paused)
                    {
                        update_begin();
                        update();
                        tasklist.update();
                        update_end();
                    }
                }
                tasklist.terminate();
                stopped();
                return 0;
            }

            void start()
            {
                m_running = true;
            }

            void pause()
            {
                m_paused = true;
                paused();
            }

            void resume()
            {
                m_paused = false;
                resumed();
            }

            void stop()
            {
                m_running = false;
            }

            bool running () { return m_running; }

            reload::tasklist<TApp>  tasklist;

            messagebus messages;

        protected:
    };
}

