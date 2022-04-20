#pragma once
#include "tasklist.h"
namespace reload {

    template <class TApp>
    class application
    {
        public:
            application()
            : m_running(false)
            , m_paused(false)
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

            virtual void before_update()
            {
            }
            virtual void update()
            {
            }
            virtual void after_update()
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
                        before_update();
                        update();
                        tasklist.update();
                        after_update();
                    }
                }
                tasklist.terminate();
                stopped();
                return 0;
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

        protected:
            bool                    m_running;
            bool                    m_paused;
    };
}

